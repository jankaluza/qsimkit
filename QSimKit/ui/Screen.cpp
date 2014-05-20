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
#include "Peripherals/SimulationObject.h"
#include "Peripherals/SimulationModel.h"
#include "Peripherals/PeripheralManager.h"
#include "Project/ProjectLoader.h"
#include "MCU/MCUManager.h"
#include "MCU/MCU.h"
#include "ConnectionNode.h"
#include "ScreenObject.h"
#include "ConnectionManager.h"
#include "AddPeripheral.h"


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
#include <QMessageBox>

#define NORM(X) ((X) - (X) % 12)

Screen::Screen(QWidget *parent) : QWidget(parent) {
	m_moving = 0;
	m_peripherals = 0;
	m_mcuManager = 0;
	m_conns = new ConnectionManager(this);
	setMouseTracking(true);
}

void Screen::prepareSimulation(SimulationModel *dig) {
	m_wrappers.clear();
	for (int i = 0; i < m_objects.size(); ++i) {
		Peripheral *p = dynamic_cast<Peripheral *>(m_objects[i]);
		if (p) {
			p->reset();
			
			SimulationObjectWrapper *wrapper = new SimulationObjectWrapper(p, m_trackedPins[m_objects[i]]);
			dig->add(wrapper);
			p->setWrapper(wrapper);

			m_wrappers[m_objects[i]] = wrapper;

			std::vector<SimulationObject *> internalObjects;
			p->getInternalSimulationObjects(internalObjects);
			for (int x = 0; x < internalObjects.size(); ++x) {
				SimulationObjectWrapper *wrapper = new SimulationObjectWrapper(internalObjects[x]);
				dig->add(wrapper);
			}
		}
	}

	m_conns->prepareSimulation(dig, m_wrappers);
}

void Screen::setSimulator(adevs::Simulator<SimulationEvent> *sim) {
	for (std::map<ScreenObject *, SimulationObjectWrapper *>::iterator it = m_wrappers.begin(); it != m_wrappers.end(); ++it) {
		it->second->setSimulator(sim);
	}
}

void Screen::setMCU(MCU *mcu) {
	if (m_objects.empty()) {
		m_objects.append(mcu);
	}
	else {
		onPeripheralRemoved(m_objects[0]);
		disconnect(m_objects[0], SIGNAL(onUpdated()), this, SLOT(update()));
		m_objects[0]->deleteLater();
		m_objects[0] = mcu;
	}

	onPeripheralAdded(m_objects[0]);
	connect(m_objects[0], SIGNAL(onUpdated()), this, SLOT(update()));
}

MCU *Screen::getMCU() {
	if (m_objects.empty()) {
		return 0;
	}

	return static_cast<MCU *>(m_objects[0]);
}

