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

#include "BasicClockItem.h"

#include "ui/QSimKit.h"
#include "MCU/MSP430/MSP430.h"
#include "MCU/RegisterSet.h"
#include "MCU/Register.h"
#include "MCU/Memory.h"
#include "Peripherals/Peripheral.h"
#include "DockWidgets/Peripherals/MemoryItem.h"
#include "CPU/Variants/Variant.h"
#include "CPU/BasicClock/BasicClock.h"
#include "CPU/BasicClock/ACLK.h"
#include "CPU/BasicClock/MCLK.h"
#include "CPU/BasicClock/SMCLK.h"

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

BasicClockItem::BasicClockItem(MCU_MSP430 *cpu) : QTreeWidgetItem(QTreeWidgetItem::UserType) {
	m_cpu = cpu;
	setText(0, "Basic Clock");
	setFirstColumnSpanned(true);
	setExpanded(true);

	QTreeWidgetItem *item;

#define ADD_ITEM(NAME) item = new QTreeWidgetItem(this); \
	item->setText(0, NAME); \
	item->setBackground(0, QApplication::palette().window());

	ADD_ITEM("ACLK");
	ADD_ITEM("ACLK");
	ADD_ITEM("MCLK");
	ADD_ITEM("MCLK");
	ADD_ITEM("SMCLK");
	ADD_ITEM("SMCLK");
}

BasicClockItem::~BasicClockItem() {
	
}

void BasicClockItem::refresh() {
	MSP430::BasicClock *bc = m_cpu->getBasicClock();
	QTreeWidgetItem *item;

	item = static_cast<QTreeWidgetItem *>(child(0));
	item->setText(1, QString::fromStdString(bc->getACLK()->getSourceName()));

	item = static_cast<QTreeWidgetItem *>(child(1));
	item->setText(1, QString("%1 Hz").arg(bc->getACLK()->getFrequency()));

	item = static_cast<QTreeWidgetItem *>(child(2));
	item->setText(1, QString::fromStdString(bc->getMCLK()->getSourceName()));

	item = static_cast<QTreeWidgetItem *>(child(3));
	item->setText(1, QString("%1 Hz").arg(bc->getMCLK()->getFrequency()));

	item = static_cast<QTreeWidgetItem *>(child(4));
	item->setText(1, QString::fromStdString(bc->getSMCLK()->getSourceName()));

	item = static_cast<QTreeWidgetItem *>(child(5));
	item->setText(1, QString("%1 Hz").arg(bc->getSMCLK()->getFrequency()));
	
}
