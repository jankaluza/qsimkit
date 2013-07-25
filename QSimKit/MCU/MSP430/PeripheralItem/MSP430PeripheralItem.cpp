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

#include "MSP430PeripheralItem.h"

#include "ui/QSimKit.h"
#include "MCU/MSP430/MSP430.h"
#include "Peripherals/Peripheral.h"
#include "CPU/Memory/RegisterSet.h"
#include "CPU/Memory/Register.h"

#include "RegistersItem.h"
#include "TimerAItem.h"
#include "TimerBItem.h"
#include "BasicClockItem.h"

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

MSP430PeripheralItem::MSP430PeripheralItem(MCU_MSP430 *cpu) {
	m_cpu = cpu;
	setText(0, "MSP430");
	setFirstColumnSpanned(true);
	setExpanded(true);

	m_registersItem = new RegistersItem(m_cpu);
	addChild(m_registersItem);

	m_bcItem = new BasicClockItem(m_cpu);
	addChild(m_bcItem);

	m_timerAItem = new TimerAItem(m_cpu);
	addChild(m_timerAItem);

	m_timerBItem = new TimerBItem(m_cpu);
	addChild(m_timerBItem);
}

MSP430PeripheralItem::~MSP430PeripheralItem() {
	
}

void MSP430PeripheralItem::refresh() {
	m_registersItem->refresh();
	m_bcItem->refresh();
	m_timerAItem->refresh();
	m_timerBItem->refresh();
}
