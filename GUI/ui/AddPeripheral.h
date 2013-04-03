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

#include "ui_AddPeripheral.h"

class PeripheralManager;

class AddPeripheral : public QDialog, public Ui::AddPeripheral
{
	Q_OBJECT

	public:
		AddPeripheral(PeripheralManager *peripherals, QWidget *parent = 0);

		QString getPeripheral();

	private slots:
		void handleItemActivated(QTreeWidgetItem *, int);

	private:
		PeripheralManager *m_peripherals;
};

