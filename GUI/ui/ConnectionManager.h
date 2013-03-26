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

#include <QPoint>
#include <QPainter>
#include <QMouseEvent>
#include <vector>
#include <map>

#include "adevs.h"

class ScreenObject;
class SimulationObjectWrapper;
class SimulationEvent;


typedef struct {
	ScreenObject *from;
	int fport;
	ScreenObject *to;
	int tport;
	std::vector<QPoint> points;
} Connection;

class ConnectionManager
{

	public:
		ConnectionManager();
		virtual ~ConnectionManager() {}

		void addConnection(ScreenObject *from, int fport, ScreenObject *to, int tport, const std::vector<QPoint> &points);

		void paint(QPainter &p);

		bool mouseMoveEvent(QMouseEvent *event);

		void prepareSimulation(adevs::Digraph<SimulationEvent *> *dig, std::map<ScreenObject *, SimulationObjectWrapper *> &wrappers);

	private:
		std::list<Connection> m_conns;
		QPoint *m_moving;
		int m_movingX;
		int m_movingY;
};

