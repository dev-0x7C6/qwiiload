/***************************************************************************
 *   Copyright (C) 2008 by Bartlomiej Burdukiewicz                         *
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

#include <QTextCodec>

AboutForm::AboutForm(QWidget * parent, Qt::WFlags f):QDialog(parent, f)
{
 QTextCodec::setCodecForTr (QTextCodec::codecForName ("UTF-8")); 
 ui.setupUi(this);
 connect(ui.confirmBtn, SIGNAL(clicked()), this, SLOT(slotConfirmButton()));
 //connect(this, SIGNAL(show()), this, SLOT(onShow()));
}

void AboutForm::show()
{
 setMaximumHeight(height());
 setMinimumHeight(height());
 setMaximumWidth(width());
 setMinimumWidth(width());
}

void AboutForm::slotConfirmButton()
{
 close();
}
