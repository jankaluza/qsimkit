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

#include "PythonPeripheral.h"
#include "Script/Script.h"
#include <QApplication>
#include <QDebug>

PythonPeripheral::PythonPeripheral(Script *script) : m_script(script), m_screenRegistered(false) {
	m_script->setVariable("x", m_x);
	m_script->setVariable("y", m_y);
	resize(m_script->getVariable("width").toInt(), m_script->getVariable("height").toInt());

	QVariantList pins = m_script->getVariable("pins").toList();
	for (int i = 0; i < pins.size(); ++i) {
		m_pins.push_back(Pin(pins[i].toRect(), "", 0));
	}

	pins = m_script->getVariable("pins_desc").toList();
	for (int i = 0; i < pins.size(); ++i) {
		m_pins[i].name = pins[i].toString();
	}

}

PythonPeripheral::~PythonPeripheral() {
	delete m_script;
}

void PythonPeripheral::internalTransition() {
	m_script->call("internalTransition");
}

void PythonPeripheral::externalEvent(double t, const SimulationEventList &events) {
	int i = 0;
	for (SimulationEventList::const_iterator it = events.begin(); i != events.size(); ++it, ++i) {
		m_script->call("externalEvent", QVariantList() << (*it).port << (*it).value);
	}
}

void PythonPeripheral::output(SimulationEventList &output) {
	QVariantList v = m_script->call("output").toList();
	while (!v.empty()) {
		output.insert(SimulationEvent(v[0].toInt(), v[1].toDouble()));
		v = m_script->call("output").toList();
	}
}

double PythonPeripheral::timeAdvance() {
	return m_script->call("timeAdvance").toDouble();
}

void PythonPeripheral::objectMoved(int x, int y) {
	m_script->setVariable("x", x);
	m_script->setVariable("y", y);

	QVariantList pins;
	for (PinList::iterator it = m_pins.begin(); it != m_pins.end(); it++) {
		pins << it->rect;
	}

	m_script->setVariable("pins", pins);
}

void PythonPeripheral::paint(QWidget *screen) {
	if (!m_screenRegistered) {
		m_screenRegistered = true;
		m_script->registerObject("screen", screen);
	}
	m_script->call("paint");
}

void PythonPeripheral::reset() {
	m_script->call("reset");
}

const QStringList &PythonPeripheral::getOptions() {
	m_options = m_script->getVariable("options").toStringList();
	return m_options;
}

void PythonPeripheral::executeOption(int option) {
	m_script->call("executeOption", QVariantList() << option);
}

bool PythonPeripheral::clicked(const QPoint &p) {
	m_script->call("clicked", QVariantList() << p);
	return m_script->getVariable("hasNewOutput").toBool();
}

void PythonPeripheral::save(QTextStream &stream) {
	ScreenObject::save(stream);
	QString str = m_script->call("save").toString();
	stream << str;
}

void PythonPeripheral::load(QDomElement &object, QString &error) {
	QString str;
	QTextStream stream(&str);
	object.save(stream, 0);
	m_script->call("load", QVariantList() << str);
}
