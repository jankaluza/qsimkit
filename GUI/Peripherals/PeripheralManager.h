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

#include <QWidget>
#include <QString>
#include <QChar>
#include <QRect>
#include <QMap>
#include <QDir>

class PeripheralInterface;
class PeripheralManager;
class Peripheral;
class ScriptEngine;

class PeripheralInfo {
	public:
		PeripheralInfo() : m_peripheral(0) {}
		virtual ~PeripheralInfo() {}

		Peripheral *create();

		const QString &getName() const { return m_name; }
		const QString &getLibrary() const { return m_library; }

	private:
		PeripheralInterface *m_peripheral;
		QString m_name;
		QString m_library;

	friend class PeripheralManager;
};

typedef QMap<QString, PeripheralInfo> PeripheralList;

class PeripheralManager : public QObject {
	Q_OBJECT

	public:
		PeripheralManager();
		~PeripheralManager();

		void loadPeripherals();
		PeripheralInterface *loadBinaryPeripheral(QString dir);
		PeripheralInterface *loadPythonPeripheral(QString dir, QString name);

		PeripheralInfo &getPeripheral(const QString &name) {
			return m_peripherals[name];
		}

		const PeripheralList &getPeripherals() {
			return m_peripherals;
		}

	private:
		bool loadXML(QString xml);

		PeripheralList m_peripherals;
		ScriptEngine *m_scriptEngine;

};

