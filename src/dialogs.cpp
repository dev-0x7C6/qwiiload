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

#include "dialogs.hpp"

#include <QFileDialog>
#include <QMessageBox>

namespace dialogs {
namespace strings {
constexpr auto CRITICAL_TITLE = "Critical";
constexpr auto INFORMATION_TITLE = "Information";

constexpr auto FILE_NOT_FOUND = "%1: File not found !";
constexpr auto CANT_OPEN_TO_READ = "%1: Cannot open file to read !";
constexpr auto FILE_IS_EMPTY = "%1: File is empty !";
constexpr auto UPLOAD_SUCCESSFUL = "Data written successful";
} // namespace strings
} // namespace dialogs

using namespace dialogs::strings;

QString dialogs::ask::open_file(QWidget *parent) noexcept {
	QFileDialog fileOpenDialog(parent);
	return fileOpenDialog.getOpenFileName();
}

void dialogs::critical::file_not_found(QWidget *parent, const QString &path) noexcept {
	QMessageBox::critical(parent, CRITICAL_TITLE, QString(FILE_NOT_FOUND).arg(path));
}

void dialogs::critical::file_cant_read(QWidget *parent, const QString &path) noexcept {
	QMessageBox::critical(parent, CRITICAL_TITLE, QString(CANT_OPEN_TO_READ).arg(path));
}

void dialogs::critical::file_empty(QWidget *parent, const QString &path) noexcept {
	QMessageBox::critical(parent, CRITICAL_TITLE, QString(FILE_IS_EMPTY).arg(path));
}

void dialogs::information::data_written_successful(QWidget *parent) noexcept {
	QMessageBox::information(parent, INFORMATION_TITLE, UPLOAD_SUCCESSFUL);
}

void dialogs::critical::transfer_failed(QWidget *parent, const QString &reason) noexcept {
	QMessageBox::critical(parent, CRITICAL_TITLE, reason);
}
