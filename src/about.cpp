#include "about.h"
#include "ui_about.h"

AboutForm::AboutForm(QWidget *parent)
		: QDialog(parent)
		, m_ui(std::make_unique<Ui::AboutFormClass>()) {
	m_ui->setupUi(this);
	connect(m_ui->pushButton, &QPushButton::clicked, this, &AboutForm::close);
}

AboutForm::~AboutForm() = default;
