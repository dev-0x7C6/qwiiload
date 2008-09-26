#include "about.h"

AboutForm::AboutForm(QWidget * parent, Qt::WFlags f):QDialog(parent, f)
{
 QTextCodec::setCodecForTr (QTextCodec::codecForName ("UTF-8")); 
 ui.setupUi(this);
 connect(ui.confirmBtn, SIGNAL(clicked()), this, SLOT(slotConfirmButton()));
}

void AboutForm::slotConfirmButton()
{
 close();
}
