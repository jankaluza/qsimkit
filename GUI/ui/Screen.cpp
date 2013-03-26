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

#include "Screen.h"

#include "Peripherals/MSP430/MSP430.h"
#include "Peripherals/LED/LED.h"
#include "ScreenObject.h"
#include "ConnectionManager.h"


#include <QWidget>
#include <QMainWindow>
#include <QToolTip>
#include <QString>
#include <QFileDialog>
#include <QInputDialog>
#include <QFile>
#include <QCursor>
#include <QIODevice>
#include <QMouseEvent>
#include <QDebug>

Screen::Screen(QWidget *parent) : QWidget(parent) {
	m_moving = 0;
	m_fromPin = -1;
	m_conns = new ConnectionManager();
	setMouseTracking(true);
}

void Screen::prepareSimulation(adevs::Digraph<SimulationEvent *> *dig) {
	std::map<ScreenObject *, SimulationObjectWrapper *> wrappers;
	for (int i = 0; i < m_objects.size(); ++i) {
		SimulationObjectWrapper *wrapper = new SimulationObjectWrapper(m_objects[i]);
		dig->add(wrapper);

		wrappers[m_objects[i]] = wrapper;
	}

	m_conns->prepareSimulation(dig, wrappers);
}

void Screen::setCPU(MSP430 *cpu) {
	if (m_objects.empty()) {
		m_objects.append(cpu);
		m_objects.append(new LED());
	}
	else {
		disconnect(m_objects[0], SIGNAL(onUpdated()), this, SLOT(update()));
		m_objects[0]->deleteLater();
		m_objects[0] = cpu;
	}

	connect(m_objects[0], SIGNAL(onUpdated()), this, SLOT(update()));
}

MSP430 *Screen::getCPU() {
	return static_cast<MSP430 *>(m_objects[0]);
}

void Screen::paintEvent(QPaintEvent *e) {
	QPainter p(this);
	for (int i = 0; i < m_objects.size(); ++i) {
		m_objects[i]->paint(p);
	}

	m_conns->paint(p);

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


		to = mapFromGlobal(QCursor::pos());

		if (getObject(from.x(), to.y()) == m_moving && getPin(m_moving, from.x(), to.y()) != -1) {
			p.drawLine(from, QPoint(to.x(), from.y()));
			p.drawLine(QPoint(to.x(), from.y()), to);
		}
		else {
			p.drawLine(from, QPoint(from.x(), to.y()));
			p.drawLine(QPoint(from.x(), to.y()), to);
		}
	}
}

void Screen::resizeAccordingToObjects() {
	int maxX = 0;
	int maxY = 0;

	for (int i = 0; i < m_objects.size(); ++i) {
		int x = m_objects[i]->x() + m_objects[i]->width();
		int y = m_objects[i]->y() + m_objects[i]->height();
		if (x > maxX) {
			maxX = x;
		}
		if (y > maxY) {
			maxY = y;
		}
	}

	setMinimumSize(maxX, maxY);
}

ScreenObject *Screen::getObject(int x, int y) {
	for (int i = 0; i < m_objects.size(); ++i) {
		if (x > m_objects[i]->x() && x < (m_objects[i]->width() + m_objects[i]->x()) &&
			y > m_objects[i]->y() && y < (m_objects[i]->height() + m_objects[i]->y())) {
			return m_objects[i];
		}
	}
	return 0;
}

int Screen::getPin(ScreenObject *object, int x, int y) {
	std::map<int, Pin> &pins = object->getPins();
	for (std::map<int, Pin>::const_iterator it = pins.begin(); it != pins.end(); ++it) {
// 		qDebug() << it->second.rect.adjusted(object->x(),object->y(),object->x(),object->y()) << x << y;
		if (it->second.rect.adjusted(object->x(),object->y(),object->x(),object->y()).contains(x,y)) {
			return it->first;
		}
	}

	return -1;
}

void Screen::mousePressEvent(QMouseEvent *event) {
	if (event->button() == Qt::RightButton) {
		if (m_fromPin != -1) {
			m_fromPin = -1;
			m_points.clear();
			repaint();
		}
	}
	else if (event->button() == Qt::LeftButton) {
		if (m_fromPin != -1) {
			QPoint from = QPoint(m_movingX, m_movingY);
			QPoint to = mapFromGlobal(QCursor::pos());
			ScreenObject *object = getObject(event->x(), event->y());
			if (object) {
				int pin = getPin(object, event->x(), event->y());
				if (pin != -1) {
					to = object->getPins()[pin].rect.adjusted(object->x(), object->y(), object->x(), object->y()).center();
				}
			}
			m_points.push_back(from);

			if (getObject(from.x(), to.y()) == m_moving && getPin(m_moving, from.x(), to.y()) != -1) {
				m_points.push_back(QPoint(to.x(), from.y()));
			}
			else {
				m_points.push_back(QPoint(from.x(), to.y()));
			}

			m_points.push_back(to);

			if (object) {
				int pin = getPin(object, event->x(), event->y());
				if (pin != -1) {
					m_conns->addConnection(m_moving, m_fromPin, object, pin, m_points);
					m_fromPin = -1;
					m_points.clear();
					repaint();
					return;
				}
			}

			m_movingX = event->x();
			m_movingY = event->y();
			return;
		}

		ScreenObject *object = getObject(event->x(), event->y());
		if (!object) {
			return;
		}

		m_fromPin = getPin(object, event->x(), event->y());
		if (m_fromPin != -1) {
			m_moving = object;
			m_movingX = object->getPins()[m_fromPin].rect.adjusted(object->x(), object->y(), object->x(), object->y()).center().x();
			m_movingY = object->getPins()[m_fromPin].rect.adjusted(object->x(), object->y(), object->x(), object->y()).center().y();
		}
	}
}

void Screen::mouseMoveEvent(QMouseEvent *event) {
	if (m_conns->mouseMoveEvent(event)) {
		repaint();
		return;
	}

	if (event->buttons() & Qt::LeftButton) {
		if (m_fromPin != -1) {
			repaint();
			return;
		}
		else if (m_moving) {
			m_moving->setX(m_moving->x() - (m_movingX - event->x()));
			m_moving->setY(m_moving->y() - (m_movingY - event->y()));
			resizeAccordingToObjects();
			repaint();
		}
		else {
			ScreenObject *object = getObject(event->x(), event->y());
			if (!object) {
				return;
			}

			int pin = getPin(object, event->x(), event->y());
			if (pin != -1) {
				return;
			}
			m_moving = object;
		}
		m_movingX = event->x();
		m_movingY = event->y();
	}
	else {
		if (m_fromPin != -1) {
			repaint();
		}
		else {
			m_moving = 0;
		}

		ScreenObject *object = getObject(event->x(), event->y());
		if (!object) {
			return;
		}

		int pin = getPin(object, event->x(), event->y());
		if (pin == -1) {
			return;
		}

		Pin &p = object->getPins()[pin];
		QToolTip::showText(event->pos(), p.name);
	}
}
