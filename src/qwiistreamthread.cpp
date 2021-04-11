/***************************************************************************
 *   Copyright (C) 2008-2021 by Bartlomiej Burdukiewicz                    *
 *   dev.strikeu@gmail.com                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "qwiistreamthread.h"

#include <QDataStream>
#include <QTcpSocket>

#include <memory>

namespace homebrew {
namespace protocol {
constexpr auto major = 0;
constexpr auto minor = 5;
constexpr auto port = 4299;
constexpr auto mtu = 1400;
} // namespace protocol
} // namespace homebrew

constexpr auto CONNECT_TIMEOUT = 10000;

struct packets {
	QVector<QByteArray> data;
	std::uint64_t size{};
};

auto split_blob(const QByteArray &blob) noexcept -> packets {
	QDataStream stream(blob);

	packets ret;

	while (!stream.atEnd()) {
		QByteArray array;
		array.resize(homebrew::protocol::mtu);
		array.resize(stream.readRawData(array.data(), array.size()));
		ret.size += array.size();
		ret.data.push_back(std::move(array));
	}

	return ret;
}

auto block_write_qt(QAbstractSocket *socket, const QByteArray &data, const int timeout = 3000) -> int {
	socket->write(data);
	socket->flush();
	socket->waitForBytesWritten(timeout);

	return data.size();
}

auto block_write(QAbstractSocket *socket, const char *data, int size, const int timeout = 3000) -> int {
	return block_write_qt(socket, QByteArray::fromRawData(data, size), timeout);
}

QWiiStreamThread::QWiiStreamThread(const QString &hostname, QByteArray &&blob)
		: m_hostname(hostname)
		, m_blob(blob) {
}

QWiiStreamThread::~QWiiStreamThread() {
	wait();
}

void QWiiStreamThread::write_datagram(QAbstractSocket *socket, const std::array<unsigned char, 4> datagram) {
	block_write(socket, reinterpret_cast<const char *>(datagram.data()), datagram.size());
}

void QWiiStreamThread::run() {
	auto socket = std::make_unique<QTcpSocket>();

	socket->connectToHost(m_hostname, homebrew::protocol::port);
	if (!socket->waitForConnected(CONNECT_TIMEOUT))
		return;

	auto packets = split_blob(m_blob);
	auto calc_size = [&](auto &&size) {
		std::array<unsigned char, 4> ret;
		ret[0] = (size >> 24) & 0xff;
		ret[1] = (size >> 16) & 0xff;
		ret[2] = (size >> 8) & 0xff;
		ret[3] = (size >> 0) & 0xff;
		return ret;
	};

	write_datagram(socket.get(), {'H', 'A', 'X', 'X'});
	write_datagram(socket.get(), {homebrew::protocol::major, homebrew::protocol::minor, 0, 0});
	write_datagram(socket.get(), calc_size(packets.size));
	write_datagram(socket.get(), calc_size(0));

	for (auto total = 0; auto &&datagram : packets.data) {
		total += block_write_qt(socket.get(), datagram);
		emit progressBarPosition(total);
	}

	if (status == 0)
		transferDone();
	else
		transferFail(errorName);
}
