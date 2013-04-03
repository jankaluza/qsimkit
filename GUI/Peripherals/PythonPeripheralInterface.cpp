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

#include "PythonPeripheralInterface.h"
#include "PythonPeripheral.h"
#include "Script/Script.h"
#include <QApplication>
#include <QDebug>

PythonPeripheralInterface::PythonPeripheralInterface(Script *script) : m_script(script) {

}

PythonPeripheralInterface::~PythonPeripheralInterface() {
	delete m_script;
}

Peripheral *PythonPeripheralInterface::create() {
	qDebug() << "create";
	PythonQtObjectPtr p = m_script->eval("Peripheral()\n");
	Script *script = new Script(p);

	PythonPeripheral *peripheral = new PythonPeripheral(script);
	return peripheral;
}
