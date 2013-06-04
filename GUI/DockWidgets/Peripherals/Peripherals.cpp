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

#include "Peripherals.h"
#include "PeripheralItem.h"

#include "ui/QSimKit.h"
#include "MCU/MCU.h"
#include "Peripherals/Peripheral.h"
#include "MCU/RegisterSet.h"
#include "MCU/Register.h"

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

Peripherals::Peripherals(QSimKit *simkit) :
DockWidget(simkit), m_mcu(0), m_simkit(simkit) {
	setupUi(this);
}

void Peripherals::refresh() {
	for (int i = 0; i < view->topLevelItemCount(); ++i) {
		PeripheralItem *item = dynamic_cast<PeripheralItem *>(view->topLevelItem(i));
		item->refresh();
	}
}

void Peripherals::addPeripheralItem(PeripheralItem *item) {
	view->addTopLevelItem(item);
	item->setExpanded(true);
	for (int i = 0; i < item->childCount(); ++i) {
		item->child(i)->setExpanded(true);
	}

	view->resizeColumnToContents(0);
	view->resizeColumnToContents(1);
	view->resizeColumnToContents(2);
	view->resizeColumnToContents(3);
}

void Peripherals::addPeripheral(QObject *peripheral) {
	Peripheral *p = dynamic_cast<Peripheral *>(peripheral);
	if (!p) {
		return;
	}

	PeripheralItem *item = p->getPeripheralItem();
	if (!item) {
		return;
	}

	addPeripheralItem(item);
}

void Peripherals::removePeripheralItem(PeripheralItem *item) {
	for (int i = 0; i < view->topLevelItemCount(); ++i) {
		PeripheralItem *item = dynamic_cast<PeripheralItem *>(view->topLevelItem(i));
		if (item == item) {
			view->takeTopLevelItem(i);
			break;
		}
	}

	view->resizeColumnToContents(0);
	view->resizeColumnToContents(1);
	view->resizeColumnToContents(2);
	view->resizeColumnToContents(3);
}

void Peripherals::removePeripheral(QObject *peripheral) {
	Peripheral *p = dynamic_cast<Peripheral *>(peripheral);
	if (!p) {
		return;
	}

	PeripheralItem *item = p->getPeripheralItem();
	if (!item) {
		return;
	}

	removePeripheralItem(item);
}

void Peripherals::setMCU(MCU *mcu) {
	m_mcu = mcu;

	
}
