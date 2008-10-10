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

#include <QFileInfo>

class QFileInfo;

MainForm::MainForm(QWidget * parent, Qt::WFlags f):QMainWindow(parent, f)
{
 QTextCodec::setCodecForTr (QTextCodec::codecForName ("UTF-8")); 

 ui.setupUi(this);
 setMaximumHeight(height());
 setMinimumHeight(height());
 setWindowTitle(mainWindowTitle);
 FileDialog = new QFileDialog(this);
 connect(ui.readyBtn, SIGNAL(clicked()), this, SLOT(slotReadyBtnClicked()));
 connect(ui.openFile, SIGNAL(clicked()), this, SLOT(slotOpenFileClicked()));
// MainMenu
 connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(slotOpenFileClicked()));
 connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(slotActionExit()));
// connect(ui.actionManagerRun, SIGNAL(triggered()), this, SLOT(slotActionManagerRun()));
 connect(ui.actionAboutProgram, SIGNAL(triggered()), this, SLOT(slotAboutProgram()));
 connect(ui.actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

MainForm::~MainForm(){
 disconnect(&networkThread, 0, 0, 0);
 disconnect(&nstreamThread, 0, 0, 0);
 nstreamThread.cancel();
 nstreamThread.wait();
 networkThread.quit();
 networkThread.wait();
 
 delete FileDialog;
}


void MainForm::slotOpenFileClicked(){
 QString fileName = FileDialog->getOpenFileName();
 if (fileName != QString("")) ui.localFile->setText(fileName);
}

void MainForm::slotActionExit(){close();}

void MainForm::slotAboutProgram(){
 AboutForm *window = new AboutForm(this);
 window->exec();
 delete window;
}

void MainForm::slotActionManagerRun(){
 ManagerForm *window;
 window = new ManagerForm(this);
 window->exec();
 delete window;
}

void MainForm::defaultProgressBar(bool enabled, int max, int min, int value){
 ui.progressBar->setMaximum(max);
 ui.progressBar->setMinimum(min);
 ui.progressBar->setEnabled(enabled);
 ui.progressBar->setValue(value);
}

void MainForm::progressSetup(bool enabled, int max, int min, int value){ defaultProgressBar(enabled, max, min, value); }
void MainForm::progressValue(int value){ ui.progressBar->setValue(value); }

void MainForm::setReadyMode(){
 ui.readyBtn->setIcon(QIcon(QString::fromUtf8(":/actions/icons/actions/button_ok.png")));
 ui.readyBtn->setText("ready");
}

void MainForm::setCancelMode(){
 ui.readyBtn->setIcon(QIcon(QString::fromUtf8(":/actions/icons/actions/button_cancel.png")));
 ui.readyBtn->setText("cancel");
}

QString fileName;

void MainForm::slotReadyBtnClicked()
{
 if (ui.readyBtn->text() == QString("ready")) {
  QString Hostname = ui.wiiHostName->text();
  if (Hostname == QString("")) {
   QMessageBox::warning(this, trUtf8("Warning"), trUtf8("Hostname is empty"));
   return;
  }

  fileName = ui.localFile->text();
  QFileInfo *FileInfo;
  FileInfo = new QFileInfo(fileName);
  bool fileExists = FileInfo->exists() && !FileInfo->isDir();
  delete FileInfo;

  if (fileExists == FALSE) {
   QMessageBox::critical(this, trUtf8("Critical"), trUtf8("File not found"));
   return;
  }

  setCancelMode();
  defaultProgressBar(FALSE, 100, 0, 0);
  networkThread.setHost(Hostname);

  switch(ui.channelSelect->currentIndex()) {
   case 0: networkThread.setPort(4299); break;
   case 1: networkThread.setPort(8080); break;
  }
  disconnect(&networkThread, 0, 0, 0);
  disconnect(&nstreamThread, 0, 0, 0);

  connect(&networkThread, SIGNAL(connected(QTcpSocket*)), this, SLOT(onConnected(QTcpSocket*)));
  connect(&networkThread, SIGNAL(state(QAbstractSocket::SocketState)), this, SLOT(onState(QAbstractSocket::SocketState)));
  connect(&networkThread, SIGNAL(error(QString)), this, SLOT(onError(QString)));

  networkThread.start();

 } else {
  disconnect(&networkThread, 0, 0, 0);
  disconnect(&nstreamThread, 0, 0, 0);
  nstreamThread.cancel();
  nstreamThread.wait();
  networkThread.quit();
  networkThread.wait();
  ui.statusLabel->setText("Disconnected");
  defaultProgressBar(FALSE, 100, 0, 0);
  setReadyMode();
  QMessageBox::warning(this, trUtf8("Warning"), trUtf8("Transfer interrupted"));
 }
}

void MainForm::slotDone()
{
 disconnect(&networkThread, 0, 0, 0);
 disconnect(&nstreamThread, 0, 0, 0);
 QMessageBox::information(this, trUtf8("Information"), trUtf8("Transfer successful"));
 defaultProgressBar(FALSE, 100, 0, 0);
 ui.statusLabel->setText("Disconnected");
 networkThread.quit();
 networkThread.wait();
 setReadyMode();
}

void MainForm::slotFail()
{
 disconnect(&networkThread, 0, 0, 0);
 disconnect(&nstreamThread, 0, 0, 0);
 QMessageBox::critical(this, trUtf8("Critical"), trUtf8("Transfer failed"));
 nstreamThread.cancel();
 nstreamThread.wait();
 networkThread.quit();
 networkThread.wait();
 ui.statusLabel->setText("Disconnected");
 defaultProgressBar(FALSE, 100, 0, 0);
 setReadyMode();
}

void MainForm::onConnected(QTcpSocket *socket)
{
 disconnect(&nstreamThread, 0, 0, 0);
 nstreamThread.setSock(socket);
 nstreamThread.setFile(fileName);
 connect(&nstreamThread, SIGNAL(progressSetup(bool, int, int, int)), this, SLOT(progressSetup(bool, int, int, int)));
 connect(&nstreamThread, SIGNAL(progressValue(int)), this, SLOT(progressValue(int)));
 connect(&nstreamThread, SIGNAL(statusMessage(QString)), this, SLOT(statusMessage(QString)));
 connect(&nstreamThread, SIGNAL(done()), this, SLOT(slotDone()));
 connect(&nstreamThread, SIGNAL(fail()), this, SLOT(slotFail()));
 nstreamThread.start();
}

void MainForm::onState(QAbstractSocket::SocketState value)
{
 switch(value) {
  case QAbstractSocket::UnconnectedState: ui.statusLabel->setText("Disconnected"); break;
  case QAbstractSocket::HostLookupState: ui.statusLabel->setText("Resolving hostname..."); break;
  case QAbstractSocket::ConnectingState: ui.statusLabel->setText("Connecting..."); break;
  case QAbstractSocket::ConnectedState: ui.statusLabel->setText("Connected"); break;
  case QAbstractSocket::ClosingState: ui.statusLabel->setText("Waiting for close connection..."); break;
 }
}


void MainForm::onError(QString error)
{
 disconnect(&networkThread, 0, 0, 0);
 disconnect(&nstreamThread, 0, 0, 0);
 QMessageBox::critical(this, trUtf8("Critical"), error);
 nstreamThread.cancel();
 nstreamThread.wait();
 networkThread.quit();
 networkThread.wait();
 ui.statusLabel->setText("Disconnected");
 defaultProgressBar(FALSE, 100, 0, 0);
 setReadyMode();
}
