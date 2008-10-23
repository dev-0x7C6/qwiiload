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

void QNetworkThread::run(){
 Network = new QTcpSocket();
 QStreamThread streamThread;
 streamThread.setSock(Network);
 //nstreamThread.setFile(); 
 connect(&streamThread, SIGNAL(pbSetEnabledSig(bool)), this, SLOT(pbSetEnabled(bool)));
 connect(&streamThread, SIGNAL(pbSetValueSig(quint64)), this, SLOT(pbSetValue(quint64)));
 connect(&streamThread, SIGNAL(pbSetRangeSig(quint64,quint64)), this, SLOT(pbSetRange(quint64,quint64)));

 connect(Network, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onState(QAbstractSocket::SocketState)));
 connect(Network, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError()));
 connect(Network, SIGNAL(connected()), this, SLOT(onConnected()));
 Network->connectToHost(wiiHost, wiiPort);
 exec();
 disconnect(Network, 0, 0, 0);
 disconnect(&streamThread, 0, 0, 0);
}
