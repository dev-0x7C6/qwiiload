#include "ui_mainform.h"

#include <QDialog>
#include <QThread>
#include <QFileDialog>
#include <QTextCodec>
#include <QTcpSocket>

using namespace Ui;

class QConnectionThread: public QThread
{
Q_OBJECT
 private:
   QString QHost;
   int QPort;
 public:
   void run();
   QConnectionThread(QString Host, int Port);
};

class MainForm: public QMainWindow
{
Q_OBJECT
 private:
   QFileDialog *FileDialog;
   QDialog *Window;
   QConnectionThread *ConnectionThread;
 public:
   mainform ui;
   MainForm(QWidget * parent = 0, Qt::WFlags f = 0 );
   ~MainForm();
 public slots:
   void slotConnected();
   void slotDisconnected();
   void slotReadyRead();
   void slotReadyBtnClicked();
   void slotOpenFileClicked();
};
