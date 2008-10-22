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

void QStreamThread::run(){
 emit statusMessage("Stream data...");
 breakLoop = FALSE;
 unsigned char datagram[4];
 if (Network->peerPort() == 4299)
 {
  char header[4];
  header[0] = 'H';
  header[1] = 'A';
  header[2] = 'X';
  header[3] = 'X';
  Network->write((const char *)&header, sizeof(header));
  //waitForBytesWritten(-1);
  datagram[0] = 0;
  datagram[1] = 3;
  datagram[2] = (0 >> 8) && 0xFF;
  datagram[3] = 0 && 0xFF;
  Network->write((const char *)&datagram, sizeof(datagram));
  //Network->waitForBytesWritten(-1);
 }
 QFile file(sourceFile);
 if (!file.open(QIODevice::ReadOnly)) {
//  emit fail();
  return;
 }

 int FileSize = file.size();
 datagram[0] = FileSize >> 24;
 datagram[1] = FileSize >> 16;
 datagram[2] = FileSize >> 8;
 datagram[3] = FileSize;
 Network->write((const char *)&datagram, sizeof(datagram));
// Network->waitForBytesWritten(-1);

 emit progressSetup(TRUE, FileSize, 0, 0);

 char buffer[1023];
 int readed, total = 0;
 QDataStream readfile(&file);

 while (!readfile.atEnd()) {
  if (breakLoop == TRUE) return;
  readed = readfile.readRawData(buffer, sizeof(buffer));
  total += readed;
  Network->write((const char *)&buffer, readed);
  Network->flush();
  //waitForBytesWritten(-1);
// if (Network->bytesToWrite() != 0)
//  {
//   if (!Network->waitForBytesWritten(timeOut))
//   {
//    emit fail();
//    return;
//   }
//  }
  emit progressValue(total);
 }
 Network->waitForDisconnected(-1);
 Network->disconnectFromHost();
 emit done();
}
