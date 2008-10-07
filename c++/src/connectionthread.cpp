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

#include "connectionthread.h"
#include <QMetaType>

#include <QDataStream>
#include <QFile>

class QDataStream;
class QFile;
class QFileInfo;

QConnectionThread::QConnectionThread(QObject *parent):QThread(parent){
 qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
 qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
}

QConnectionThread::~QConnectionThread(){}


void QConnectionThread::run()
{
 Network = new QTcpSocket();
 connect(Network, SIGNAL(connected()), this, SLOT(slotConnected()));
 connect(Network, SIGNAL(connectionClosed()), this, SLOT(slotDisconnected()));
 connect(Network, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));
 connect(Network, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(slotStateChanged(QAbstractSocket::SocketState)));
 connect(Network, SIGNAL(hostFound()), this, SLOT(slotHostFound()));
 Network->connectToHost(wiiHost, wiiPort);
 exec();
}

void QConnectionThread::slotConnected()
{
 unsigned char datagram[4];
 if (wiiPort == 4299)
 {
  char header[4];
  header[0] = 'H';
  header[1] = 'A';
  header[2] = 'X';
  header[3] = 'X';
  Network->write((const char *)&header, sizeof(header));
  datagram[0] = 0;
  datagram[1] = 3;
  datagram[2] = (0 >> 8) && 0xFF;
  datagram[3] = 0 && 0xFF;
  Network->write((const char *)&datagram, sizeof(datagram));
 }

 QFile file(wiiFile);
 if (!file.open(QIODevice::ReadOnly))
 {
  return;
 }

 int FileSize = file.size();
 datagram[0] = FileSize >> 24;
 datagram[1] = FileSize >> 16;
 datagram[2] = FileSize >> 8;
 datagram[3] = FileSize;
 Network->write((const char *)&datagram, sizeof(datagram));
 emit setProgressBarEnabled(TRUE);
 emit setProgressBarMax(FileSize);
 emit setProgressBarMin(0);
 emit setProgressBarValue(0);

 char buffer[256];
 int readed, total = 0;
 QDataStream readfile(&file);

 currentStatus = "Stream data...";
 emit onChangeStatus(currentStatus);

 while (!readfile.atEnd()) {
  readed = readfile.readRawData(buffer, sizeof(buffer));
  total += readed;
  Network->write((const char *)&buffer, readed);
  emit setProgressBarValue(total);
 }
 emit setProgressBarEnabled(FALSE);
 Network->disconnectFromHost();
}

void QConnectionThread::slotDisconnected(){}

void QConnectionThread::slotError(QAbstractSocket::SocketError error){}
void QConnectionThread::slotHostFound(){}

void QConnectionThread::slotStateChanged(QAbstractSocket::SocketState state){
 switch(state) {
  case QAbstractSocket::UnconnectedState: currentStatus = "Disconnected"; break;
  case QAbstractSocket::HostLookupState: currentStatus = "Resolving hostname..."; break;
  case QAbstractSocket::ConnectingState: currentStatus = "Connecting..."; break;
  case QAbstractSocket::ConnectedState: currentStatus = "Connected"; break;
  case QAbstractSocket::ClosingState: currentStatus = "Waiting for close connection..."; break;
 }
 emit onChangeStatus(currentStatus);
}
