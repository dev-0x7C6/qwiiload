#include "ui_progressform.h"
#include <QDialog>
#include <QTextCodec>

using namespace Ui;

class ProgressForm: public QDialog
{
Q_OBJECT
 public:
  progressform ui;
  ProgressForm(QWidget * parent = 0, Qt::WFlags f = 0 );
  ~ProgressForm(){};
 public slots:
};