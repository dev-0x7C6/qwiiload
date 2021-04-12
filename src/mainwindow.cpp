/***************************************************************************
 *   Copyright (C) 2008-2021 by Bartlomiej Burdukiewicz                    *
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
#include "uploadthread.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QSettings>
#include <QTimer>

#include <src/dialogs.hpp>

MainWindow::MainWindow(QWidget *parent)
		: QMainWindow(parent)
		, m_ui(std::make_unique<Ui::MainWindowClass>()) {
	m_ui->setupUi(this);
	m_ui->actionQuit->setShortcut(QKeySequence::Close);
	connect(m_ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
	connect(m_ui->actionAbout, &QAction::triggered, this, &MainWindow::actionAbout);
	connect(m_ui->openFile, &QPushButton::clicked, this, &MainWindow::openFile);
	connect(m_ui->streamButton, &QPushButton::clicked, this, &MainWindow::stream);

	loadSettings();

	if (QCoreApplication::arguments().count() > 1)
		m_ui->fileEdit->setText(QCoreApplication::arguments().at(1));

	auto timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &MainWindow::transactionUpdate);
	timer->start(10);
}

MainWindow::~MainWindow() {
	saveSettings();
}

void MainWindow::transactionUpdate() {
	if (!m_stream)
		return;

	const std::int32_t size = m_stream->progress().size;
	const std::int32_t uploaded = m_stream->progress().uploaded;
	const upload_status status = m_stream->progress().status;

	m_ui->progressBar->setEnabled(upload_status::progress == status);
	m_ui->streamButton->setEnabled(upload_status::progress != status);
	m_ui->progressBar->setMaximum(size);
	m_ui->progressBar->setValue(upload_status::progress == status ? uploaded : 0);

	if (upload_status::progress != status)
		m_stream.reset();

	switch (status) {
		case upload_status::progress:
			break;
		case upload_status::connection_timeout:
			return dialogs::critical::transfer_failed(this, "connection timeout");
		case upload_status::error:
			return dialogs::critical::transfer_failed(this, "communication error");
		case upload_status::successful:
			return dialogs::information::data_written_successful(this);
	}
}

void MainWindow::actionAbout() {
	AboutForm about(this);
	about.exec();
}

void MainWindow::openFile() {
	if (auto fileName = dialogs::ask::open_file(this); !fileName.isEmpty())
		m_ui->fileEdit->setText(fileName);
}

void MainWindow::loadSettings() {
	QSettings settings;
	settings.beginGroup("settings");
	m_ui->hostEdit->setText(settings.value("Hostname", "").toString());
	m_ui->fileEdit->setText(settings.value("Filename", "").toString());
	settings.endGroup();
}

void MainWindow::saveSettings() {
	QSettings settings;
	settings.beginGroup("settings");
	settings.setValue("Hostname", m_ui->hostEdit->text());
	settings.setValue("Filename", m_ui->fileEdit->text());
	settings.endGroup();
}

void MainWindow::stream() {
	const auto path = m_ui->fileEdit->text();
	QFile file(path);

	if (!file.exists())
		return dialogs::critical::file_not_found(this, path);

	if (!file.open(QIODevice::ReadOnly))
		return dialogs::critical::file_cant_read(this, path);

	if (file.atEnd())
		return dialogs::critical::file_empty(this, path);

	m_stream = std::make_unique<UploadThread>(m_ui->hostEdit->text(), file.readAll());
	m_stream->start();
	transactionUpdate();
}
