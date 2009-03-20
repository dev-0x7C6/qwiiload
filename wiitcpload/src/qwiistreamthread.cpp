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

const qint16 wait_time = 200;

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
    disconnect(tcpSocket, 0, 0, 0);
    delete tcpSocket;

    switch (status)
    {
        case ok:
            {
                emit transferDone();
            }
            break;
        case fail:
            {
                emit transferFail(errorName);
            }
            break;
    }

}

void QWiiStreamThread::slotConnected()
{
    switch (protocol)
    {
        case HOMEBREW_PROTO:
            {
                 unsigned char datagram[4];
                 char header[4];
                 header[0] = 'H';
                 header[1] = 'A';
                 header[2] = 'X';
                 header[3] = 'X';
                 tcpSocket->write((const char *) &header, sizeof(header));
                 while (tcpSocket->bytesToWrite() != 0) usleep(wait_time);
                 datagram[0] = 0x00;
                 datagram[1] = 0x03;
                 datagram[2] = (0x00 >> 0x08) && 0xFF;
                 datagram[3] = 0x00 && 0xFF;
                 tcpSocket->write((const char *) &datagram, sizeof(datagram));
                 while (tcpSocket->bytesToWrite() != 0) usleep(wait_time);
                 datagram[0] = fileStream->size() >> 0x18;
                 datagram[1] = fileStream->size() >> 0x10;
                 datagram[2] = fileStream->size() >> 0x08;
                 datagram[3] = fileStream->size();
                 tcpSocket->write((const char *) &datagram, sizeof(datagram));
                 while (tcpSocket->bytesToWrite() != 0) usleep(wait_time);
            } break;
    }


    QDataStream *readFile = new QDataStream(fileStream);

    qint64 readed, total = 0;

    while (!readFile->atEnd())
    {
        if (errorName.count() > 0)
        {
            delete readFile;
            fileStream->close();
            delete fileStream;
            quit();
        }
        readed = readFile->readRawData(buffer, sizeof(buffer));
        total += readed;
        tcpSocket->write(buffer, readed);
        while (tcpSocket->bytesToWrite() != 0) usleep(wait_time);
        emit progressBarPosition(total);
    }

    delete readFile;
    fileStream->close();
    delete fileStream;

    status = ok;

    quit();
}

void QWiiStreamThread::slotError(QAbstractSocket::SocketError socketError)
{
    errorName = tcpSocket->errorString();
    status = fail;
    quit();
}
