/***************************************************************************
 *   Copyright (C) 2008-2009 by Bartlomiej Burdukiewicz                    *
 *   dev.strikeu@gmail.com                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "about.h"
#include "mainwindow.h"
#include "qwiistreamthread.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>

const QString FILE_NOT_FOUND = "%1: File not found !";
const QString CANT_OPEN_TO_READ = "%1: Cannot open file to read !";
const QString FILE_IS_EMPTY = "%1: File is empty !";

MainWindow::MainWindow(QWidget *parent)
		: QMainWindow(parent)
		, m_ui(std::make_unique<Ui::MainWindowClass>()) {
	m_ui->setupUi(this);
	m_ui->actionQuit->setShortcut(QKeySequence::Close);
	connect(m_ui->actionQuit, &QAction::triggered, this, &MainWindow::actionQuit);
	connect(m_ui->actionAbout, &QAction::triggered, this, &MainWindow::actionAbout);
	connect(m_ui->openFile, &QPushButton::clicked, this, &MainWindow::openFile);
	connect(m_ui->streamButton, &QPushButton::clicked, this, &MainWindow::stream);

	loadSettings();

	if (QCoreApplication::arguments().count() > 1)
		m_ui->fileEdit->setText(QCoreApplication::arguments().at(1));
}

MainWindow::~MainWindow() {
	saveSettings();
}

void MainWindow::actionAbout() {
	AboutForm about(this);
	about.exec();
}

void MainWindow::openFile() {
	QFileDialog fileOpenDialog(this);
	QString fileName = fileOpenDialog.getOpenFileName();
	if (fileName.count() != 0)
		m_ui->fileEdit->setText(fileName);
}

void MainWindow::progressBarPosition(int value) {
	m_ui->progressBar->setValue(value);
}

void MainWindow::loadSettings() {
	QSettings settings("homebrew", "qwiiload");
	settings.beginGroup("settings");
	m_ui->hostEdit->setText(settings.value("Hostname", "").toString());
	m_ui->fileEdit->setText(settings.value("Filename", "").toString());
	settings.endGroup();
}

void MainWindow::saveSettings() {
	QSettings settings("homebrew", "qwiiload");
	settings.beginGroup("settings");
	settings.setValue("Hostname", m_ui->hostEdit->text());
	settings.setValue("Filename", m_ui->fileEdit->text());
	settings.endGroup();
}

void MainWindow::transferDone() {
	QMessageBox::information(this, "Information", "Data written successful");
	m_ui->streamButton->setEnabled(true);
	m_ui->progressBar->setEnabled(false);
	m_ui->progressBar->setValue(0);
}

void MainWindow::transferFail(QString errorName) {
	QMessageBox::critical(this, "Critical", errorName);
	m_ui->streamButton->setEnabled(true);
	m_ui->progressBar->setEnabled(false);
	m_ui->progressBar->setValue(0);
}

void MainWindow::stream() {
	if (QFile::exists(m_ui->fileEdit->text()) != true) {
		QMessageBox::critical(this, "Critical", QString(FILE_NOT_FOUND).arg(m_ui->fileEdit->text()));
		return;
	}

	QFile file(m_ui->fileEdit->text());

	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::critical(this, "Critical", QString(CANT_OPEN_TO_READ).arg(m_ui->fileEdit->text()));
		return;
	}

	if (file.size() == 0) {
		QMessageBox::critical(this, "Critical", QString(FILE_IS_EMPTY).arg(m_ui->fileEdit->text()));
		return;
	}

	m_ui->progressBar->setMaximum(file.size());
	m_ui->progressBar->setEnabled(true);

	m_stream = std::make_unique<QWiiStreamThread>(m_ui->hostEdit->text(), file.readAll());
	connect(m_stream.get(), &QWiiStreamThread::transferFail, this, &MainWindow::transferFail);
	connect(m_stream.get(), &QWiiStreamThread::transferDone, this, &MainWindow::transferDone);
	connect(m_stream.get(), &QWiiStreamThread::progressBarPosition, this, &MainWindow::progressBarPosition);
	m_stream->start();
	m_ui->streamButton->setEnabled(false);
}
