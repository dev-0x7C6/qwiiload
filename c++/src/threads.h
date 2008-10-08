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

class QString;
class QThread;
class QTcpSocket;
class QFile;

class QConnectionThread: public QThread
{
Q_OBJECT
 private:
   QString wiiHost;
   QString wiiFile;
   int wiiPort;
   QString currentStatus;
 public:
   QTcpSocket *Network;

   QConnectionThread(QObject *parent = 0, QStreamThread *thread = 0);
   ~QConnectionThread();

   void setHost(QString Host){wiiHost = Host;};
   void setFile(QString File){wiiFile = File;};
   void setPort(int Port){wiiPort = Port;};
   void disconnectAnyway();
 protected:
    void run();
 signals:
   void transferDone();
   void transferFail(QString error);

   void onChangeStatus(QString status);
   void setProgressBarState(bool enabled, int max, int min, int value);
   void setProgressBarValue(int value);
};
