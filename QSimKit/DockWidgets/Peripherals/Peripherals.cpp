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
#include "Breakpoints/AddMemoryBreakpoint.h"
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
	connect(m_simkit->getBreakpointManager(), SIGNAL(onMemoryBreakAdded(uint16_t)), this, SLOT(handleMemoryBreakAdded(uint16_t)));
	connect(m_simkit->getBreakpointManager(), SIGNAL(onMemoryBreakRemoved(uint16_t)), this, SLOT(handleMemoryBreakRemoved(uint16_t)));
}

QList<QTreeWidgetItem *> Peripherals::findItemsWithAddress(uint16_t addr) {
	QList<QTreeWidgetItem *> ret;

	QTreeWidgetItemIterator it(view);
	while (*it) {
		if ((*it)->data(0, Qt::UserRole).toInt() == addr) {
			ret.append(*it);
		}
		++it;
	}

	return ret;
}

void Peripherals::handleMemoryBreakAdded(uint16_t address) {
	QString addr = QString::number(address, 16);
	QList<QTreeWidgetItem *> items = findItemsWithAddress(address);
	foreach(QTreeWidgetItem *it, items) {
		it->setBackground(0, QBrush(Qt::red));
	}
}

void Peripherals::handleMemoryBreakRemoved(uint16_t address) {
	QString addr = QString::number(address, 16);
	QList<QTreeWidgetItem *> items = findItemsWithAddress(address);
	foreach(QTreeWidgetItem *it, items) {
		it->setBackground(0, view->palette().window());
	}
}

void Peripherals::addBreakpoint() {
	QTreeWidgetItem *item = view->currentItem();

	AddMemoryBreakpoint dialog(m_simkit->getBreakpointManager(), item->data(0, Qt::UserRole).toString(), this);
	if (dialog.exec() == QDialog::Accepted) {
		item->setBackground(0, QBrush(Qt::red));
	}
}

void Peripherals::removeBreakpoint() {
	QTreeWidgetItem *item = view->currentItem();

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
	if (!m_simkit->getBreakpointManager()->getMemoryBreaks().contains(item->data(0, Qt::UserRole).toInt())) {
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

void Peripherals::removePeripheralItem(PeripheralItem *it) {
	for (int i = 0; i < view->topLevelItemCount(); ++i) {
		PeripheralItem *item = dynamic_cast<PeripheralItem *>(view->topLevelItem(i));
		if (item == it) {
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
