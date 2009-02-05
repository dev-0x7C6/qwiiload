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

#include "mainform.h"

#include <QFileInfo>
#include <QCoreApplication>

class QFileInfo;

const
 QString messageConfirmConnection = "Information: Confirm connection on your Wii first";
 QString messageFileNotFound = "Critical: File not found";
 QString messageHostnameEmpty = "Warning: Hostname field is empty";
 QString messageTransferInterrupted = "Critical: Transfer Interrupted";

const 
 QString readyButtonCancel = "cancel";
 QString readyButtonReady = "ready";

const
 QString msgBoxCritical = "Critical"; 
 QString msgBoxInformation = "Information";
 QString msgBoxWarning = "Warning";

const
 QString statusDisconnected = "Disconnected";
 QString statusHostLockup = "Resolving hostname...";
 QString statusConnecting = "Connecting...";
 QString statusConnected = "Connected";
 QString statusClosing = "Waiting for close connection...";



MainForm::MainForm(QWidget * parent, Qt::WFlags f):QMainWindow(parent, f)
{
 ui.setupUi(this);
 if (QCoreApplication::arguments().count() > 1) ui.localFile->setText(QCoreApplication::arguments().at(1));
 setMaximumHeight(height());
 setMinimumHeight(height());
 setWindowTitle(mainWindowTitle);
 FileDialog = new QFileDialog(this);
 connect(ui.readyBtn, SIGNAL(clicked()), this, SLOT(slotReadyBtnClicked()));
 connect(ui.openFile, SIGNAL(clicked()), this, SLOT(slotOpenFileClicked()));
// MainMenu
 connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(slotOpenFileClicked()));
 connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(slotActionExit()));
 connect(ui.actionAboutProgram, SIGNAL(triggered()), this, SLOT(slotAboutProgram()));
}

MainForm::~MainForm(){
 disconnect(&networkThread, 0, 0, 0);
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

void MainForm::defaultProgressBar(bool enabled, int max, int min, int value){
 ui.progressBar->setMaximum(max);
 ui.progressBar->setMinimum(min);
 ui.progressBar->setEnabled(enabled);
 ui.progressBar->setValue(value);
}

void MainForm::setReadyMode(){
 ui.readyBtn->setIcon(QIcon(QString::fromUtf8(":/actions/icons/actions/button_ok.png")));
 ui.readyBtn->setText(readyButtonReady);
}

void MainForm::setCancelMode(){
 ui.readyBtn->setIcon(QIcon(QString::fromUtf8(":/actions/icons/actions/button_cancel.png")));
 ui.readyBtn->setText(readyButtonCancel);
}

QString fileName;

void MainForm::slotReadyBtnClicked()
{
 if (ui.readyBtn->text() == QString(readyButtonReady)) {
  QString Hostname = ui.wiiHostName->text();
  if (Hostname == QString("")) {
   QMessageBox::warning(this, msgBoxWarning, messageHostnameEmpty);
   return;
  }

  fileName = ui.localFile->text();
  QFileInfo *FileInfo;
  FileInfo = new QFileInfo(fileName);
  bool fileExists = FileInfo->exists() && !FileInfo->isDir();
  delete FileInfo;

  if (fileExists == FALSE) {
   QMessageBox::critical(this, msgBoxCritical, messageFileNotFound);
   return;
  }

  setCancelMode();
  defaultProgressBar(FALSE, 100, 0, 0);

  switch(ui.channelSelect->currentIndex()) {
   case 0: networkThread.setDestPort(4299); break;
   case 1: networkThread.setDestPort(8080); break;
  }
  networkThread.setHostname(Hostname);
  networkThread.setFilename(fileName);

  disconnect(&networkThread, 0, 0, 0);
// Informations
  connect(&networkThread, SIGNAL(updateState(QAbstractSocket::SocketState)), this, SLOT(onState(QAbstractSocket::SocketState)));
// Progress Bar
  connect(&networkThread, SIGNAL(pbSetEnabledSig(bool)), this, SLOT(pbSetEnabled(bool)));
  connect(&networkThread, SIGNAL(pbSetValueSig(quint64)), this, SLOT(pbSetValue(quint64)));
  connect(&networkThread, SIGNAL(pbSetRangeSig(quint64,quint64)), this, SLOT(pbSetRange(quint64,quint64)));
  connect(&networkThread, SIGNAL(endWork(bool, QString)), this, SLOT(slotDone(bool, QString)));
  connect(&networkThread, SIGNAL(waitForAccepted()), this, SLOT(slotWaitForAccepted()));
//

  networkThread.start();

 } else {
  disconnect(&networkThread, 0, 0, 0);
  networkThread.cancelTransfer();
  networkThread.quit();
  networkThread.wait();
  ui.statusLabel->setText(statusDisconnected);
  defaultProgressBar(FALSE, 100, 0, 0);
  setReadyMode();
  QMessageBox::warning(this, msgBoxWarning, messageTransferInterrupted);
 }
}

void MainForm::slotDone(bool result, QString msg)
{
 disconnect(&networkThread, 0, 0, 0);
 if (result == TRUE) 
  QMessageBox::information(this, msgBoxInformation, msg); else
  QMessageBox::critical(this, msgBoxCritical, msg);
 networkThread.quit();
 networkThread.wait();
 ui.statusLabel->setText(statusDisconnected);
 defaultProgressBar(FALSE, 100, 0, 0);
 setReadyMode();
}

void MainForm::pbSetEnabled(bool opt){ ui.progressBar->setEnabled(opt); }
void MainForm::pbSetValue(quint64 value){ ui.progressBar->setValue(value); }
void MainForm::pbSetRange(quint64 min, quint64 max){
 ui.progressBar->setMaximum(max);
 ui.progressBar->setMinimum(min);
}

void MainForm::onState(QAbstractSocket::SocketState value)
{
 switch(value) {
  case QAbstractSocket::UnconnectedState: ui.statusLabel->setText(statusDisconnected); break;
  case QAbstractSocket::HostLookupState: ui.statusLabel->setText(statusHostLockup); break;
  case QAbstractSocket::ConnectingState: ui.statusLabel->setText(statusConnecting); break;
  case QAbstractSocket::ConnectedState: ui.statusLabel->setText(statusConnected); break;
  case QAbstractSocket::ClosingState: ui.statusLabel->setText(statusClosing); break;
 }
}

void MainForm::slotWaitForAccepted()
{
 QMessageBox::information(this, msgBoxInformation, messageConfirmConnection);
 networkThread.acceptTransfer();
}
