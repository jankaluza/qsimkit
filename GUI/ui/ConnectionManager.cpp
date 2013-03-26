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

#include "ConnectionManager.h"
#include "ScreenObject.h"

#include <QDebug>

ConnectionManager::ConnectionManager() {
	
}

void ConnectionManager::addConnection(ScreenObject *from, int fport, ScreenObject *to, int tport, const std::vector<QPoint> &points) {
	Connection c;
	c.from = from;
	c.fport = fport;
	c.to = to;
	c.tport = tport;
	c.points = points;

	m_conns.push_back(c);
}

void ConnectionManager::prepareSimulation(adevs::Digraph<SimulationEvent *> *dig, std::map<ScreenObject *, SimulationObjectWrapper *> &wrappers) {
	for (std::list<Connection>::iterator it = m_conns.begin(); it != m_conns.end(); ++it) {
		qDebug() << wrappers[it->from] << it->fport << wrappers[it->to] << it->tport;
		dig->couple(wrappers[it->from], it->fport, wrappers[it->to], it->tport);
		dig->couple(wrappers[it->to], it->tport, wrappers[it->from], it->fport);
	}
}

void ConnectionManager::paint(QPainter &p) {
	QPoint from;
	QPoint to;
	for (std::list<Connection>::iterator it = m_conns.begin(); it != m_conns.end(); ++it) {
		from = it->from->getPins()[it->fport].rect.adjusted(it->from->x(), it->from->y(), it->from->x(), it->from->y()).center();
		std::vector<QPoint>::iterator it2 = it->points.begin();
		if (it2 != it->points.end()) {
			for (it2++; it2 != it->points.end() - 1; ++it2) {
				to = *it2;
				p.drawLine(from, to);
				from = to;
			}
			to = it->to->getPins()[it->tport].rect.adjusted(it->to->x(), it->to->y(), it->to->x(), it->to->y()).center();
			p.drawLine(from, to);
		}
	}
}
