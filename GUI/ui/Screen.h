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
#include <QList>

#include "adevs.h"

class Package;
class ScreenObject;
class MSP430;
class SimulationEvent;
class ConnectionManager;
class PeripheralManager;

class Screen : public QWidget
{
	Q_OBJECT

	public:
		Screen(QWidget *parent = 0);

		void setCPU(MSP430 *cpu);
		MSP430 *getCPU();

		void addObject(ScreenObject *obj) {
			m_objects.append(obj);
		}

		void addObject(const QPoint &pos);

		void removeObject(ScreenObject *obj);

		void prepareSimulation(adevs::Digraph<SimulationEvent *> *dig);

		ScreenObject *getObject(int x, int y);
		int getPin(ScreenObject *object, int x, int y);

		void setPeripheralManager(PeripheralManager *peripherals) {
			m_peripherals = peripherals;
		}

	protected:
		void paintEvent(QPaintEvent *e);
		void mouseMoveEvent(QMouseEvent *event);
		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);

	private:
		void resizeAccordingToObjects();
		void showObjectMenu(ScreenObject *object, const QPoint &pos);
		void showScreenMenu(const QPoint &pos);

	private:
		QList<ScreenObject *> m_objects;
		ScreenObject *m_moving;
		int m_movingX;
		int m_movingY;		
		ConnectionManager *m_conns;
		PeripheralManager *m_peripherals;
};

