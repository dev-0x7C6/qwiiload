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

#include <QTcpSocket>

class QTcpSocket;

class WiiConnection: public QObject
{
Q_OBJECT
 public:
   QTcpSocket *Network;
   WiiConnection();
   ~WiiConnection();
 public slots:
   void slotNetworkConnected();
   void slotNetworkDisconnected();
   void slotNetworkError(QAbstractSocket::SocketError error);
   void slotNetworkStateChanged(QAbstractSocket::SocketState state);
 signals:
   void signalConnected();
   void signalDisconnected();
   void signalError(QAbstractSocket::SocketError error);
};
