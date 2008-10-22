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

#include <QThread>
#include <QTcpSocket>
#include <QFile>
#include <QMetaType>
#include <QMutex>

class QString;
class QThread;
class QTcpSocket;
class QFile;
class QMutex;

const quint16 timeOut = 5000;

class QNetworkThread: public QThread
{
Q_OBJECT
 private:
   QString wiiHost;
   quint16 wiiPort;
   QTcpSocket *Network;
   QMutex mutex;
 public:
   QNetworkThread(QObject *parent = 0);
   ~QNetworkThread();
   void setHost(QString host){ wiiHost = host; };
   void setPort(quint16 port){ wiiPort = port; };
 protected:
    void run();
 private slots:
  void onConnected();
  void onError(QAbstractSocket::SocketError id);
  void onState(QAbstractSocket::SocketState id);
 signals:
};
