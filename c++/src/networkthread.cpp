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

QNetworkThread::QNetworkThread(QObject *parent):QThread(parent){
 qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
 qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
 setTerminationEnabled(FALSE);
}

QNetworkThread::~QNetworkThread(){}

void QNetworkThread::setDestPort(quint16 port)
{
 QMutex mutexLock;
 mutexLock.lock();
 destPort = port;
 mutexLock.unlock();
}

void QNetworkThread::setFilename(QString file)
{
 QMutex mutexLock;
 mutexLock.lock();
 fileName = file;
 mutexLock.unlock();
}

void QNetworkThread::setHostname(QString host)
{
 QMutex mutexLock;
 mutexLock.lock();
 hostName = host;
 mutexLock.unlock();
}

void QNetworkThread::run()
{
 QMutex mutexLock;
 mutexLock.lock();
 QString hostname = hostName;
 QString filename = fileName;
 quint16 destport = destPort;
 mainResult = FALSE;
 mutexLock.unlock();

 Network = new QTcpSocket();
 streamThread.setSock(Network);
 streamThread.setFile(filename);

 connect(&streamThread, SIGNAL(pbSetEnabledSig(bool)), this, SLOT(pbSetEnabled(bool)));
 connect(&streamThread, SIGNAL(pbSetValueSig(quint64)), this, SLOT(pbSetValue(quint64)));
 connect(&streamThread, SIGNAL(pbSetRangeSig(quint64,quint64)), this, SLOT(pbSetRange(quint64,quint64)));
 connect(&streamThread, SIGNAL(done(bool)), this, SLOT(slotDone(bool)));

 connect(Network, SIGNAL(connected()), this, SLOT(slotConnected()));
 connect(Network, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(slotStateChanged(QAbstractSocket::SocketState)));
 connect(Network, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));

 Network->connectToHost(hostname, destport);
 exec();
 Network->disconnectFromHost();
 streamThread.wait();
 QString msg;
 if (mainResult == TRUE)
 {
  msg = "Transfer done.";
 } else {
  msg = "Transfer failed. " + Network->errorString();
 }
 emit endWork(mainResult, msg);
 disconnect(Network, 0, 0, 0);
 disconnect(&streamThread, 0, 0, 0);
}

void QNetworkThread::slotConnected()
{
 streamThread.start();
}

void QNetworkThread::slotDone(bool result)
{
 QMutex mutexLock;
 mutexLock.lock();
 mainResult = result;
 mutexLock.unlock();
 quit();
}

void QNetworkThread::onError(QAbstractSocket::SocketError error)
{
 quit();
}
