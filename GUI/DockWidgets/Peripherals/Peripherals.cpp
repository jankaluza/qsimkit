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
#include "Peripherals/MSP430/MSP430.h"
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

Peripherals::Peripherals(QSimKit *simkit) :
DockWidget(simkit), m_cpu(0), m_simkit(simkit) {
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
}

void Peripherals::removePeripheralItem(PeripheralItem *item) {
	for (int i = 0; i < view->topLevelItemCount(); ++i) {
		PeripheralItem *item = dynamic_cast<PeripheralItem *>(view->topLevelItem(i));
		if (item == item) {
			view->takeTopLevelItem(i);
			return;
		}
	}
}

void Peripherals::setCPU(MSP430 *cpu) {
	m_cpu = cpu;

	
}
