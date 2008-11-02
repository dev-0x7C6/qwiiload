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

const int timeOut = 3000;

class QStreamThread: public QThread
{
Q_OBJECT
 private:
   QTcpSocket *pSocket;
   QString pFileName;
   bool accepted;
 public:
   QStreamThread(QObject *parent = 0);
   ~QStreamThread();
   void setSock(QTcpSocket *socket);
   void setFile(QString fileName);
   void acceptTransfer();
 protected:
   void run();
 signals:
// Progress Bar
   void pbSetEnabledSig(bool opt);
   void pbSetRangeSig(quint64 min, quint64 max);
   void pbSetValueSig(quint64 value);
//
   void done(bool result);
   void waitForAccepted();

 signals:
   void statusMessage(QString msg);
};

class QNetworkThread: public QThread
{
Q_OBJECT
 private:
   quint16 destPort;
   QString hostName;
   QString fileName;
   QTcpSocket *Network;
   QStreamThread streamThread;
   bool mainResult;
 public:
   QNetworkThread(QObject *parent = 0);
   ~QNetworkThread();
   void setDestPort(quint16 port);
   void setFilename(QString file);
   void setHostname(QString host);
   void acceptTransfer(){ streamThread.acceptTransfer(); };
 protected:
   void run();
 private slots:
// Progress Bar
   void pbSetEnabled(bool opt){ emit pbSetEnabledSig(opt); };
   void pbSetRange(quint64 min, quint64 max){ emit pbSetRangeSig(min, max); };
   void pbSetValue(quint64 value){ emit pbSetValueSig(value); };
//
   void slotWaitForAccepted(){ emit waitForAccepted(); };
   void slotConnected();
   void onError(QAbstractSocket::SocketError error);
   void slotStateChanged(QAbstractSocket::SocketState state){ emit updateState(state); };
   void slotDone(bool);
 signals:
// Progress Bar
   void pbSetEnabledSig(bool opt);
   void pbSetRangeSig(quint64 min, quint64 max);
   void pbSetValueSig(quint64 value);
// Informations
   void waitForAccepted();
//
   void updateState(QAbstractSocket::SocketState state);
   void endWork(bool result, QString msg);

// private slots:
//   void onState(QAbstractSocket::SocketState value){ emit state(value); };

// signals:
//   void connected(QTcpSocket *socket);
//   void state(QAbstractSocket::SocketState state);
//   void error(QString error);
};

