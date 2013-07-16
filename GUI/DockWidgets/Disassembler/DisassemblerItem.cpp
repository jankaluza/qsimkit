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

#include "DisassemblerItem.h"
#include "GUI/MCU/MCU.h"
#include "Disassembler.h"

#include "GUI/MCU/RegisterSet.h"
#include "GUI/MCU/Register.h"
#include "GUI/MCU/Memory.h"

#include <QWidget>
#include <QString>
#include <QTreeWidgetItem>
#include <QDebug>

DisassemblerItem::DisassemblerItem(Disassembler *dis) : m_dis(dis) {
	setText(0, "Variables");
}

void DisassemblerItem::refresh() {
	MCU *mcu = m_dis->getMCU();
	Subprogram *s = m_dis->getCurrentSubprogram();

	while(childCount()) {
		delete takeChild(0);
	}

	if (!s) {
		return;
	}

	Variables &vars = s->getVariables();
	foreach(Variable *v, vars) {
		QTreeWidgetItem *it = new QTreeWidgetItem(this);
		it->setText(0, v->getName());
		it->setText(1, v->getValue(mcu->getRegisterSet(), mcu->getMemory(), s, mcu->getRegisterSet()->get(0)->getBigEndian()));
	}
}

