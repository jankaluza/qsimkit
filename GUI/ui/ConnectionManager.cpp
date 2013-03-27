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
	m_moving = NULL;
}

void ConnectionManager::addConnection(ScreenObject *from, int fpin, ScreenObject *to, int tpin, const std::vector<QPoint> &points) {
	if (points.size() < 3) {
		return;
	}

	Connection c;
	c.from = from;
	c.fpin = fpin;
	c.to = to;
	c.tpin = tpin;
	c.points = points;

	m_conns.push_back(c);
}

void ConnectionManager::prepareSimulation(adevs::Digraph<SimulationEvent *> *dig, std::map<ScreenObject *, SimulationObjectWrapper *> &wrappers) {
	for (ConnectionList::iterator it = m_conns.begin(); it != m_conns.end(); ++it) {
		dig->couple(wrappers[it->from], it->fpin, wrappers[it->to], it->tpin);
		dig->couple(wrappers[it->to], it->tpin, wrappers[it->from], it->fpin);
	}
}

void ConnectionManager::paint(QPainter &p, Connection &c) {
	QPoint from;
	QPoint to;

	from = c.from->getPins()[c.fpin].rect.center();
	for (int i = 1; i < c.points.size() - 1; ++i) {
		to = c.points[i];
		p.drawLine(from, to);
		from = to;
		p.drawRect(to.x() - 5, to.y() - 5, 10, 10);
	}
	to = c.to->getPins()[c.tpin].rect.center();
	p.drawLine(from, to);
}

void ConnectionManager::paint(QPainter &p) {
	for (ConnectionList::iterator it = m_conns.begin(); it != m_conns.end(); ++it) {
		paint(p, *it);
	}
}

bool ConnectionManager::mouseMoveEvent(QMouseEvent *event) {
	if (event->buttons() & Qt::LeftButton) {
		if (m_moving) {
			m_moving->setX(m_moving->x() - (m_movingX - event->x()));
			m_moving->setY(m_moving->y() - (m_movingY - event->y()));
			m_movingX = event->x();
			m_movingY = event->y();
			return true;
		}
		else {
			QPointF from;
			QPointF to;
			QPointF intersectPnt;
			for (std::list<Connection>::iterator it = m_conns.begin(); it != m_conns.end(); ++it) {
				from = it->from->getPins()[it->fpin].rect.center();
				std::vector<QPoint>::iterator it2 = it->points.begin();
				if (it2 != it->points.end()) {
					for (it2++; it2 != it->points.end() - 1; ++it2) {
						to = *it2;
						QRectF r(to.x() - 5, to.y() - 5, 10, 10);
						if (r.contains(event->x(), event->y())) {
							m_moving = &(*it2);
							break;
						}
						else {
							QLineF line(from, to);
							QLineF line2(event->x()-10, event->y()-10, event->x()+10, event->y()+10);
							if (line.intersect(line2, &intersectPnt)==QLineF::BoundedIntersection) {
								QPoint p = intersectPnt.toPoint();
								it2 = it->points.insert(it2, p);
								m_moving = &(*it2);
								break;
							}
						}
						from = to;
					}
					to = it->to->getPins()[it->tpin].rect.center();
					QLineF line(from, to);
					QLineF line2(event->x()-10, event->y()-10, event->x()+10, event->y()+10);
					if (line.intersect(line2, &intersectPnt)==QLineF::BoundedIntersection) {
						QPoint p = intersectPnt.toPoint();
						it2 = it->points.insert(it2, p);
						m_moving = &(*it2);
						break;
					}
					if (m_moving)
						break;
				}
			}
		}

		m_movingX = event->x();
		m_movingY = event->y();
	}
	else {
		m_moving = 0;
	}
	return false;
}
