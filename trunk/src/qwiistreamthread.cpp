/***************************************************************************
 *   Copyright (C) 2008 by Bartlomiej Burdukiewicz                         *
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
#include <QMetaType>

const qint8 ok = 0x00;
const qint8 fail = 0x01;

const qint16 wait_time = 500;

QWiiStreamThread::QWiiStreamThread(QString host, qint8 proto, QFile *file)
{
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
    this->setTerminationEnabled(true);
    hostname = host;
    protocol = proto;
    status = ok;
    fileStream = file;
    errorName = QString("");
}

QWiiStreamThread::~QWiiStreamThread()
{
}

void QWiiStreamThread::run()
{
    tcpSocket = new QTcpSocket();
    connect(tcpSocket, SIGNAL(connected()), this, SLOT(slotConnected()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));
    tcpSocket->connectToHost(hostname, HOMEBREW_PORT);
    exec();
    if (status == ok)
        tcpSocket->waitForDisconnected(3000);
    errorName = tcpSocket->errorString();
    delete tcpSocket;

    if (status == ok)
        transferDone(); else
            transferFail(errorName);
}

void QWiiStreamThread::slotConnected()
{
    readFile = new QDataStream(fileStream);
    readed, total = 0;
    unsigned char datagram[4];
    char header[4];
    header[0] = 'H';
    header[1] = 'A';
    header[2] = 'X';
    header[3] = 'X';
    tcpSocket->write((const char *) &header, sizeof(header));
    tcpSocket->flush();
    datagram[0] = 0x00;
    datagram[1] = 0x03;
    datagram[2] = (0x00 >> 0x08) && 0xFF;
    datagram[3] = 0x00 && 0xFF;
    tcpSocket->write((const char *) &datagram, sizeof(datagram));
    tcpSocket->flush();
    datagram[0] = fileStream->size() >> 0x18;
    datagram[1] = fileStream->size() >> 0x10;
    datagram[2] = fileStream->size() >> 0x08;
    datagram[3] = fileStream->size();
    tcpSocket->write((const char *) &datagram, sizeof(datagram));
    connect(tcpSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
    tcpSocket->flush();
}

void QWiiStreamThread::bytesWritten(qint64 value)
{
    emit progressBarPosition(total);
    if (readFile->atEnd() == true)
    {
        delete readFile;
        disconnect(tcpSocket, 0, 0, 0);
        status = ok;
        quit();
        return;
    }
    char buffer[1400];
    readed = readFile->readRawData(buffer, sizeof(buffer));
    total += readed;
    tcpSocket->write(buffer, readed);
}

void QWiiStreamThread::slotError(QAbstractSocket::SocketError socketError)
{
    errorName = tcpSocket->errorString();
    status = fail;
    quit();
}
