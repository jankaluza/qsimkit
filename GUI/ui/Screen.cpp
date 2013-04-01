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

#include "Peripherals/Peripheral.h"
#include "Peripherals/MSP430/MSP430.h"
// #include "Peripherals/LED/LED.h"
#include "ConnectionNode.h"
#include "ScreenObject.h"
#include "ConnectionManager.h"


#include <QWidget>
#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QToolTip>
#include <QString>
#include <QApplication>
#include <QFileDialog>
#include <QInputDialog>
#include <QFile>
#include <QCursor>
#include <QIODevice>
#include <QMouseEvent>
#include <QDebug>

#define NORM(X) ((X) - (X) % 12)

Screen::Screen(QWidget *parent) : QWidget(parent) {
	m_moving = 0;
	m_conns = new ConnectionManager(this);
	setMouseTracking(true);
}

void Screen::prepareSimulation(adevs::Digraph<SimulationEvent *> *dig) {
	std::map<ScreenObject *, SimulationObjectWrapper *> wrappers;
	for (int i = 0; i < m_objects.size(); ++i) {
		Peripheral *p = dynamic_cast<Peripheral *>(m_objects[i]);
		if (p) {
			SimulationObjectWrapper *wrapper = new SimulationObjectWrapper(p);
			dig->add(wrapper);

			wrappers[m_objects[i]] = wrapper;
		}
	}

	m_conns->prepareSimulation(dig, wrappers);
}

void Screen::setCPU(MSP430 *cpu) {
	if (m_objects.empty()) {
		m_objects.append(cpu);
// 		m_objects.append(new LED());
// 		m_objects.append(new ConnectionNode());
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
	p.fillRect(QRect(0, 0, width(), height()), QBrush(QColor(255, 255, 255)));
	p.setPen(QPen(QColor(245, 245, 245), 1, Qt::SolidLine));
	for (int i = 0; i < 1500; i += 12) {
		p.drawLine(i, 0, i, 1500);
	}

	for (int i = 0; i < 1500; i += 12) {
		p.drawLine(0, i, 1500, i);
	}


	for (int i = 0; i < m_objects.size(); ++i) {
		m_objects[i]->paint(p);
	}

	m_conns->paint(p);
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
		if (it->second.rect.contains(x,y)) {
			return it->first;
		}
	}

	return -1;
}

void Screen::mouseReleaseEvent(QMouseEvent *event) {
	if (m_conns->mouseReleaseEvent(event)) {
		repaint();
		return;
	}
}

void Screen::removeObject(ScreenObject *object) {
	m_conns->objectRemoved(object);
	m_objects.removeAll(object);
	qDebug() << "removing";
	qDebug() << "removing" << object;
	delete object;
}

void Screen::mousePressEvent(QMouseEvent *event) {
	if (m_conns->mousePressEvent(event)) {
		repaint();
		return;
	}

	if (event->button() == Qt::RightButton) {
		ScreenObject *object = getObject(event->x(), event->y());
		if (!object) {
			return;
		}

		QList<QAction *> actions;
		actions.append(new QAction("Remove object", 0));
		QAction *action = QMenu::exec(actions, event->globalPos(), 0, 0);
		if (action) {
			removeObject(object);
		}
	}
	else if (event->button() == Qt::LeftButton) {

	}
}

void Screen::mouseMoveEvent(QMouseEvent *event) {
	if (!m_moving && m_conns->mouseMoveEvent(event)) {
		ScreenObject *object = getObject(event->x(), event->y());
		if (!object) {
			return;
		}

		int pin = getPin(object, event->x(), event->y());
		if (pin == -1) {
			return;
		}

		Pin &p = object->getPins()[pin];
		QToolTip::showText(mapToGlobal(event->pos()), p.name);
		return;
	}

	if (event->buttons() & Qt::LeftButton) {
		if (m_moving) {
			m_moving->setX(NORM(m_moving->x() - (m_movingX - event->x())));
			m_moving->setY(NORM(m_moving->y() - (m_movingY - event->y())));
			m_conns->movePins(m_moving);
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
		m_movingX = NORM(event->x());
		m_movingY = NORM(event->y());
	}
	else {
		if (m_moving) {
			m_conns->objectMoved(m_moving);
		}
		m_moving = 0;

		ScreenObject *object = getObject(event->x(), event->y());
		if (!object) {
			return;
		}

		int pin = getPin(object, event->x(), event->y());
		if (pin == -1) {
			return;
		}

		Pin &p = object->getPins()[pin];
		QToolTip::showText(mapToGlobal(event->pos()), p.name);
	}
}
