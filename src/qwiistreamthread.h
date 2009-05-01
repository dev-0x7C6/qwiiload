/***************************************************************************
 *   Copyright (C) 2008-2009 by Bartlomiej Burdukiewicz                    *
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

#ifndef QWIISTREAMTHREAD_H
#define QWIISTREAMTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QFile>

const qint8 HOMEBREW_PROTO = 0x00;
const qint16 HOMEBREW_PORT = 4299;

class QWiiStreamThread : public QThread
{
    Q_OBJECT
public:
    QWiiStreamThread(QString host, qint8 proto, QFile *file);
   ~QWiiStreamThread();

private:
   QString hostname;
   qint8 protocol;

   QTcpSocket *tcpSocket;
   QFile *fileStream;
   QDataStream *readFile;
   QString errorName;

   qint8 status;
   qint64 readed, total;


private slots:
   void bytesWritten(qint64 value);

public slots:
   void slotConnected();
   void slotError(QAbstractSocket::SocketError socketError);

protected:
   void run();

signals:
   void transferDone();
   void transferFail(QString error);
   void progressBarPosition(int);

};

#endif // QWIISTREAMTHREAD_H
