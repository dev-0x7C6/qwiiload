#include "mainform.h"
#include "about.h"

QTcpSocket *Network;

MainForm::MainForm(QWidget * parent, Qt::WFlags f):QMainWindow(parent, f)
{
 QTextCodec::setCodecForTr (QTextCodec::codecForName ("UTF-8")); 
 ui.setupUi(this);
 setMaximumHeight(height());
 setMinimumHeight(height());
 ui.statusbar->showMessage("Disconnected");

 Network = new QTcpSocket;

 FileDialog = new QFileDialog(this);

 connect(Network, SIGNAL(connected()), this, SLOT(slotConnected()));
 connect(Network, SIGNAL(connectionClosed()), this, SLOT(slotDisconnected()));
 connect(Network, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
 connect(ui.readyBtn, SIGNAL(clicked()), this, SLOT(slotReadyBtnClicked()));
 connect(ui.openFile, SIGNAL(clicked()), this, SLOT(slotOpenFileClicked()));
// MainMenu
 connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(slotOpenFileClicked()));
 connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(slotActionExit()));
 connect(ui.actionAboutProgram, SIGNAL(triggered()), this, SLOT(slotAboutProgram()));
 connect(ui.actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

}

MainForm::~MainForm()
{
 delete Network;
 delete FileDialog;
}

//QConnectionThread::QConnectionThread(QString Host, int Port):QThread()
//{
// QHost = Host;
// QPort = Port;
//}

//void QConnectionThread::run()
//{
//}

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
 ui.progressBar->setMaximum(FileSize);
 ui.progressBar->setMinimum(0);
 ui.progressBar->setValue(0);
 ui.progressBar->setEnabled(TRUE);
 Network->write((const char *)&datagram, sizeof(datagram));

 char buffer[256];
 int readed;
 QDataStream readfile(&file);

 while (!readfile.atEnd()) {
  readed = readfile.readRawData(buffer, sizeof(buffer));
  Network->write((const char *)&buffer, readed);
  ui.progressBar->setValue(ui.progressBar->value() + readed);
 }
 ui.progressBar->setEnabled(FALSE);
 Network->disconnectFromHost();
}


void MainForm::slotOpenFileClicked()
{
 filename = FileDialog->getOpenFileName();
 ui.localFile->setText(filename); 
}

void MainForm::slotDisconnected()
{
  ui.statusbar->showMessage("Disconnected");
}

void MainForm::slotReadyRead()
{
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

void MainForm::slotReadyBtnClicked()
{
 host = ui.wiiHostName->text();
 if (ui.channelSelect->currentIndex() == 0) port = 4299;
 if (ui.channelSelect->currentIndex() == 1) port = 8080;
 int status = Network->state();
 if (status != QTcpSocket::UnconnectedState)
 {
  Network->disconnectFromHost();
 }
 Network->connectToHost(host, port);
 //ConnectionThread = new QConnectionThread(host, port);
 //ConnectionThread->start();
};
