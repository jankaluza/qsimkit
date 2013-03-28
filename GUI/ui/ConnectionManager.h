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
#include <QPointF>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <vector>
#include <map>

#include "adevs.h"

class ScreenObject;
class SimulationObjectWrapper;
class SimulationEvent;
class Screen;


typedef struct {
	ScreenObject *from;
	int fpin;
	ScreenObject *to;
	int tpin;
	std::vector<QPoint> points;
} Connection;

class ConnectionManager
{
	typedef std::list<Connection *> ConnectionList;

	public:
		ConnectionManager(Screen *screen);
		virtual ~ConnectionManager() {}

		void addConnection(ScreenObject *from, int fpin, ScreenObject *to, int tpin, const std::vector<QPoint> &points);

		void paint(QPainter &p);

		bool mouseMoveEvent(QMouseEvent *event);
		bool mousePressEvent(QMouseEvent *event);
		bool mouseReleaseEvent(QMouseEvent *event);

		void prepareSimulation(adevs::Digraph<SimulationEvent *> *dig, std::map<ScreenObject *, SimulationObjectWrapper *> &wrappers);

		void movePins(ScreenObject *object);

	private:
		void paint(QPainter &p, Connection *c);
		Connection *getPoint(int x, int y, int &point);
		Connection *getConnection(int x, int y, int &point, QPointF *intersectPnt = 0);
		void removeConnection(Connection *c);
		void removePoint(Connection *c, int point);
		void removeDuplicatePoints(Connection *c);

	private:
		ConnectionList m_conns;
		QPoint *m_moving;
		int m_movingX;
		int m_movingY;
		std::vector<QPoint> m_points;
		Screen *m_screen;
		int m_fromPin;
		ScreenObject *m_fromObject;
		Connection *m_movingConn;
};

