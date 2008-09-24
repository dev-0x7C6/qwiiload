#include "ui_mainform.h"

#include <QDialog>
#include <QFileDialog>
#include <QTextCodec>
#include <QTcpSocket>

using namespace Ui;

class MainForm: public QDialog
{
Q_OBJECT
 private:
   QTcpSocket *WiiSock;
   QFileDialog *FileDialog;
 public:
   mainform ui;
   MainForm(QWidget * parent = 0, Qt::WFlags f = 0 );
   ~MainForm(){};
 public slots:
   void slotConnected();
   void slotDisconnected();
   void slotReadyRead();
   void slotReadyBtnClicked();
   void slotOpenFileClicked();
};
