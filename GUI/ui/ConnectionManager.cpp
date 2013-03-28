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
 * Foundation, Inc->, 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 **/

#include "ConnectionManager.h"
#include "ScreenObject.h"
#include "Screen.h"

#include <QAction>
#include <QMenu>
#include <QDebug>

ConnectionManager::ConnectionManager(Screen *screen) {
	m_moving = NULL;
	m_screen = screen;
	m_fromPin = -1;
	m_fromObject = 0;
	m_movingConn = 0;
}

void ConnectionManager::addConnection(ScreenObject *from, int fpin, ScreenObject *to, int tpin, const std::vector<QPoint> &points) {
	if (points.size() < 3) {
		return;
	}

	Connection *c = new Connection;
	c->from = from;
	c->fpin = fpin;
	c->to = to;
	c->tpin = tpin;
	c->points = points;

	c->points[0] = c->from->getPins()[c->fpin].rect.center();
	c->points[c->points.size() - 1] = c->to->getPins()[c->tpin].rect.center();

	removeDuplicatePoints(c);

	m_conns.push_back(c);
}

void ConnectionManager::prepareSimulation(adevs::Digraph<SimulationEvent *> *dig, std::map<ScreenObject *, SimulationObjectWrapper *> &wrappers) {
	for (ConnectionList::iterator it = m_conns.begin(); it != m_conns.end(); ++it) {
		dig->couple(wrappers[(*it)->from], (*it)->fpin, wrappers[(*it)->to], (*it)->tpin);
		dig->couple(wrappers[(*it)->to], (*it)->tpin, wrappers[(*it)->from], (*it)->fpin);
	}
}

void ConnectionManager::movePins(ScreenObject *object) {
	for (ConnectionList::iterator it = m_conns.begin(); it != m_conns.end(); ++it) {
		Connection *c = *it;
		if (c->from == object) {
			c->points[0] = c->from->getPins()[c->fpin].rect.center();
		}
		if (c->to == object) {
			c->points[c->points.size() - 1] = c->to->getPins()[c->tpin].rect.center();
		}
	}
}

void ConnectionManager::paint(QPainter &p, Connection *c) {
	QPoint from;
	QPoint to;

	from = c->points[0];
	p.drawRect(from.x() - 5, from.y() - 5, 12, 12);
	for (int i = 1; i < c->points.size(); ++i) {
		to = c->points[i];
		p.drawLine(from, to);
		from = to;
		p.drawRect(to.x() - 5, to.y() - 5, 12, 12);
	}
}

void ConnectionManager::paint(QPainter &p) {
	for (ConnectionList::iterator it = m_conns.begin(); it != m_conns.end(); ++it) {
		paint(p, *it);
	}

	if (m_fromPin != -1) {
		QPoint from = QPoint(m_movingX, m_movingY);
		QPoint to;
		std::vector<QPoint>::iterator it = m_points.begin();
		if (it != m_points.end()) {
			from = *it;
			for (it++; it != m_points.end(); ++it) {
				to = *it;
				p.drawLine(from, to);
				from = to;
			}
		}


		to = m_screen->mapFromGlobal(QCursor::pos());

		QPointF fto(to);
		QPointF ffrom(from);

		if (m_screen->getObject(from.x(), to.y()) == m_fromObject ||
			(!m_points.empty() && m_points[m_points.size()-1].y() == to.y())) {
			p.drawLine(from, QPoint(to.x(), from.y()));
			p.drawLine(QPoint(to.x(), from.y()), to);
		}
		else {
			p.drawLine(from, QPoint(from.x(), to.y()));
			p.drawLine(QPoint(from.x(), to.y()), to);
		}
	}
}

Connection *ConnectionManager::getPoint(int x, int y, int &point) {
	QPoint p;
	point = -1;

	for (ConnectionList::iterator it = m_conns.begin(); it != m_conns.end(); ++it) {
		Connection *c = *it;
		for (int i = 0; i < c->points.size(); ++i) {
			p = c->points[i];
			QRect r(p.x() - 6, p.y() - 6, 12, 12);
			if (r.contains(x, y)) {
				point = i;
				return (*it);
			}
		}
	}

	return 0;
}

