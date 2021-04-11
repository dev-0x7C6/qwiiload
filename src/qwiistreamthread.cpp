/***************************************************************************
 *   Copyright (C) 2008-2009 by Bartlomiej Burdukiewicz                    *
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

#include <QAbstractSocket>
#include <QDataStream>
#include <QMetaType>

QWiiStreamThread::QWiiStreamThread(QString host, qint8 proto, QFile *file) {
	qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
	qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
	this->setTerminationEnabled(true);
	hostname = host;
	protocol = proto;
	status = 0;
	fileStream = file;
	errorName = QString("");
}

QWiiStreamThread::~QWiiStreamThread() {
}

void QWiiStreamThread::run() {
	tcpSocket = new QTcpSocket();
	connect(tcpSocket, SIGNAL(connected()), this, SLOT(slotConnected()));
	connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));
	tcpSocket->connectToHost(hostname, HOMEBREW_PORT);
	exec();
	if (status == 0)
		tcpSocket->waitForDisconnected(3000);
	errorName = tcpSocket->errorString();
	delete tcpSocket;

	if (status == 0)
		transferDone();
	else
		transferFail(errorName);
}

void QWiiStreamThread::slotConnected() {
	readFile = new QDataStream(fileStream);

	auto args_length = 0;

	std::array<unsigned char, 4> datagram{'H', 'A', 'X', 'X'};

	auto write_datagram = [&]() {
		tcpSocket->write(reinterpret_cast<const char *>(datagram.data()), datagram.size());
		tcpSocket->flush();
	};

	write_datagram();

	datagram[0] = 0;
	datagram[1] = 5;
	datagram[2] = (args_length >> 8) & 0xff;
	datagram[3] = args_length & 0xff;
	write_datagram();

	const auto size = fileStream->size();
	datagram[0] = (size >> 24) & 0xff;
	datagram[1] = (size >> 16) & 0xff;
	datagram[2] = (size >> 8) & 0xff;
	datagram[3] = size & 0xff;
	write_datagram();

	datagram[0] = 0;
	datagram[1] = 0;
	datagram[2] = 0;
	datagram[3] = 0;
	connect(tcpSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
	write_datagram();
}

void QWiiStreamThread::bytesWritten(qint64) {
	emit progressBarPosition(total);
	if (readFile->atEnd() == true) {
		delete readFile;
		disconnect(tcpSocket, 0, 0, 0);
		status = 0;
		quit();
		return;
	}

	char buffer[4 * 1024];
	readed = readFile->readRawData(buffer, sizeof(buffer));
	total += readed;
	tcpSocket->write(buffer, readed);
}

void QWiiStreamThread::slotError(QAbstractSocket::SocketError) {
	errorName = tcpSocket->errorString();
	status = -1;
	quit();
}
