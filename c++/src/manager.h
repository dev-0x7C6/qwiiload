#include "ui_wiimanager.h"

#include <QDialog>
#include <QTextCodec>

using namespace Ui;

class ManagerForm: public QDialog
{
Q_OBJECT
 public:
   managerForm ui;
   ManagerForm(QWidget * parent = 0, Qt::WFlags f = 0);
   ~ManagerForm();
 public slots:
   void slotSelectButtonClicked();
   void slotAConnectionClicked();
   void slotDConnectionClicked();
};
