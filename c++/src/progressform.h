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

#include "ui_progressform.h"
#include <QThread>
#include "network.h"

using namespace Ui;

class QDialog;
class QString;
class QWidget;
class QThread;

class QConnectionThread: public QThread
{
Q_OBJECT 
 private:
   QString QHost;
   int QPort;
   WiiConnection *wiiConnection;
 public:
   void run();
   QConnectionThread(QString Host, int Port);
};

class ProgressForm: public QDialog
{
Q_OBJECT
 private:
   QConnectionThread *ConnectionThread;
 public:
   progressform ui;
   ProgressForm(QWidget * parent = 0, Qt::WFlags f = 0);
   ~ProgressForm(){};
};
