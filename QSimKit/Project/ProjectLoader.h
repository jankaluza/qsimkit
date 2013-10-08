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

#include <stdint.h>
#include <QObject>
#include <QList>
#include <QDomDocument>

class MCU;
class ScreenObject;
class MCUManager;
class PeripheralManager;

class ProjectLoader : public QObject {
	Q_OBJECT
	public:
		ProjectLoader(MCUManager *mcuManager, PeripheralManager *perManager);
		~ProjectLoader();

		bool load(QDomDocument &doc, QString &error);

		MCU *getMCU() {
			return m_mcu;
		}

		QList<ScreenObject *> &getObjects() {
			return m_objects;
		}

	private:
		MCU *m_mcu;
		QList<ScreenObject *> m_objects;
		MCUManager *m_mcuManager;
		PeripheralManager *m_perManager;
};


