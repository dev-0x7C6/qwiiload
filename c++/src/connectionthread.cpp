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

#include "threads.h"
#include <QMetaType>

#include <QDataStream>
#include <QApplication>

class QDataStream;
class QFileInfo;

QConnectionThread::QConnectionThread(QObject *parent, QStreamThread *thread):QThread(parent){
 qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
 qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
 StreamThread = thread;
}

QConnectionThread::~QConnectionThread(){}

void QConnectionThread::run(){
 Network = new QTcpSocket(this);
 connect(Network, SIGNAL(connected()), this, SLOT(slotConnected()));
 connect(Network, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));
 connect(Network, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(slotStateChanged(QAbstractSocket::SocketState)));
 Network->connectToHost(wiiHost, wiiPort); 
 exec();
}

void QConnectionThread::disconnectAnyway(){ 
 StreamThread->quit();
 Network->disconnectFromHost();
}

void QConnectionThread::updateProgressBar(int value)
{
 emit setProgressBarValue(value);
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
  Network->waitForBytesWritten(-1);
  datagram[0] = 0;
  datagram[1] = 3;
  datagram[2] = (0 >> 8) && 0xFF;
  datagram[3] = 0 && 0xFF;
  Network->write((const char *)&datagram, sizeof(datagram));
  Network->waitForBytesWritten(-1);
 }

 QFile *file;
 file = new QFile(wiiFile);
 if (!file->open(QIODevice::ReadOnly)) {
  emit transferFail("Can't open file to the read");
  return;
 }

 int FileSize = file->size();
 datagram[0] = FileSize >> 24;
 datagram[1] = FileSize >> 16;
 datagram[2] = FileSize >> 8;
 datagram[3] = FileSize;
 Network->write((const char *)&datagram, sizeof(datagram));
 Network->waitForBytesWritten(-1);
 emit setProgressBarState(TRUE, FileSize, 0, 0);

 currentStatus = "Stream data...";
 emit onChangeStatus(currentStatus);

 connect(StreamThread, SIGNAL(updateProgressBar(int)), this, SLOT(updateProgressBar(int)));
 StreamThread->start();
}

void QConnectionThread::slotError(QAbstractSocket::SocketError error){
 QString strError = Network->errorString();
 emit transferFail(strError);
 terminate();
}

void QConnectionThread::slotStateChanged(QAbstractSocket::SocketState state){
 switch(state) {
  case QAbstractSocket::UnconnectedState: currentStatus = "Disconnected"; break;
  case QAbstractSocket::HostLookupState: currentStatus = "Resolving hostname..."; break;
  case QAbstractSocket::ConnectingState: currentStatus = "Connecting..."; break;
  case QAbstractSocket::ConnectedState: currentStatus = "Connected"; break;
  case QAbstractSocket::ClosingState: currentStatus = "Waiting for close connection..."; break;
 }
 emit onChangeStatus(currentStatus);
 if (state == QAbstractSocket::UnconnectedState || Network->error() ) 
 {
  //emit transferDone();
 // terminate();
 }
}
