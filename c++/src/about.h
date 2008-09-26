#include "ui_about.h"

#include <QDialog>
#include <QTextCodec>

using namespace Ui;

class AboutForm: public QDialog
{
Q_OBJECT
 public:
   aboutform ui;
   AboutForm(QWidget * parent = 0, Qt::WFlags f = 0 );
   ~AboutForm(){};
};
