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

#pragma once

#include <QThread>

#include <atomic>
#include <cstdint>

class QAbstractSocket;

enum class upload_status {
	progress,
	connection_timeout,
	error,
	successful,
};

struct upload_progress {
	std::atomic<upload_status> status{upload_status::progress};
	std::atomic<std::int32_t> uploaded{};
	std::atomic<std::int32_t> size{};
};

class UploadThread final : public QThread {
	Q_OBJECT
public:
	UploadThread(const QString &hostname, QByteArray &&blob);
	virtual ~UploadThread();

	auto progress() const noexcept -> const upload_progress & { return m_progress; }

protected:
	auto upload() noexcept -> upload_status;
	void run() final;

private:
	auto write_datagram(QAbstractSocket *socket, std::array<unsigned char, 4> data) noexcept -> bool;

private:
	const QString m_hostname;
	const QByteArray m_blob;

	upload_progress m_progress;
};
