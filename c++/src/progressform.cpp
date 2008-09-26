#include "progressform.h"

ProgressForm::ProgressForm(QWidget *parent, Qt::WFlags f):QDialog(parent, f)
{
 ui.setupUi(this);
 QTextCodec::setCodecForTr (QTextCodec::codecForName ("UTF-8"));
}
