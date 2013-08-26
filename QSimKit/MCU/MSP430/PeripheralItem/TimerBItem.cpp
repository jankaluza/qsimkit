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

#include "TimerBItem.h"

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

TimerBItem::TimerBItem(MCU_MSP430 *cpu) : QTreeWidgetItem(QTreeWidgetItem::UserType) {
	m_cpu = cpu;
	setText(0, "Timer B");
	setFirstColumnSpanned(true);
	setExpanded(true);

	QTreeWidgetItem *item;
	Variant *v = m_cpu->getVariantPtr();

#define ADD_ITEM(METHOD, NAME) if (METHOD != 0) { \
	item = new MemoryItem(this, NAME, METHOD, t); \
}

	VariableType t("uint16_t", 1, VariableType::Unsigned, VariableType::Base);
	ADD_ITEM(v->getTBCTL(), "TBCTL");
	ADD_ITEM(v->getTBR(), "TBR");
	ADD_ITEM(v->getTBCCTL0(), "TBCCTL0");
	ADD_ITEM(v->getTBCCR0(), "TBCCR0");
	ADD_ITEM(v->getTBCCTL1(), "TBCCTL1");
	ADD_ITEM(v->getTBCCR1(), "TBCCR1");
	ADD_ITEM(v->getTBCCTL2(), "TBCCTL2");
	ADD_ITEM(v->getTBCCR2(), "TBCCR2");
	ADD_ITEM(v->getTBCCTL3(), "TBCCTL3");
	ADD_ITEM(v->getTBCCR3(), "TBCCR3");
	ADD_ITEM(v->getTBCCTL4(), "TBCCTL4");
	ADD_ITEM(v->getTBCCR4(), "TBCCR4");
	ADD_ITEM(v->getTBCCTL5(), "TBCCTL5");
	ADD_ITEM(v->getTBCCR5(), "TBCCR5");
	ADD_ITEM(v->getTBCCTL6(), "TBCCTL6");
	ADD_ITEM(v->getTBCCR6(), "TBCCR6");
	ADD_ITEM(v->getTBIV(), "TBIV");
}

TimerBItem::~TimerBItem() {
	
}

void TimerBItem::refresh() {
	for (int i = 0; i < childCount(); ++i) {
		MemoryItem *item = static_cast<MemoryItem *>(child(i));
		item->refresh(m_cpu->getMemory());
	}
}
