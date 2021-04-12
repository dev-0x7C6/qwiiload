#pragma once

#include <QDialog>
#include <memory>

namespace Ui {
class AboutFormClass;
}

using namespace Ui;

class AboutForm final : public QDialog {
	Q_OBJECT
public:
	AboutForm(QWidget *parent = 0);
	virtual ~AboutForm();

private:
	std::unique_ptr<Ui::AboutFormClass> m_ui;
};
