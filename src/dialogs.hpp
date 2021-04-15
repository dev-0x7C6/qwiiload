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

#pragma once

#include <QString>

class QWidget;

namespace dialogs {
namespace ask {
QString open_file(QWidget *parent) noexcept;
}

namespace critical {
void file_not_found(QWidget *parent, const QString &path) noexcept;
void file_cant_read(QWidget *parent, const QString &path) noexcept;
void file_empty(QWidget *parent, const QString &path) noexcept;
void transfer_failed(QWidget *parent, const QString &reason) noexcept;
} // namespace critical

namespace information {
void data_written_successful(QWidget *parent) noexcept;
}
} // namespace dialogs