void Screen::paintEvent(QPaintEvent *e) {
	QPainter p;
	p.begin(this);
	p.fillRect(QRect(0, 0, width(), height()), QBrush(QColor(255, 255, 255)));
	p.setPen(QPen(QColor(245, 245, 245), 1, Qt::SolidLine));
	for (int i = e->rect().x(); i < e->rect().width(); i += 12) {
		p.drawLine(i, 0, i, height());
	}

	for (int i = e->rect().y(); i < e->rect().height(); i += 12) {
		p.drawLine(0, i, width(), i);
	}

	if (m_objects.empty()) {
		p.setPen(QPen(QColor(0, 0, 0), 1, Qt::SolidLine));
		p.drawText(0, 0, width(), height(), Qt::AlignCenter,
				   "There is no project loaded yet.\nCreate new project or open the existing one.");
	}

	p.end();

	for (int i = 0; i < m_objects.size(); ++i) {
		m_objects[i]->paint(this);
		m_objects[i]->m_updated = false;
	}

	p.begin(this);

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
	PinList &pins = object->getPins();
	for (PinList::const_iterator it = pins.begin(); it != pins.end(); ++it) {
		if (it->rect.contains(x,y)) {
			return it - pins.begin();
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
	onPeripheralRemoved(object);
	m_conns->objectRemoved(object);
	m_objects.removeAll(object);
	delete object;
    repaint();
}

void Screen::clear() {
	while (!m_objects.empty()) {
		removeObject(m_objects[0]);
	}
}

void Screen::save(QTextStream &stream) {
	stream << "<objects>\n";
	for (int i = 0; i < m_objects.size(); ++i) {
		stream << "<object id='" << i << "' type='" << m_objects[i]->type();
		stream << "' interface='" + m_objects[i]->interface() + "' name='" + m_objects[i]->name() + "'>\n";
		m_objects[i]->save(stream);
		stream << "</object>\n";
	}
	stream << "</objects>\n";

	stream << "<trackedpins>\n";
	for (std::map<ScreenObject *, QList<int> >::iterator it = m_trackedPins.begin(); it != m_trackedPins.end(); ++it) {
		foreach(int pin, it->second) {
			stream << "<object id='" << objectId(it->first) << "' pin='" << pin << "'/>\n";
		}
	}
	stream << "</trackedpins>\n";

	m_conns->save(stream);
}

void Screen::loadTrackedPins(QDomDocument &doc) {
	QDomElement root = doc.firstChild().toElement();
	QDomElement connections = root.firstChildElement("trackedpins");

	for(QDomNode node = connections.firstChild(); !node.isNull(); node = node.nextSibling()) {
		QDomElement c = node.toElement();

		ScreenObject *object = objectFromId(c.attribute("id").toInt());
		int pin = c.attribute("pin").toInt();

		m_trackedPins[object].append(pin);
		onPinTracked(object, pin);
	}
}

bool Screen::load(QDomDocument &doc) {
	QString error;

	ProjectLoader p(m_mcuManager, m_peripherals);
	if (!p.load(doc, error)) {
		QMessageBox::critical(this, tr("Loading error"), error);
		return false;
	}

	setMCU(p.getMCU());
	m_objects = p.getObjects();

	m_conns->load(doc);

	loadTrackedPins(doc);

	repaint();
	return true;
}

void Screen::showObjectMenu(ScreenObject *object, const QPoint &pos) {
	QList<QAction *> actions;
	int index = 0;
	foreach (const QString &str, object->getOptions()) {
		QString name = str;
		bool checkable = false;
		bool checked = false;
		if (str[0] == '+') {
			name = name.mid(1);
			checkable = true;
			checked = true;
		}
		else if (str[0] == '-') {
			name = name.mid(1);
			checkable = true;
		}

		QAction *action = new QAction(name, 0);
		action->setCheckable(checkable);
		action->setChecked(checked);
		action->setData(index++);
		actions.append(action);
	}

	if (object != m_objects[0]) {
		QAction *action = new QAction("Remove object", 0);
		action->setData(index);
		actions.append(action);
	}

	QAction *action = QMenu::exec(actions, pos, 0, 0);
	if (action && action->data() == index) {
		removeObject(object);
	}
	else if (action) {
		object->executeOption(action->data().toInt());
	}
}

void Screen::showPinMenu(ScreenObject *object, int pin, const QPoint &pos) {
	QList<QAction *> actions;

	QAction *action = new QAction("Track pin", 0);
	action->setCheckable(true);
	if (m_trackedPins[object].contains(pin)) {
		action->setChecked(true);
	}
	actions.append(action);

	action = QMenu::exec(actions, pos, 0, 0);
	if (action) {
		if (action->isChecked()) {
			m_trackedPins[object].append(pin);
			onPinTracked(object, pin);
		}
		else {
			m_trackedPins[object].removeAll(pin);
			onPinUntracked(object, pin);
		}
	}

}

void Screen::addObject(ScreenObject *obj) {
	m_objects.append(obj);
	onPeripheralAdded(obj);
}

void Screen::addObject(const QPoint &pos) {
	AddPeripheral dialog(m_peripherals, this);
	if (dialog.exec() == QDialog::Accepted) {
		QString name = dialog.getPeripheral();
		Peripheral *p = m_peripherals->getPeripheral(name).create();
		QPoint local = mapFromGlobal(pos);
		p->setX(NORM(local.x()));
		p->setY(NORM(local.y()));
		addObject(p);
	}
}

void Screen::showScreenMenu(const QPoint &pos) {
	QList<QAction *> actions;
	actions.append(new QAction("Add peripheral", 0));

	QAction *action = QMenu::exec(actions, pos, 0, 0);
	if (action) {
		addObject(pos);
	}
}

void Screen::mousePressEvent(QMouseEvent *event) {
	if (m_conns->mousePressEvent(event)) {
		repaint();
		return;
	}

	if (event->button() == Qt::RightButton) {
		ScreenObject *object = getObject(event->x(), event->y());
		if (object) {
			int pin = getPin(object, event->x(), event->y());
			if (pin == -1) {
				showObjectMenu(object, event->globalPos());
			}
			else {
				showPinMenu(object, pin, event->globalPos());
			}
			
		}
		else {
			showScreenMenu(event->globalPos());
		}
	}
	else if (event->button() == Qt::LeftButton) {
		ScreenObject *object = getObject(event->x(), event->y());
		if (!object) {
			return;
		}

		if (object->clicked(event->pos()) && m_wrappers[object]) {
			m_wrappers[object]->reschedule();
		}
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
