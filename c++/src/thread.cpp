/***************************************************************************
 *   Copyright (C) 2008 by Bartlomiej Burdukiewicz                         *
 *   strikeu@poczta.linux.pl                                               *
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

#include "thread.h"

#include <QMetaType>

QNetworkThread::QNetworkThread(QObject *parent):QThread(parent){
 qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
 qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
 setTerminationEnabled(FALSE);
}

QNetworkThread::~QNetworkThread(){}

void QNetworkThread::run(){
 mutex.lock();
 QString hostname = wiiHost;
 quint16 destport = wiiPort;
 mutex.unlock();

 Network = new QTcpSocket();
 connect(Network, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onState(QAbstractSocket::SocketState)));
 connect(Network, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError()));
 connect(Network, SIGNAL(connected()), this, SLOT(connected()));

 Network->connectToHost(hostname, destport);
 exec();
 disconnect(Network, 0, 0, 0);
}

void QNetworkThread::connected()
{
 mutex.lock();
 QString filename = sourceFile;
 mutex.unlock();

 emit setStatus("Stream data...");
 QFile file(filename);
 if (!file.open(QIODevice::ReadOnly)) {
  emit sendMessage("Can't open file to read");
  return;
 }

 unsigned char datagram[4];
 if (Network->peerPort() == 4299)
 {
  char header[4];
  header[0] = 'H';
  header[1] = 'A';
  header[2] = 'X';
  header[3] = 'X';
  if (!Network->write((const char *)&header, sizeof(header)))
  {
   return;
  }
  datagram[0] = 0;
  datagram[1] = 3;
  datagram[2] = (0 >> 8) && 0xFF;
  datagram[3] = 0 && 0xFF;
  if (!Network->write((const char *)&datagram, sizeof(datagram)))
  {
   return;
  }
 }
 int FileSize = file.size();
 datagram[0] = FileSize >> 24;
 datagram[1] = FileSize >> 16;
 datagram[2] = FileSize >> 8;
 datagram[3] = FileSize;
 if (!Network->write((const char *)&datagram, sizeof(datagram)))
 {
  return;
 }

 emit pbSetRange(0, FileSize);
 emit pbSetValue(0);
 emit pbSetEnabled(TRUE);

 char buffer[1023];
 quint64 readed, total = 0;

 QDataStream readfile(&file);
 while (!readfile.atEnd()) {
  readed = readfile.readRawData(buffer, sizeof(buffer));
  total += readed;
  if (!Network->write((const char *)&buffer, readed))
  {
   return;
  }
  emit pbSetValue(total);
 }
 emit pbSetValue(total);

 disconnect(Network, SIGNAL(error(QAbstractSocket::SocketError)), 0, 0);
 connect(Network, SIGNAL(disconnected()), this, SLOT(disconnectedFromHost()));
}

void QNetworkThread::disconnectedFromHost()
{
 emit sendMessage("Done");
}

void QNetworkThread::onError(QAbstractSocket::SocketError id)
{
}

void QNetworkThread::onState(QAbstractSocket::SocketState id)
{
}
