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

#include "RegistersItem.h"

#include "ui/QSimKit.h"
#include "Peripherals/MSP430/MSP430.h"
#include "Peripherals/Peripheral.h"
#include "CPU/Memory/RegisterSet.h"
#include "CPU/Memory/Register.h"

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

RegistersItem::RegistersItem(MSP430 *cpu) : QTreeWidgetItem(QTreeWidgetItem::UserType) {
	m_cpu = cpu;
	setText(0, "Registers");
	setFirstColumnSpanned(true);
	setExpanded(true);

	for (int r = 0; r < 16; r++) {
		QTreeWidgetItem *item = new QTreeWidgetItem(this);
		if (r == 0)
			item->setText(0, QString("PC"));
		else if (r == 1)
			item->setText(0, QString("SP"));
		else if (r == 2)
			item->setText(0, QString("SR"));
		else
			item->setText(0, QString("R") + QString::number(r));
		item->setText(1, "0x00");
		item->setBackground(0, QApplication::palette().window());
	}
}

RegistersItem::~RegistersItem() {
	
}

void RegistersItem::refresh() {
	for (int r = 0; r < 16; r++) {
		int16_t n = m_cpu->getRegisterSet()->get(r)->getBigEndian();
		QString dec;
		QString hex;
		QString bin;
		if (r == 0) {
			dec = QString::number((uint16_t) n);
			hex = QString("0x%1").arg((uint16_t) n, 0, 16);
			bin = QString("%1").arg((uint16_t) n, 0, 2);
		}
		else {
			dec = QString::number(n);
			hex = QString("0x%1").arg((uint16_t) n, 0, 16);
			bin = QString("%1").arg((uint16_t) n, 0, 2);
		}

		QTreeWidgetItem *it = child(r);
		it->setText(1, hex);

		QString tooltip = "Dec: " + dec + "<br/>";
		tooltip += "Hex: " + hex + "<br/>";
		tooltip += "Bin: " + bin;
		it->setToolTip(1, tooltip);
	}
}
