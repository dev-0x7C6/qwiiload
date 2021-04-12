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

#include "uploadthread.h"

#include <QDataStream>
#include <QTcpSocket>

#include <memory>

namespace homebrew {
namespace connection {
constexpr auto timeout = 4000;
}

namespace protocol {
constexpr auto major = 0;
constexpr auto minor = 5;
constexpr auto port = 4299;
constexpr auto mtu = 1400;
} // namespace protocol
} // namespace homebrew

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

auto block_write_qt(QAbstractSocket *socket, const QByteArray &data, const int timeout = homebrew::connection::timeout) -> int {
	socket->write(data);
	socket->flush();
	socket->waitForBytesWritten(timeout);

	return data.size();
}

auto block_write(QAbstractSocket *socket, const char *data, int size, const int timeout = homebrew::connection::timeout) -> int {
	return block_write_qt(socket, QByteArray::fromRawData(data, size), timeout);
}

UploadThread::UploadThread(const QString &hostname, QByteArray &&blob)
		: m_hostname(hostname)
		, m_blob(blob) {
	m_progress.size = blob.size();
}

UploadThread::~UploadThread() {
	wait();
}

auto UploadThread::upload() noexcept -> upload_status {
	auto socket = std::make_unique<QTcpSocket>();

	socket->connectToHost(m_hostname, homebrew::protocol::port);
	if (!socket->waitForConnected(homebrew::connection::timeout))
		return upload_status::connection_timeout;

	auto packets = split_blob(m_blob);
	auto calc_size = [&](auto &&size) {
		std::array<unsigned char, 4> ret;
		ret[0] = (size >> 24) & 0xff;
		ret[1] = (size >> 16) & 0xff;
		ret[2] = (size >> 8) & 0xff;
		ret[3] = (size >> 0) & 0xff;
		return ret;
	};

	std::array<const std::array<unsigned char, 4>, 4> datagrams{{{'H', 'A', 'X', 'X'},
		{homebrew::protocol::major, homebrew::protocol::minor, 0, 0},
		calc_size(packets.size),
		calc_size(0)}};

	for (auto &&datagram : datagrams)
		if (!write_datagram(socket.get(), datagram))
			return upload_status::error;

	for (auto &&datagram : packets.data) {
		const auto ret = block_write_qt(socket.get(), datagram);
		if (ret == 0)
			return upload_status::error;
		m_progress.uploaded += ret;
	}

	return upload_status::successful;
}

auto UploadThread::write_datagram(QAbstractSocket *socket, const std::array<unsigned char, 4> datagram) noexcept -> bool {
	return block_write(socket, reinterpret_cast<const char *>(datagram.data()), datagram.size()) == datagram.size();
}

void UploadThread::run() {
	m_progress.status = upload();
}
