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

#include "mainform.h"
#include "manager.h"
#include "about.h"

const QString mainWindowTitle = "WiiTCPLoadGUI v0.02";

MainForm::MainForm(QWidget * parent, Qt::WFlags f):QMainWindow(parent, f)
{
 QTextCodec::setCodecForTr (QTextCodec::codecForName ("UTF-8")); 
 ui.setupUi(this);
 setMaximumHeight(height());
 setMinimumHeight(height());
 setWindowTitle(mainWindowTitle);

 Network = new QTcpSocket;
 FileDialog = new QFileDialog(this);

 connect(Network, SIGNAL(connected()), this, SLOT(slotConnected()));
 connect(Network, SIGNAL(connectionClosed()), this, SLOT(slotDisconnected()));
 connect(Network, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotSocketError(QAbstractSocket::SocketError)));

 connect(ui.readyBtn, SIGNAL(clicked()), this, SLOT(slotReadyBtnClicked()));
 connect(ui.openFile, SIGNAL(clicked()), this, SLOT(slotOpenFileClicked()));
// MainMenu
 connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(slotOpenFileClicked()));
 connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(slotActionExit()));
 connect(ui.actionManagerRun, SIGNAL(triggered()), this, SLOT(slotActionManagerRun()));
 connect(ui.actionAboutProgram, SIGNAL(triggered()), this, SLOT(slotAboutProgram()));
 connect(ui.actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

}

MainForm::~MainForm()
{
 delete Network;
 delete FileDialog;
}

QString host, filename = "";
int port = 0;


void MainForm::slotConnected()
{
 ui.statusbar->showMessage("Connected");
 unsigned char datagram[4];
 if (port == 4299)
 {
 	char header[4];
 	header[0] = 'H';
 	header[1] = 'A';
 	header[2] = 'X';
 	header[3] = 'X';
 	Network->write((const char *)&header, sizeof(header));
 
 	datagram[0] = 0;
 	datagram[1] = 3;
 	datagram[2] = (0 >> 8) && 0xFF;
 	datagram[3] = 0 && 0xFF;
 	Network->write((const char *)&datagram, sizeof(datagram));
 } else
 {
  QMessageBox::information(this, trUtf8("Info"), trUtf8("Please confirm connection on your Wii"));
 }

 QFile file(filename);
 if (!file.open(QIODevice::ReadOnly))
 {
  QMessageBox::information(this, trUtf8("Info"), trUtf8("Can't open file"));
  return;
 }

 int FileSize = file.size();
 datagram[0] = FileSize >> 24;
 datagram[1] = FileSize >> 16;
 datagram[2] = FileSize >> 8;
 datagram[3] = FileSize;
 //ui.progressBar->setMaximum(FileSize);
 //ui.progressBar->setMinimum(0);
 //ui.progressBar->setValue(0);
 //ui.progressBar->setEnabled(TRUE);
 Network->write((const char *)&datagram, sizeof(datagram));

 char buffer[256];
 int readed;
 QDataStream readfile(&file);

 while (!readfile.atEnd()) {
  readed = readfile.readRawData(buffer, sizeof(buffer));
  Network->write((const char *)&buffer, readed);
 // ui.progressBar->setValue(ui.progressBar->value() + readed);
 }
// ui.progressBar->setEnabled(FALSE);
 Network->disconnectFromHost();
}


void MainForm::slotOpenFileClicked()
{
 ui.localFile->setText(FileDialog->getOpenFileName()); 
}

void MainForm::slotDisconnected()
{
 QMessageBox::information(this, trUtf8("Info"), trUtf8("Hmm"));
}

void MainForm::slotSocketError(QAbstractSocket::SocketError socketError)
{
 QMessageBox::information(this, trUtf8("Info"), Network->errorString());
}

void MainForm::slotAboutProgram()
{
 AboutForm *window = new AboutForm(this);
 window->exec();
 delete window;
}

void MainForm::slotActionExit()
{
 close();
}

void MainForm::slotActionManagerRun()
{
 ManagerForm *window;
 window = new ManagerForm(this);
 window->exec();
 delete window;
}

void MainForm::slotReadyBtnClicked()
{
 host = ui.wiiHostName->text();
 if (host == QString("")) {
  QMessageBox::warning(this, trUtf8("Warning"), trUtf8("Wii hostname is empty"));
  return;
 }
 filename = ui.localFile->text();
 QFileInfo *FileInfo;
 FileInfo = new QFileInfo(filename);
 bool fileExists = FileInfo->exists() && !FileInfo->isDir();
 delete FileInfo;

 if (fileExists == FALSE) {
  QMessageBox::critical(this, trUtf8("Critical"), trUtf8("Selected file don't exists"));
  return;
 }

 if (ui.channelSelect->currentIndex() == 0) port = 4299;
 if (ui.channelSelect->currentIndex() == 1) port = 8080;
 int status = Network->state();
 if (status != QTcpSocket::UnconnectedState)
 {
  Network->disconnectFromHost();
 }
 setWindowTitle(mainWindowTitle);
 Network->connectToHost(host, port);
}

QConnectionThread::QConnectionThread(QString Host, int Port)
{
}

void QConnectionThread::run()
{
}
