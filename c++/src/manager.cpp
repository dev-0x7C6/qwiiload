/***************************************************************************
 *   Copyright (C) 2008 by Bartlomiej Burdukiewicz                         *
 *   strikeu@poczta.linux.pl                                               *
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