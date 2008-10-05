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

#include "ui_mainform.h"
#include "connectionthread.h"
#include "manager.h"
#include "about.h"


#include <QMessageBox>
#include <QFileDialog>
#include <QTextCodec>
#include <QTcpSocket>
#include <QFile>
#include <QFileInfo>
#include <QDataStream>


using namespace Ui;

class QDialog;
class QThread;
class QWidget;

const QString mainWindowTitle = "WiiTCPLoadGUI v0.02 (broken)";

class MainForm: public QMainWindow
{
Q_OBJECT
 private:
   QFileDialog *FileDialog;
 public:
   mainform ui;
   MainForm(QWidget * parent = 0, Qt::WFlags f = 0 );
   ~MainForm();
 public slots:
// Form
   void slotReadyBtnClicked();
   void slotOpenFileClicked();
// MainMenu
   void slotAboutProgram();
   void slotActionExit();
   void slotActionManagerRun();
};
