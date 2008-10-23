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

#include <QDataStream>


class QDataStream;
class QFileInfo;


QStreamThread::QStreamThread(QObject *parent):QThread(parent){
 qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
 qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
 setTerminationEnabled(FALSE);
}

QStreamThread::~QStreamThread(){}

void QStreamThread::setSock(QTcpSocket *socket)
{
 QMutex mutexLock;
 mutexLock.lock();
 pSocket = socket;
 mutexLock.unlock();
}

void QStreamThread::setFile(QString fileName)
{
 QMutex mutexLock;
 mutexLock.lock();
 pFileName = fileName;
 mutexLock.unlock();
}

void QStreamThread::run()
{
 QMutex mutexLock;

 mutexLock.lock();
 QTcpSocket *Network = pSocket;
 QString fileName = pFileName;
 mutexLock.unlock();

 QFile file(fileName);
 if (!file.open(QIODevice::ReadOnly)) {
  emit done(FALSE);
  return;
 }
 QDataStream readfile(&file);

 emit statusMessage("Stream data...");

 unsigned char datagram[4];
 if (Network->peerPort() == 4299)
 {
  char header[4];
  header[0] = 'H';
  header[1] = 'A';
  header[2] = 'X';
  header[3] = 'X';
  Network->write((const char *)&header, sizeof(header));
  if (Network->state() != QAbstractSocket::ConnectedState) return;
  datagram[0] = 0;
  datagram[1] = 3;
  datagram[2] = (0 >> 8) && 0xFF;
  datagram[3] = 0 && 0xFF;
  Network->write((const char *)&datagram, sizeof(datagram));
  if (Network->state() != QAbstractSocket::ConnectedState) return;
 }

 datagram[0] = file.size() >> 24;
 datagram[1] = file.size() >> 16;
 datagram[2] = file.size() >> 8;
 datagram[3] = file.size();
 Network->write((const char *)&datagram, sizeof(datagram));

#ifdef Q_OS_UNIX
 if ((!Network->waitForBytesWritten(timeOut)) && (Network->bytesToWrite() != 0))
 {
  emit done(FALSE);
  return;
 }
#else
 Network->flush();
 msleep(1);
#endif
 if (Network->state() != QAbstractSocket::ConnectedState) return;

 emit pbSetRangeSig(0, file.size());
 emit pbSetValueSig(0);
 emit pbSetEnabledSig(TRUE);

 char buffer[1023];
 quint64 readed, total = 0;

 while (!readfile.atEnd()) {
  readed = readfile.readRawData(buffer, sizeof(buffer));
  total += readed;
  if (Network->state() != QAbstractSocket::ConnectedState) return;
  Network->write((const char *)&buffer, readed);
#ifdef Q_OS_UNIX
 if ((!Network->waitForBytesWritten(timeOut)) && (Network->bytesToWrite() != 0))
 {
  emit done(FALSE);
  return;
 }
#else
 Network->flush();
 msleep(1);
#endif
  emit pbSetValueSig(total);
 }
 emit pbSetValueSig(total);

#ifndef Q_OS_UNIX
 Network->waitForDisconnected(-1);
#endif

 emit done(TRUE);
}
