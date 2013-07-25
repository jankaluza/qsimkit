/**
 * QSimKit - MSP430 simulator
 * Copyright (C) 2013 Jan "HanzZ" Kaluza (hanzz.k@gmail.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 **/

#include "LocalItem.h"
#include "QSimKit/DockWidgets/Peripherals/VariableItem.h"

#include "MCU/RegisterSet.h"
#include "MCU/Register.h"
#include "MCU/Memory.h"
#include "MCU/MCU.h"

#include <QString>
#include <QTreeWidgetItem>
#include <QDebug>

LocalItem::LocalItem() : QTreeWidgetItem(QTreeWidgetItem::UserType) {
	setText(0, "Local Variables");
	setFirstColumnSpanned(true);
	setExpanded(true);
}

LocalItem::~LocalItem() {
	
}

void LocalItem::refresh(RegisterSet *r, Memory *m, Subprogram *s, uint16_t pc) {
	while(childCount()) {
		delete takeChild(0);
	}

	if (!s) {
		return;
	}

	Variables &vars = s->getVariables();
	foreach(Variable *v, vars) {
		VariableItem *it = new VariableItem(this, v);
		it->refresh(r, m, s, pc);
	}
}
