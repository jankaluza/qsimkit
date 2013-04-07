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
#include <QDockWidget>
#include <QTreeWidgetItem>

#include "ui_Registers.h"

class MSP430;
class QSimKit;

class Registers : public QDockWidget, public Ui::Registers
{
	Q_OBJECT

	public:
		Registers(QSimKit *simkit);

		void setCPU(MSP430 *cpu);

		void refresh();

	private:
		MSP430 *m_cpu;
		QSimKit *m_simkit;
};

