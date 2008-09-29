#include "manager.h"

ManagerForm::ManagerForm(QWidget * parent, Qt::WFlags f):QDialog(parent, f)
{
 ui.setupUi(this);
 QTextCodec::setCodecForTr (QTextCodec::codecForName ("UTF-8")); 
 connect(ui.selectButton, SIGNAL(Clicked()), this, SLOT(slotSelectButtonClicked()));
 connect(ui.aConnectionButton, SIGNAL(Clicked()), this, SLOT(slotAConnectionClicked()));
 connect(ui.dConnectionButton, SIGNAL(Clicked()), this, SLOT(slotDConnectionClicked()));
}

ManagerForm::~ManagerForm()
{
}

void ManagerForm::slotSelectButtonClicked()
{
}

void ManagerForm::slotAConnectionClicked()
{
}

void ManagerForm::slotDConnectionClicked()
{
}
