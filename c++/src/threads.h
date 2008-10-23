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
 public:
   QNetworkThread(QObject *parent = 0);
   ~QNetworkThread();
   void setHost(QString host){ wiiHost = host; };
   void setPort(quint16 port){ wiiPort = port; };
 protected:
   void run();
 private slots:
// Progress Bar
   void pbSetEnabled(bool opt){ emit pbSetEnabledSig(opt); };
   void pbSetRange(quint64 min, quint64 max){ emit pbSetRangeSig(min, max); };
   void pbSetValue(quint64 value){ emit pbSetValueSig(value); };
//
 signals:
   void pbSetEnabledSig(bool opt);
   void pbSetRangeSig(quint64 min, quint64 max);
   void pbSetValueSig(quint64 value);



 private slots:
   void onConnected(){ emit connected(Network); };
   void onError(){ emit error(Network->errorString()); };
   void onState(QAbstractSocket::SocketState value){ emit state(value); };

 signals:
   void connected(QTcpSocket *socket);
   void state(QAbstractSocket::SocketState state);
   void error(QString error);
};


class QStreamThread: public QThread
{
Q_OBJECT
 private:
   QTcpSocket *pSocket;
   QString pFileName;
   bool breakLoop;
 public:
   QStreamThread(QObject *parent = 0);
   ~QStreamThread();
   void setSock(QTcpSocket *socket){ pSocket = socket; };
   void setFile(QString fileName){ pFileName = fileName; };
 protected:
   void run();
 public slots:
   void cancel(){ breakLoop = TRUE; };
 signals:
// Progress Bar
   void pbSetEnabledSig(bool opt);
   void pbSetRangeSig(quint64 min, quint64 max);
   void pbSetValueSig(quint64 value);
 signals:
   void done();
   void fail();
//
   void statusMessage(QString msg);
};

