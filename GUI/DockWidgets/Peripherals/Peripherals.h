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

#pragma once

#include <QDialog>
#include <QString>
#include <QTimer>
#include <DockWidgets/DockWidget.h>
#include <QTreeWidgetItem>

#include "ui_Peripherals.h"

class MCU;
class QSimKit;
class PeripheralItem;

class Peripherals : public DockWidget, public Ui::Peripherals
{
	Q_OBJECT

	public:
		Peripherals(QSimKit *simkit);

		void setMCU(MCU *mcu);

		void refresh();

		void addPeripheralItem(PeripheralItem *item);

		void removePeripheralItem(PeripheralItem *item);

	public slots:
		void addPeripheral(QObject *peripheral);

		void removePeripheral(QObject *peripheral);

		void handleContextMenu(const QPoint &point);

	private:
		void addBreakpoint();
		void removeBreakpoint();

	private:
		MCU *m_mcu;
		QSimKit *m_simkit;
		QList<QTreeWidgetItem *> m_breakpoints;
};