Connection *ConnectionManager::getConnection(int x, int y, int &point, QPointF *intersectPnt) {
	QPointF from;
	QPointF to;

	for (ConnectionList::iterator it = m_conns.begin(); it != m_conns.end(); ++it) {
		Connection *c = *it;
		from = c->points[0];
		for (int i = 1; i < c->points.size(); ++i) {
			to = c->points[i];
			QLineF line(from, to);
			QLineF line2(x-5, y-5, x+5, y+5);
			if (line.intersect(line2, intersectPnt)==QLineF::BoundedIntersection) {
				point = i;
				return (*it);
			}
			else {
				QLineF line3(x-5, y+5, x+5, y-5);
				if (line.intersect(line3, intersectPnt)==QLineF::BoundedIntersection) {
					point = i;
					return (*it);
				}
			}
			from = to;
		}
	}

	return 0;
}

void ConnectionManager::removeConnection(Connection *c) {
	m_conns.remove(c);
	delete c;
}

void ConnectionManager::removePoint(Connection *c, int point) {
	c->points.erase(c->points.begin() + point);
	if (c->points.size() < 2) {
		removeConnection(c);
	}
}

void ConnectionManager::removeDuplicatePoints(Connection *c) {
	bool removedSome = false;
	bool remove = true;

	while(remove) {
		remove = false;
		for (int point1 = 0; point1 < c->points.size(); ++point1) {
			QRect r1(c->points[point1].x() - 6, c->points[point1].y() - 6, 12, 12);
			int point2 = point1 + 1;

			for (; point2 < c->points.size(); ++point2) {
				QRect r2(c->points[point2].x() - 6, c->points[point2].y() - 6, 12, 12);

				if (r1.intersects(r2)) {
					remove = true;
					break;
				}
			}

			if (remove) {
				for (int i = point1; i < point2; i++) {
					removePoint(c, point1);
					removedSome = true;
				}
				break;
			}
		}
	}

	if (removedSome) {
		m_screen->repaint();
	}
}

bool ConnectionManager::mousePressEvent(QMouseEvent *event) {
	if (event->button() == Qt::RightButton) {
		if (m_fromPin != -1) {
			m_fromPin = -1;
			m_points.clear();
			return true;
		}

		int point;
		Connection *c = getPoint(event->x(), event->y(), point);
		if (point != -1) {
			QList<QAction *> actions;
			actions.append(new QAction("Remove point", 0));
			QAction *action = QMenu::exec(actions, event->globalPos(), 0, 0);
			if (action) {
				removePoint(c, point);
			}
			return true;
		}

		c = getConnection(event->x(), event->y(), point);
		if (c) {
			QList<QAction *> actions;
			actions.append(new QAction("Remove connection", 0));
			QAction *action = QMenu::exec(actions, event->globalPos(), 0, 0);
			if (action) {
				removeConnection(c);
			}
			return true;
		}
	}
	else if (event->button() == Qt::LeftButton) {
		if (m_fromPin != -1) {
			QPoint from = QPoint(m_movingX, m_movingY);
			QPoint to = event->pos();
			ScreenObject *object = m_screen->getObject(event->x(), event->y());
			if (object) {
				int pin = m_screen->getPin(object, event->x(), event->y());
				if (pin != -1) {
					to = object->getPins()[pin].rect.center();
				}
			}

			bool firstPoints = m_points.empty();

			if (firstPoints) {
				m_points.push_back(from);
			}

			if (m_screen->getObject(from.x(), to.y()) == m_fromObject/* &&
				m_screen->getPin(m_moving, from.x(), to.y()) != -1*/) {
				m_points.push_back(QPoint(to.x(), from.y()));
			}
			else {
				m_points.push_back(QPoint(from.x(), to.y()));
			}

			m_points.push_back(to);

			if (object) {
				int pin = m_screen->getPin(object, event->x(), event->y());
				if (pin != -1) {
					addConnection(m_fromObject, m_fromPin, object, pin, m_points);
					m_fromPin = -1;
					m_points.clear();
					return true;
				}
			}

			m_movingX = event->x();
			m_movingY = event->y();
			return true;
		}

		int point;
		Connection *c = getPoint(event->x(), event->y(), point);
		if (point != -1) {
			if (point == 0) {
				m_fromObject = c->from;
			}
			else if (point == c->points.size() - 1) {
				m_fromObject = c->to;
			}
			else {
				m_fromObject = 0;
			}
			m_movingConn = c;
			m_moving = &c->points[point];
			m_movingX = event->x();
			m_movingY = event->y();
			return true;
		}

		ScreenObject *object = m_screen->getObject(event->x(), event->y());
		if (!object) {
			return false;
		}

		m_fromPin = m_screen->getPin(object, event->x(), event->y());
		if (m_fromPin != -1) {
			m_fromObject = object;
			m_movingX = object->getPins()[m_fromPin].rect.center().x();
			m_movingY = object->getPins()[m_fromPin].rect.center().y();
			return true;
		}
	}
	return false;
}

