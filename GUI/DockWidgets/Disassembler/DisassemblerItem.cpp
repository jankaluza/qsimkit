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

#include "LocalItem.h"

#include "GUI/MCU/RegisterSet.h"
#include "GUI/MCU/Register.h"
#include "GUI/MCU/Memory.h"

#include <QWidget>
#include <QString>
#include <QTreeWidgetItem>
#include <QDebug>

DisassemblerItem::DisassemblerItem(Disassembler *dis) : m_dis(dis) {
	setText(0, "Disassembler");

	m_localItem = new LocalItem();
	addChild(m_localItem);
}

void DisassemblerItem::refresh() {
	MCU *mcu = m_dis->getMCU();
	RegisterSet *r = mcu->getRegisterSet();
	Memory *m = mcu->getMemory();
	uint16_t pc = r->get(0)->getBigEndian();
	Subprogram *s = m_dis->getCurrentSubprogram();

	m_localItem->refresh(r, m, s, pc);
}

