#include "mainform.h"

#include <QMessageBox>
#include <QTcpSocket>
#include <QFile>
#include <QDataStream>
#include <QFileDialog>

QString host, filename = "";
int port = 0;

MainForm::MainForm(QWidget * parent, Qt::WFlags f):QDialog(parent, f)
{
 ui.setupUi(this);
 QTextCodec::setCodecForTr (QTextCodec::codecForName ("UTF-8"));
 WiiSock = new QTcpSocket(this);
 FileDialog = new QFileDialog(this);
 connect(WiiSock, SIGNAL(connected()), this, SLOT(slotConnected()));
 connect(WiiSock, SIGNAL(connectionClosed()), this, SLOT(slotDisconnected()));
 connect(WiiSock, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
 connect(ui.readyButton, SIGNAL(clicked()), this, SLOT(slotReadyBtnClicked()));
 connect(ui.openFile, SIGNAL(clicked()), this, SLOT(slotOpenFileClicked()));
 ui.wiiHostName->setText("192.168.1.2");
 FileDialog->setFilter( "*.*" );
}

void MainForm::slotConnected()
{
 QMessageBox::information(this, "Information", tr("Connected with ") + host);
 unsigned char datagram[4];
 if (port == 4299)
 {
 	char header[4];
 	header[0] = 'H';
 	header[1] = 'A';
 	header[2] = 'X';
 	header[3] = 'X';
 	WiiSock->write((const char *)&header, sizeof(header));
 
 	datagram[0] = 0;
 	datagram[1] = 3;
 	datagram[2] = (0 >> 8) && 0xFF;
 	datagram[3] = 0 && 0xFF;
 	WiiSock->write((const char *)&datagram, sizeof(datagram));
 };

 
 QFile file(filename);
 if (!file.open(QIODevice::ReadOnly))
  return;

 int FileSize = file.size();
 datagram[0] = FileSize >> 24;
 datagram[1] = FileSize >> 16;
 datagram[2] = FileSize >> 8;
 datagram[3] = FileSize;
 WiiSock->write((const char *)&datagram, sizeof(datagram));

 char buffer[4096];
 int readed;
 QDataStream readfile(&file);

 while (!readfile.atEnd()) {
  readed = readfile.readRawData(buffer, sizeof(buffer));
  WiiSock->write((const char *)&buffer, readed);
 }
}


void MainForm::slotOpenFileClicked()
{
 filename = FileDialog->getOpenFileName();
 ui.localFile->setText(filename); 
}

void MainForm::slotDisconnected()
{
}

void MainForm::slotReadyRead()
{
}

void MainForm::slotReadyBtnClicked()
{
 host = ui.wiiHostName->text();
 if (ui.channelSelect->currentIndex() == 0) port = 4299;
 if (ui.channelSelect->currentIndex() == 1) port = 8080;
 WiiSock->connectToHost(host, port);
};
