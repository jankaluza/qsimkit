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

#include "USIItem.h"

#include "ui/QSimKit.h"
#include "MCU/MSP430/MSP430.h"
#include "MCU/RegisterSet.h"
#include "MCU/Register.h"
#include "MCU/Memory.h"
#include "Peripherals/Peripheral.h"
#include "DockWidgets/Peripherals/MemoryItem.h"
#include "CPU/Variants/Variant.h"

#include <QWidget>
#include <QTime>
#include <QMainWindow>
#include <QString>
#include <QFileDialog>
#include <QInputDialog>
#include <QFile>
#include <QProcess>
#include <QTreeWidgetItem>
#include <QDebug>

USIItem::USIItem(MCU_MSP430 *cpu) : QTreeWidgetItem(QTreeWidgetItem::UserType) {
	m_cpu = cpu;
	setText(0, "USI");
	setFirstColumnSpanned(true);
	setExpanded(true);

	QTreeWidgetItem *item;
	Variant *v = m_cpu->getVariantPtr();

#define ADD_ITEM(METHOD, NAME) if ((METHOD) > 1) { \
	item = new MemoryItem(this, NAME, (METHOD)); \
}

	ADD_ITEM(v->getUSICTL(), "USICTL0");
	ADD_ITEM(v->getUSICTL() + 1, "USICTL1");
	ADD_ITEM(v->getUSICCTL(), "USIKCTL");
	ADD_ITEM(v->getUSICCTL() + 1, "USICNT");
}

USIItem::~USIItem() {
	
}

void USIItem::refresh() {
	for (int i = 0; i < childCount(); ++i) {
		MemoryItem *item = static_cast<MemoryItem *>(child(i));
		item->refresh(m_cpu->getMemory());
	}
}
