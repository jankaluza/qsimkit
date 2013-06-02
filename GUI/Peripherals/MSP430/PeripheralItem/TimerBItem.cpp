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
#include "Peripherals/MSP430/MSP430.h"
#include "Peripherals/Peripheral.h"
#include "CPU/Memory/RegisterSet.h"
#include "CPU/Memory/Register.h"
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

TimerBItem::TimerBItem(MSP430 *cpu) : QTreeWidgetItem(QTreeWidgetItem::UserType) {
	m_cpu = cpu;
	setText(0, "Timer B");
	setFirstColumnSpanned(true);
	setExpanded(true);

	QTreeWidgetItem *item;
	Variant *v = m_cpu->getVariant();

#define ADD_ITEM(METHOD, NAME) if (METHOD != 0) { \
	item = new QTreeWidgetItem(this); \
	item->setData(0, Qt::UserRole, METHOD); \
	item->setText(0, NAME); \
	item->setText(1, 0x00); \
	item->setBackground(0, QApplication::palette().window()); \
}

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
		QTreeWidgetItem *item = child(i);
		int address = item->data(0, Qt::UserRole).toInt();
		if (!address) {
			continue;
		}

		int16_t n = m_cpu->getMemory()->getBigEndian(address);
		QString dec = QString::number(n);
		QString hex = QString("0x%1").arg((uint16_t) n, 0, 16);
		QString bin = QString("%1").arg((uint16_t) n, 0, 2);

		item->setText(1, hex);

		QString tooltip = "Dec: " + dec + "<br/>";
		tooltip += "Hex: " + hex + "<br/>";
		tooltip += "Bin: " + bin;
		item->setToolTip(1, tooltip);
	
	}
}
