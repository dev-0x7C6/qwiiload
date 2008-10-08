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

void MainForm::setReadyMode(){
 ui.readyBtn->setIcon(QIcon(QString::fromUtf8(":/actions/icons/actions/button_ok.png")));
 ui.readyBtn->setText("ready");
}

void MainForm::setCancelMode(){
 ui.readyBtn->setIcon(QIcon(QString::fromUtf8(":/actions/icons/actions/button_cancel.png")));
 ui.readyBtn->setText("cancel");
}

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

  setCancelMode();

  ConnectionThread = new QConnectionThread(this, streamThread);
  ConnectionThread->setHost(Hostname);
  ConnectionThread->setFile(fileName);

  switch(ui.channelSelect->currentIndex()) {
   case 0: ConnectionThread->setPort(4299); break;
   case 1: ConnectionThread->setPort(8080); break;
  }
  connect(ConnectionThread, SIGNAL(onChangeStatus(QString)), this, SLOT(onChangeStatus(QString)));
  connect(ConnectionThread, SIGNAL(setProgressBarState(bool, int, int, int)), this, SLOT(setProgressBarState(bool, int, int, int)));
  connect(ConnectionThread, SIGNAL(setProgressBarValue(int)), this, SLOT(setProgressBarValue(int)));
  connect(ConnectionThread, SIGNAL(transferDone()), this, SLOT(transferDone())); 
  connect(ConnectionThread, SIGNAL(transferFail(QString)), this, SLOT(transferFail(QString))); 

  ConnectionThread->start();

 } else {
  defaultProgressBar(FALSE, 100, 0, 0);
  setReadyMode();
  ui.statusLabel->setText("Disconnected");

  ConnectionThread->disconnectAnyway();
  ConnectionThread->quit();
 }
}



void MainForm::transferDone(){
 defaultProgressBar(FALSE, 100, 0, 0); 
 setReadyMode();
 QMessageBox::information(this, trUtf8("Done"),trUtf8("File was successful transfered"));
}

void MainForm::transferFail(QString error)
{
 defaultProgressBar(FALSE, 100, 0, 0);
 QMessageBox::critical(this, trUtf8("Critical"), error);
 ui.statusLabel->setText("Disconnected");
 setReadyMode();
}
