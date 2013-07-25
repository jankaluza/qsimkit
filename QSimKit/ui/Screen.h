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
#include <QTextStream>
#include <QDomDocument>
#include <QList>

#include "Peripherals/Peripheral.h"
#include "adevs.h"

class Package;
class ScreenObject;
class MCUManager;
class MCU;
class ConnectionManager;
class PeripheralManager;
class SimulationObjectWrapper;

class Screen : public QWidget
{
	Q_OBJECT

	public:
		Screen(QWidget *parent = 0);

		void setMCU(MCU *cpu);
		MCU *getMCU();

		void addObject(ScreenObject *obj);

		void addObject(const QPoint &pos);

		int objectId(ScreenObject *obj) {
			return m_objects.indexOf(obj);
		}

		ScreenObject *objectFromId(int id) {
			return m_objects[id];
		}

		void removeObject(ScreenObject *obj);

		void clear();

		void save(QTextStream &stream);
		bool load(QDomDocument &doc);

		void prepareSimulation(adevs::Digraph<double> *dig);
		void setSimulator(adevs::Simulator<SimulationEvent> *sim);

		ScreenObject *getObject(int x, int y);
		int getPin(ScreenObject *object, int x, int y);

		void setPeripheralManager(PeripheralManager *peripherals) {
			m_peripherals = peripherals;
		}

		void setMCUManager(MCUManager *mcuManager) {
			m_mcuManager = mcuManager;
		}

		std::map<ScreenObject *, SimulationObjectWrapper *> &getWrappers() {
			return wrappers;
		}

	signals:
		void onPeripheralAdded(QObject *peripheral);
		void onPeripheralRemoved(QObject *peripheral);
		void onPinTracked(QObject *peripheral, int pin);
		void onPinUntracked(QObject *peripheral, int pin);

	protected:
		void paintEvent(QPaintEvent *e);
		void mouseMoveEvent(QMouseEvent *event);
		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);

	private:
		void resizeAccordingToObjects();
		void showObjectMenu(ScreenObject *object, const QPoint &pos);
		void showPinMenu(ScreenObject *object, int pin, const QPoint &pos);
		void showScreenMenu(const QPoint &pos);

	private:
		QList<ScreenObject *> m_objects;
		ScreenObject *m_moving;
		int m_movingX;
		int m_movingY;		
		ConnectionManager *m_conns;
		PeripheralManager *m_peripherals;
		MCUManager *m_mcuManager;
		std::map<ScreenObject *, SimulationObjectWrapper *> wrappers;
		std::map<ScreenObject *, QList<int> > m_trackedPins;
};

