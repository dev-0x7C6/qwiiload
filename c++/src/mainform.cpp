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
#include <QCoreApplication>

class QFileInfo;

MainForm::MainForm(QWidget * parent, Qt::WFlags f):QMainWindow(parent, f)
{
 QTextCodec::setCodecForTr (QTextCodec::codecForName ("UTF-8"));
 ui.setupUi(this);
 if (QCoreApplication::arguments().count() > 1)
  ui.localFile->setText(QCoreApplication::arguments().at(1));
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
  QString hostName = ui.wiiHostName->text();
  if (hostName == QString("")) {
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

  disconnect(&thread, 0, 0, 0);
  connect(&thread, SIGNAL(pbSetEnabled(bool)), this, SLOT(pbSetEnabled(bool)));
  connect(&thread, SIGNAL(pbSetValue(quint64)), this, SLOT(pbSetValue(quint64)));
  connect(&thread, SIGNAL(pbSetRange(quint64,quint64)), this, SLOT(pbSetRange(quint64,quint64)));
  connect(&thread, SIGNAL(sendMessage(QString)), this, SLOT(sendMessage(QString)));

  switch(ui.channelSelect->currentIndex()) {
   case 0: thread.setPort(4299); break;
   case 1: thread.setPort(8080); break;
  }

  thread.setHost(hostName);
  thread.setFile(fileName);
  thread.start();

 } else {
  disconnect(&thread, 0, 0, 0);
  ui.progressBar->setMaximum(100);
  ui.progressBar->setMinimum(0);
  ui.progressBar->setValue(0);
  ui.progressBar->setEnabled(FALSE);
 }
}

void MainForm::pbSetEnabled(bool opt){ ui.progressBar->setEnabled(opt); }
void MainForm::pbSetValue(quint64 value){ ui.progressBar->setValue(value); }
void MainForm::pbSetRange(quint64 min, quint64 max){
 ui.progressBar->setMaximum(max);
 ui.progressBar->setMinimum(min);
}

void MainForm::sendMessage(QString msg){
 QMessageBox::critical(this, trUtf8("Critical"), msg);
};
