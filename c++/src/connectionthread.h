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

#include "network.h"

#include <QThread>
#include <QTcpSocket>

class QString;
class QThread;
class QTcpSocket;

class QConnectionThread: public QThread
{
Q_OBJECT 
 private:
   QString QHost;
   int QPort;
   QTcpSocket *Network;
 public:
   void run();
   QConnectionThread(QString Host, int Port);
   ~QConnectionThread();
 public slots:
   void slotConnected();
   void slotDisconnected();
   void slotError(QAbstractSocket::SocketError socketError)
   void slotStateChanged(QAbstractSocket::SocketState state);
 signals:
   void onChangeStatus();
   void setProgressBarEnable(bool b);
   void setProgressBarRange(int min, int max);
   void setProgressBarValue(int value);
};
