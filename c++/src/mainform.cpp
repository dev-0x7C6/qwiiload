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
 connect(ui.actionManagerRun, SIGNAL(triggered()), this, SLOT(slotActionManagerRun()));
 connect(ui.actionAboutProgram, SIGNAL(triggered()), this, SLOT(slotAboutProgram()));
 connect(ui.actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

MainForm::~MainForm(){
 ConnectionThread->quit();
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

void MainForm::onChangeStatus(QString status){ui.statusLabel->setText(status);}

void MainForm::defaultProgressBar(bool enabled, int max, int min, int value){
 ui.progressBar->setMaximum(max);
 ui.progressBar->setMinimum(min);
 ui.progressBar->setEnabled(enabled);
 ui.progressBar->setValue(value);
}

void MainForm::setProgressBarState(bool enabled, int max, int min, int value){ defaultProgressBar(enabled, max, min, value); }
void MainForm::setProgressBarValue(int value){ ui.progressBar->setValue(value); }

void MainForm::slotReadyBtnClicked()
{
 if (ui.readyBtn->text() == QString("ready")) {
  QString Hostname = ui.wiiHostName->text();
  if (Hostname == QString("")) {
   QMessageBox::warning(this, trUtf8("Warning"), trUtf8("Hostname is empty"));
   return;
  }

  QString fileName;
  fileName = ui.localFile->text();
  QFileInfo *FileInfo;
  FileInfo = new QFileInfo(fileName);
  bool fileExists = FileInfo->exists() && !FileInfo->isDir();
  delete FileInfo;

  if (fileExists == FALSE) {
   QMessageBox::critical(this, trUtf8("Critical"), trUtf8("File not found"));
   return;
  }

  ui.readyBtn->setIcon(QIcon(QString::fromUtf8(":/actions/icons/actions/button_cancel.png")));
  ui.readyBtn->setText("cancel");

  ConnectionThread = new QConnectionThread(this);
  ConnectionThread->setHost(Hostname);
  ConnectionThread->setFile(fileName);

  switch(ui.channelSelect->currentIndex()) {
   case 0: ConnectionThread->setPort(4299); break;
   case 1: ConnectionThread->setPort(8080); break;
  }

  connect(ConnectionThread, SIGNAL(onChangeStatus(QString)), this, SLOT(onChangeStatus(QString)));
  connect(ConnectionThread, SIGNAL(setProgressBarState(bool, int, int, int)), this, SLOT(setProgressBarState(bool, int, int, int)));
  connect(ConnectionThread, SIGNAL(setProgressBarValue(int)), this, SLOT(setProgressBarValue(int)));

  connect(ConnectionThread, SIGNAL(setReadyBtnEnabled()), this, SLOT(setReadyBtnEnabled())); 
  connect(ConnectionThread, SIGNAL(showSocketError(QAbstractSocket::SocketError)), this, SLOT(showSocketError(QAbstractSocket::SocketError))); 
  ConnectionThread->start();
 } else {
  defaultProgressBar(FALSE, 100, 0, 0);
  ui.readyBtn->setIcon(QIcon(QString::fromUtf8(":/actions/icons/actions/button_ok.png")));
  ui.readyBtn->setText("ready");
  ConnectionThread->disconnectAnyway();
  ConnectionThread->quit();
  ui.statusLabel->setText("Disconnected");
 }
}

void MainForm::setReadyBtnEnabled()
{
 defaultProgressBar(FALSE, 100, 0, 0);
 ui.readyBtn->setIcon(QIcon(QString::fromUtf8(":/actions/icons/actions/button_ok.png")));
 ui.readyBtn->setText("ready");
}


void MainForm::showSocketError(QAbstractSocket::SocketError error)
{
 defaultProgressBar(FALSE, 100, 0, 0);
 QString msgError = "Unknown socket error";
 switch (error) {
  case QAbstractSocket::ConnectionRefusedError: msgError = "The connection was refused by the peer or timed out."; break;
  case QAbstractSocket::RemoteHostClosedError: msgError = "The remote host closed the connection."; break;
  case QAbstractSocket::HostNotFoundError: msgError = "The host address was not found."; break;
  case QAbstractSocket::SocketAccessError: msgError = "The socket operation failed because the application lacked the required privileges."; break;
  case QAbstractSocket::SocketResourceError: msgError = "The local system ran out of resources (e.g., too many sockets)."; break;
  case QAbstractSocket::SocketTimeoutError: msgError = "The socket operation timed out."; break;
  case QAbstractSocket::DatagramTooLargeError: msgError = "The datagram was larger than the operating system's limit."; break;
  case QAbstractSocket::NetworkError: msgError = "An error occurred with the network."; break;
  case QAbstractSocket::UnsupportedSocketOperationError: msgError = "The requested socket operation is not supported by the local operating system."; break;
  case QAbstractSocket::ProxyAuthenticationRequiredError: msgError = "The socket is using a proxy, and the proxy requires authentication."; break;
 }
 QMessageBox::critical(this, trUtf8("Critical"), msgError);
}