bool ConnectionManager::mouseReleaseEvent(QMouseEvent *event) {
	if (event->button() == Qt::LeftButton) {
		if (m_moving && m_fromObject && m_movingConn) {
			ScreenObject *object = m_screen->getObject(event->x(), event->y());
			if (object) {
				int pin = m_screen->getPin(object, event->x(), event->y());
				if (pin != -1) {
					if (m_fromObject == m_movingConn->to) {
						m_movingConn->tpin = pin;
						m_movingConn->to = object;
					}
					else {
						m_movingConn->fpin = pin;
						m_movingConn->from = object;
					}
					m_moving->setX(object->getPins()[pin].rect.center().x());
					m_moving->setY(object->getPins()[pin].rect.center().y());
				}
				else {
					if (m_fromObject == m_movingConn->from) {
						m_moving->setX(m_fromObject->getPins()[m_movingConn->fpin].rect.center().x());
						m_moving->setY(m_fromObject->getPins()[m_movingConn->fpin].rect.center().y());
					}
					else {
						m_moving->setX(m_fromObject->getPins()[m_movingConn->tpin].rect.center().x());
						m_moving->setY(m_fromObject->getPins()[m_movingConn->tpin].rect.center().y());
					}
				}
			}
			else {
				if (m_fromObject == m_movingConn->from) {
					m_moving->setX(m_fromObject->getPins()[m_movingConn->fpin].rect.center().x());
					m_moving->setY(m_fromObject->getPins()[m_movingConn->fpin].rect.center().y());
				}
				else {
					m_moving->setX(m_fromObject->getPins()[m_movingConn->tpin].rect.center().x());
					m_moving->setY(m_fromObject->getPins()[m_movingConn->tpin].rect.center().y());
				}
			}
			m_fromObject = 0;
		}
		else if (m_moving && m_movingConn) {
			removeDuplicatePoints(m_movingConn);
			m_moving = 0;
			m_movingConn = 0;
		}
	}
}

bool ConnectionManager::mouseMoveEvent(QMouseEvent *event) {
	if (m_fromPin != -1) {
		m_screen->repaint();
		return true;
	}

	if (event->buttons() & Qt::LeftButton) {
		if (m_moving) {
			m_moving->setX(m_moving->x() - (m_movingX - event->x()));
			m_moving->setY(m_moving->y() - (m_movingY - event->y()));
			m_movingX = event->x();
			m_movingY = event->y();
			m_screen->repaint();
			return true;
		}
		else {
			int point;
			Connection *c = getPoint(event->x(), event->y(), point);
			if (point != -1) {
				m_moving = &c->points[point];
			}
			else {
				QPointF intersectPnt;
				int point;
				c = getConnection(event->x(), event->y(), point, &intersectPnt);
				if (c) {
					m_fromObject = 0;
					m_movingConn = c;
					QPoint p = intersectPnt.toPoint();
					c->points.insert(c->points.begin() + point, p);
					m_moving = &(c->points[point]);
				}
			}
		}

		m_movingX = event->x();
		m_movingY = event->y();
	}
	else {
		int point;
		Connection *c = getPoint(event->x(), event->y(), point);
		if (point != -1) {
			return true;
		}
		else {
			QPointF intersectPnt;
			int point;
			c = getConnection(event->x(), event->y(), point, &intersectPnt);
			if (c) {
				return true;
			}
		}
		m_moving = 0;
		m_fromObject = 0;
		m_movingConn = 0;
	}
	return false;
}
