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

TimerAItem::TimerAItem(MSP430 *cpu) : QTreeWidgetItem(QTreeWidgetItem::UserType) {
	m_cpu = cpu;
	setText(0, "Timer A");
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
