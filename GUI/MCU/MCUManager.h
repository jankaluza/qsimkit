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

class MCUInterface;
class MCUManager;
class MCU;

class MCUInfo {
	public:
		MCUInfo() : m_mcu(0) {}
		virtual ~MCUInfo() {}

		MCU *create(const QString &variant);

		const QString &getName() const { return m_name; }
		const QString &getLibrary() const { return m_library; }

	private:
		MCUInterface *m_mcu;
		QString m_name;
		QString m_library;

	friend class MCUManager;
};

typedef QMap<QString, MCUInfo> MCUList;

class MCUManager : public QObject {
	Q_OBJECT

	public:
		MCUManager();
		~MCUManager();

		void loadMCUs();
		MCUInterface *loadBinaryMCU(QString dir);

		MCUInfo &getMCU(const QString &name) {
			return m_mcu[name];
		}

		const MCUList &getMCUs() {
			return m_mcu;
		}

	private:
		bool loadXML(QString xml);

		MCUList m_mcu;
};

