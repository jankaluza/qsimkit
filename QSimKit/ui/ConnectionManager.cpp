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
#include "Peripherals/SimulationObject.h"
#include "ConnectionNode.h"
#include "math.h"

#include <QAction>
#include <QMenu>
#include <QDebug>

#define NORM(X) (X - X % 12 + 6)

ConnectionManager::ConnectionManager(Screen *screen) {
	m_moving = NULL;
	m_screen = screen;
	m_fromPin = -1;
	m_fromObject = 0;
	m_movingConn = 0;
	m_removingUselessNode = false;
}

void ConnectionManager::save(QTextStream &stream) {
	stream << "<connections>\n";
	int id = 0;
	for (ConnectionList::iterator it = m_conns.begin(); it != m_conns.end(); ++it) {
		Connection *c = *it;
		stream << "    <connection id='" << id++ << "' ";
		stream << "from='" << m_screen->objectId(c->from) << "' ";
		stream << "fpin='" << c->fpin << "' ";
		stream << "to='" << m_screen->objectId(c->to) << "' ";
		stream << "tpin='" << c->tpin << "'>\n";

		for (int i = 0; i < c->points.size(); ++i) {
			stream << "        <point x='" << c->points[i].x() << "' y='" << c->points[i].y() << "'/>\n";
		}

		stream << "    </connection>\n";
	}
	stream << "</connections>\n";
}

void ConnectionManager::load(QDomDocument &doc) {
	QDomElement root = doc.firstChild().toElement();
	QDomElement connections = root.firstChildElement("connections");

	for(QDomNode node = connections.firstChild(); !node.isNull(); node = node.nextSibling()) {
		QDomElement c = node.toElement();

		ScreenObject *from = m_screen->objectFromId(c.attribute("from").toInt());
		ScreenObject *to = m_screen->objectFromId(c.attribute("to").toInt());
		int fpin = c.attribute("fpin").toInt();
		int tpin = c.attribute("tpin").toInt();

		std::vector<QPoint> points;
		for(QDomNode pnode = c.firstChild(); !pnode.isNull(); pnode = pnode.nextSibling()) {
			QDomElement point = pnode.toElement();
			points.push_back(QPoint(point.attribute("x").toInt(), point.attribute("y").toInt()));
		}

		addConnection(from, fpin, to, tpin, points);
	}
}

