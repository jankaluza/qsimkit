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

#include "TimerAItem.h"

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

TimerAItem::TimerAItem(MCU_MSP430 *cpu) : QTreeWidgetItem(QTreeWidgetItem::UserType) {
	m_cpu = cpu;
	setText(0, "Timer A");
	setFirstColumnSpanned(true);
	setExpanded(true);

	QTreeWidgetItem *item;
	Variant *v = m_cpu->getVariantPtr();

#define ADD_ITEM(METHOD, NAME) if (METHOD != 0) { \
	item = new MemoryItem(this, NAME, METHOD, t); \
}

	VariableType t("uint16_t", 1, VariableType::Unsigned, VariableType::Base);
	ADD_ITEM(v->getTA0CTL(), "TA0CTL");
	ADD_ITEM(v->getTA0R(), "TA0R");
	ADD_ITEM(v->getTA0CCTL0(), "TA0CCTL0");
	ADD_ITEM(v->getTA0CCR0(), "TA0CCR0");
	ADD_ITEM(v->getTA0CCTL1(), "TA0CCTL1");
	ADD_ITEM(v->getTA0CCR1(), "TA0CCR1");
	ADD_ITEM(v->getTA0CCTL2(), "TA0CCTL2");
	ADD_ITEM(v->getTA0CCR2(), "TA0CCR2");
	ADD_ITEM(v->getTA0CCTL3(), "TA0CCTL3");
	ADD_ITEM(v->getTA0CCR3(), "TA0CCR3");
	ADD_ITEM(v->getTA0CCTL4(), "TA0CCTL4");
	ADD_ITEM(v->getTA0CCR4(), "TA0CCR4");
	ADD_ITEM(v->getTA0IV(), "TA0IV");

	ADD_ITEM(v->getTA1CTL(), "TA1CTL");
	ADD_ITEM(v->getTA1R(), "TA1R");
	ADD_ITEM(v->getTA1CCTL0(), "TA1CCTL0");
	ADD_ITEM(v->getTA1CCR0(), "TA1CCR0");
	ADD_ITEM(v->getTA1CCTL1(), "TA1CCTL1");
	ADD_ITEM(v->getTA1CCR1(), "TA1CCR1");
	ADD_ITEM(v->getTA1CCTL2(), "TA1CCTL2");
	ADD_ITEM(v->getTA1CCR2(), "TA1CCR2");
	ADD_ITEM(v->getTA1CCTL3(), "TA1CCTL3");
	ADD_ITEM(v->getTA1CCR3(), "TA1CCR3");
	ADD_ITEM(v->getTA1CCTL4(), "TA1CCTL4");
	ADD_ITEM(v->getTA1CCR4(), "TA1CCR4");
	ADD_ITEM(v->getTA1IV(), "TA1IV");
}

TimerAItem::~TimerAItem() {
	
}

void TimerAItem::refresh() {
	for (int i = 0; i < childCount(); ++i) {
		MemoryItem *item = static_cast<MemoryItem *>(child(i));
		item->refresh(m_cpu->getMemory());
	}
}
