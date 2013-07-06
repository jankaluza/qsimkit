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
#include "MemoryItem.h"

#include "ui/QSimKit.h"
#include "MCU/MCU.h"
#include "Peripherals/Peripheral.h"
#include "Breakpoints/BreakpointManager.h"
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
#include <QInputDialog>

Peripherals::Peripherals(QSimKit *simkit) :
DockWidget(simkit), m_mcu(0), m_simkit(simkit) {
	setupUi(this);

	connect(view, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(handleContextMenu(const QPoint &)) );
}

void Peripherals::addBreakpoint() {
	bool ok = false;
	int val = QInputDialog::getInt(this, "Add memory breakpoint", "Memory value:", 0, 0, 65535, 1, &ok);
	if (!ok) {
		return;
	}

	QTreeWidgetItem *item = view->currentItem();

	item->setBackground(0, QBrush(Qt::red));
	m_breakpoints.append(item);

	BreakpointManager *m = m_simkit->getBreakpointManager();
	m->addMemoryBreak(item->data(0, Qt::UserRole).toInt(), val);
}

void Peripherals::removeBreakpoint() {
	QTreeWidgetItem *item = view->currentItem();

	item->setBackground(0, view->palette().window());
	m_breakpoints.removeAll(item);

	BreakpointManager *m = m_simkit->getBreakpointManager();
	m->removeMemoryBreak(item->data(0, Qt::UserRole).toInt());
}

void Peripherals::handleContextMenu(const QPoint &pos) {
	QList<QAction *> actions;

	QTreeWidgetItem *item = view->currentItem();
	if (item->type() != MemoryItemType) {
		return;
	}

	QAction *add = 0;
	QAction *remove = 0;
	if (!m_breakpoints.contains(item)) {
		add = new QAction("Add breakpoint", 0);
		actions.append(add);
	}
	else {
		remove = new QAction("Remove breakpoint", 0);
		actions.append(remove);
	}

	QAction *action = QMenu::exec(actions, view->mapToGlobal(pos), 0, 0);
	if (!action) {
		return;
	}

	if (action == add) {
		addBreakpoint();
	}
	else if (action == remove) {
		removeBreakpoint();
	}
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