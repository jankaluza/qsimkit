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

#include "DockWidgets/Peripherals/PeripheralItem.h"

class MCU_MSP430;
class RegistersItem;
class TimerAItem;
class TimerBItem;
class BasicClockItem;
class USIItem;
class USARTItem;

class MSP430PeripheralItem : public PeripheralItem
{
	public:
		MSP430PeripheralItem(MCU_MSP430 *cpu);
		~MSP430PeripheralItem();

		void refresh();

	private:
		MCU_MSP430 *m_cpu;
		RegistersItem *m_registersItem;
		TimerAItem *m_timerAItem;
		TimerBItem *m_timerBItem;
		BasicClockItem *m_bcItem;
		USIItem *m_usiItem;
		USARTItem *m_usartItem;
};