Connection *ConnectionManager::addConnection(ScreenObject *from, int fpin, ScreenObject *to, int tpin, const std::vector<QPoint> &points) {
	if (points.size() < 2) {
		return 0;
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

	if (dynamic_cast<ConnectionNode *>(c->to)) {
		dynamic_cast<ConnectionNode *>(c->to)->setConnection(c->tpin, c);
	}

	if (dynamic_cast<ConnectionNode *>(c->from)) {
		dynamic_cast<ConnectionNode *>(c->from)->setConnection(c->fpin, c);
	}

	m_conns.push_back(c);
	return c;
}

void ConnectionManager::prepareSimulation(adevs::Digraph<double> *dig, std::map<ScreenObject *, SimulationObjectWrapper *> &wrappers) {
	for (ConnectionList::iterator it = m_conns.begin(); it != m_conns.end(); ++it) {
		Connection *c = *it;
		qDebug() << "connecting" << c->from << c->fpin << c->to << c->tpin;
		dig->couple(wrappers[c->from], c->fpin, wrappers[c->to], c->tpin);
		dig->couple(wrappers[c->to], c->tpin, wrappers[c->from], c->fpin);
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

void ConnectionManager::objectMoved(ScreenObject *object) {
	for (ConnectionList::iterator it = m_conns.begin(); it != m_conns.end(); ++it) {
		Connection *c = *it;
		if (c->from == object || c->to == object) {
			removeDuplicatePoints(c);
		}
	}
}

void ConnectionManager::objectRemoved(ScreenObject *object) {
	if (m_removingUselessNode)
		return;

	ConnectionNode *node = dynamic_cast<ConnectionNode *>(object);
	if (node) {
		bool stop = false;
		for (int i = 0; i < 4; ++i) {
			Connection *c = node->getConnection(i);
			if (!c) {
				continue;
			}

			stop = node->isUseless();
			m_removingUselessNode = true;
			removeConnection(c);
			m_removingUselessNode = false;
			if (stop) {
				return;
			}
		}
		return;
	}

	std::vector<Connection *> toRemove;
	for (ConnectionList::iterator it = m_conns.begin(); it != m_conns.end(); ++it) {
		Connection *c = *it;
		if (c->from == object || c->to == object) {
			toRemove.push_back(c);
		}
	}

	for (int i = 0; i < toRemove.size(); ++i) {
		removeConnection(toRemove[i]);
	}
}

void ConnectionManager::paint(QPainter &p, Connection *c) {
	QPoint from;
	QPoint to;

// 	p.setPen(QPen(QColor(194, 194, 194), 1, Qt::SolidLine));
	from = c->points[0];
// 	p.drawRect(from.x() - 5, from.y() - 5, 12, 12);
	for (int i = 1; i < c->points.size(); ++i) {
		to = c->points[i];
		p.setPen(QPen(Qt::black, 2, Qt::SolidLine));
		p.drawLine(from, to);
		from = to;
// 		p.setPen(QPen(QColor(194, 194, 194), 1, Qt::SolidLine));
// 		p.drawRect(to.x() - 5, to.y() - 5, 12, 12);
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
		to.setX(to.x() - to.x() % 12 + 6);
		to.setY(to.y() - to.y() % 12 + 6);

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

void ConnectionManager::removeUselessNode(ConnectionNode *node) {
	if (node->isUseless()) {
		Connection *c1 = 0;
		Connection *c2 = 0;
		int c1pin = 0;
		int c2pin = 0;
		for (int i = 0; i < 4; ++i) {
			Connection *c = node->getConnection(i);
			if (!c) {
				continue;
			}

			if (!c1) { c1 = c; c1pin = i;}
			else if (!c2) { c2 = c; c2pin = i; }
		}

		if (c1->to == node) {
			if (c1pin == 0) {
				c1->points[c1->points.size() - 1].setX(c1->points[c1->points.size() - 1].x() + 12);
			}
			else if (c1pin == 1) {
				c1->points[c1->points.size() - 1].setY(c1->points[c1->points.size() - 1].y() - 12);
			}
			else if (c1pin == 2) {
				c1->points[c1->points.size() - 1].setX(c1->points[c1->points.size() - 1].x() - 12);
			}
			else if (c1pin == 3) {
				c1->points[c1->points.size() - 1].setY(c1->points[c1->points.size() - 1].y() + 12);
			}
			if (c2->to == node) {
				qDebug() << "1x";
				c1->to = c2->from;
				c1->tpin = c2->fpin;
				c2->to = 0;
				for (int i = c2->points.size() - 1; i != -1; i--) {
					qDebug() << "push" << c2->points[i];
					c1->points.push_back(c2->points[i]);
				}
				for (int i = 0; i != c1->points.size(); i++) {
					qDebug() << "got" << c1->points[i];
				}
			}
			else {
				qDebug() << "2x";
				c1->to = c2->to;
				c1->tpin = c2->tpin;
				c2->from = 0;
				for (int i = 0; i != c2->points.size(); i++) {
					qDebug() << "push" << c2->points[i];
					c1->points.push_back(c2->points[i]);
				}
				for (int i = 0; i != c1->points.size(); i++) {
					qDebug() << "got" << c1->points[i];
				}
			}
		}
		else {
			if (c1pin == 0) {
				c1->points[0].setX(c1->points[0].x() + 12);
			}
			else if (c1pin == 1) {
				c1->points[0].setY(c1->points[0].y() - 12);
			}
			else if (c1pin == 2) {
				c1->points[0].setX(c1->points[0].x() - 12);
			}
			else if (c1pin == 3) {
				c1->points[0].setY(c1->points[0].y() + 12);
			}
			if (c2->to == node) {
				qDebug() << "3x";
				c1->from = c2->from;
				c1->fpin = c2->fpin;
				c2->to = 0;
				for (int i = 0; i != c1->points.size(); i++) {
					qDebug() << "push" << c1->points[i];
					c2->points.push_back(c1->points[i]);
				}
				c1->points = c2->points;
				for (int i = 0; i != c1->points.size(); i++) {
					qDebug() << "got" << c1->points[i];
				}
			}
			else {
				qDebug() << "4x";
				c1->from = c2->to;
				c1->fpin = c2->tpin;
				c2->from = 0;
				std::reverse(c2->points.begin(), c2->points.end());
				for (int i = 0; i != c1->points.size(); i++) {
					qDebug() << "push" << c1->points[i];
					c2->points.push_back(c1->points[i]);
				}
				c1->points = c2->points;
				for (int i = 0; i != c1->points.size(); i++) {
					qDebug() << "got" << c1->points[i];
				}
			}
		}
		removeConnection(c2);
		removeDuplicatePoints(c1);
		if (!m_removingUselessNode) {
			m_removingUselessNode = true;
			m_screen->removeObject(node);
			m_removingUselessNode = false;
		}
	}
}

void ConnectionManager::removeConnection(Connection *c) {
	ConnectionNode *node = 0;

	node = dynamic_cast<ConnectionNode *>(c->to);
	if (node) {
		node->removeConnection(c->tpin);
		c->to = 0;
		removeUselessNode(node);
	}

	node = dynamic_cast<ConnectionNode *>(c->from);
	if (node) {
		node->removeConnection(c->fpin);
		c->from = 0;
		removeUselessNode(node);
	}

	m_conns.remove(c);
	delete c;
}

void ConnectionManager::removePoint(Connection *c, int point) {
	c->points.erase(c->points.begin() + point);
	if (c->points.size() < 2) {
		removeConnection(c);
	}
	movePins(c->from);
	movePins(c->to);
}

void ConnectionManager::removeDuplicatePoints(Connection *c) {
	bool removedSome = false;
	bool remove = true;

	// If we have three points in single line, we can remove the
	// middle one
	for (int point1 = 1; point1 < c->points.size(); ++point1) {
		int point0 = point1 - 1;
		int point2 = point1 + 1;
		if (point2 == c->points.size()) {
			break;
		}

		// if (p1.x == p2.x == p3.x) or (p1.y == p2.y == p3.y)
		if ((NORM(c->points[point0].x()) == NORM(c->points[point1].x()) &&
			NORM(c->points[point2].x()) == NORM(c->points[point1].x())) ||
			(NORM(c->points[point0].y()) == NORM(c->points[point1].y()) &&
			NORM(c->points[point2].y()) == NORM(c->points[point1].y()))) {
			removedSome = true;
			qDebug() << "removing duplicate" << point1;
			removePoint(c, point1);
			point1--;
		}
	}

	// If we have two points on the same field in grid, we can remove
	// the second one.
	while(remove) {
		remove = false;
		for (int point1 = 0; point1 < c->points.size(); ++point1) {
			QRect r1(c->points[point1].x() - 6, c->points[point1].y() - 6, 12, 12);
			int point2 = point1 + 1;

			for (; point2 < c->points.size(); ++point2) {
				QRect r2(c->points[point2].x() - 6, c->points[point2].y() - 6, 12, 12);

				if (r1.intersects(r2)) {
// 					remove = true;
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

void ConnectionManager::addConnectionNode(Connection *c, int point, int x, int y, std::vector<QPoint> &points) {
	ConnectionNode *node = new ConnectionNode();
	node->setX(NORM(x) - node->width()/2);
	node->setY(NORM(y) - node->height()/2);

	std::vector<QPoint> newPoints;
	newPoints.push_back(QPoint(NORM(x), NORM(y)));
	while (c->points.size() != point + 1) {
		qDebug() << c->points[point + 1];
		newPoints.push_back(c->points[point + 1]);
		c->points.erase(c->points.begin() + point + 1);
	}

	m_screen->addObject(node);

	int addpin1 = 0;
	int addpin2 = 0;
	int addpin3 = 0;
#define PINPOS(POINT, VAR)\
qDebug() << abs(POINT.y() - NORM(y)) << abs(POINT.x() - NORM(x));\
if (abs(POINT.y() - NORM(y)) < abs(POINT.x() - NORM(x))) { \
	if (POINT.x() < NORM(x)) \
		VAR = 0; \
	else \
		VAR = 2; \
} \
else { \
	if (POINT.y() < NORM(y)) \
		VAR = 3; \
	else \
		VAR = 1; \
} \
qDebug() << VAR;

	qDebug() << newPoints[0] << newPoints[1] << newPoints[newPoints.size() - 1];
	PINPOS(c->points[c->points.size() - 2], addpin1);
	PINPOS(newPoints[newPoints.size() - 1], addpin2);
	PINPOS(points[points.size() - 2], addpin3);

	Connection *c2 = addConnection(node, addpin2, c->to, c->tpin, newPoints);
	node->setConnection(addpin2, c2);

	c->to = node;
	c->tpin = addpin1;

	movePins(c->to);
	movePins(c->from);

	node->setConnection(addpin1, c);

	Connection *c3 = addConnection(m_fromObject, m_fromPin, node, addpin3, points);
	node->setConnection(addpin3, c3);
}

bool ConnectionManager::mousePressEvent(QMouseEvent *event) {
	if (event->button() == Qt::RightButton) {
		if (m_fromPin != -1) {
			m_fromPin = -1;
			m_points.clear();
			return true;
		}

		int point;
		Connection *c = getPoint(NORM(event->x()), NORM(event->y()), point);
		if (point != -1) {
			// User can't remove first or last point like that
			if (point == 0 || point == c->points.size() - 1) {
				return false;
			}
			QList<QAction *> actions;
			actions.append(new QAction("Remove point", 0));
			QAction *action = QMenu::exec(actions, event->globalPos(), 0, 0);
			if (action) {
				removePoint(c, point);
			}
			return true;
		}

		c = getConnection(NORM(event->x()), NORM(event->y()), point);
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
			QPoint to(NORM(event->pos().x()), NORM(event->pos().y()));
			ScreenObject *object = m_screen->getObject(NORM(event->x()), NORM(event->y()));
			if (object) {
				int pin = m_screen->getPin(object, NORM(event->x()), NORM(event->y()));
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

// 			if (!firstPoints) {
				m_points.push_back(to);
// 			}

			int point;
			Connection *c = getPoint(NORM(event->x()), NORM(event->y()), point);
			if (point != -1) {
				addConnectionNode(c, point, event->x(), event->y(), m_points);

				m_fromPin = -1;
				m_points.clear();
// 				qDebug() << newPoints.size() << c->points.size() << "\n";
				return true;
			}
			else {
				QPointF intersectPnt;
				int point;
				c = getConnection(NORM(event->x()), NORM(event->y()), point, &intersectPnt);
				if (c) {
					QPoint p = intersectPnt.toPoint();
					p = QPoint(NORM(p.x()), NORM(p.y()));
					c->points.insert(c->points.begin() + point, p);
					addConnectionNode(c, point, event->x(), event->y(), m_points);
					m_fromPin = -1;
					m_points.clear();
					return true;
				}
			}

			if (object) {
				int pin = m_screen->getPin(object, NORM(event->x()), NORM(event->y()));
				if (pin != -1) {
					addConnection(m_fromObject, m_fromPin, object, pin, m_points);
					m_fromPin = -1;
					m_points.clear();
					return true;
				}
			}

			m_movingX = NORM(event->x());
			m_movingY = NORM(event->y());
			return true;
		}

// 		qDebug() << NORM(event->x()) << NORM(event->y());
		int point;
		Connection *c = getPoint(NORM(event->x()), NORM(event->y()), point);
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
			m_movingX = NORM(event->x());
			m_movingY = NORM(event->y());
			return true;
		}

		ScreenObject *object = m_screen->getObject(NORM(event->x()), NORM(event->y()));
		if (!object) {
			return false;
		}

		m_fromPin = m_screen->getPin(object, NORM(event->x()), NORM(event->y()));
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
			ScreenObject *object = m_screen->getObject(NORM(event->x()), NORM(event->y()));
			if (object) {
				int pin = m_screen->getPin(object, NORM(event->x()), NORM(event->y()));
				if (pin != -1) {
					if (m_fromObject == m_movingConn->to) {
						ConnectionNode *node = dynamic_cast<ConnectionNode *>(m_movingConn->to);
						if (node) {
							node->removeConnection(m_movingConn->tpin);
						}
						m_movingConn->tpin = pin;
						m_movingConn->to = object;
						node = dynamic_cast<ConnectionNode *>(m_movingConn->to);
						if (node) {
							node->setConnection(m_movingConn->tpin, m_movingConn);
						}
					}
					else {
						ConnectionNode *node = dynamic_cast<ConnectionNode *>(m_movingConn->from);
						if (node) {
							node->removeConnection(m_movingConn->fpin);
						}
						m_movingConn->fpin = pin;
						m_movingConn->from = object;
						node = dynamic_cast<ConnectionNode *>(m_movingConn->from);
						if (node) {
							node->setConnection(m_movingConn->fpin, m_movingConn);
						}
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
	return true;
}

bool ConnectionManager::mouseMoveEvent(QMouseEvent *event) {
	if (m_fromPin != -1) {
// 		qDebug() << NORM(event->x()) << NORM(event->y());
		m_screen->repaint();
		return true;
	}

	if (event->buttons() & Qt::LeftButton) {
		if (m_moving) {
			m_moving->setX(m_moving->x() - (m_movingX - NORM(event->x())));
			m_moving->setY(m_moving->y() - (m_movingY - NORM(event->y())));
			m_movingX = NORM(event->x());
			m_movingY = NORM(event->y());
			m_screen->repaint();
			return true;
		}
		else {
// 			int point;
// 			Connection *c = getPoint(NORM(event->x()), NORM(event->y()), point);
// 			if (point != -1) {
// 				m_moving = &c->points[point];
// 			}
// 			else {
// 				QPointF intersectPnt;
// 				int point;
// 				c = getConnection(NORM(event->x()), NORM(event->y()), point, &intersectPnt);
// 				if (c) {
// 					m_fromObject = 0;
// 					m_movingConn = c;
// 					QPoint p = intersectPnt.toPoint();
// 					c->points.insert(c->points.begin() + point, p);
// 					m_moving = &(c->points[point]);
// 				}
// 			}
		}

		m_movingX = NORM(event->x());
		m_movingY = NORM(event->y());
	}
	else {
		int point;
		Connection *c = getPoint(NORM(event->x()), NORM(event->y()), point);
		if (point != -1) {
			return true;
		}
		else {
			QPointF intersectPnt;
			int point;
			c = getConnection(NORM(event->x()), NORM(event->y()), point, &intersectPnt);
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
