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

PythonPeripheral::PythonPeripheral(Script *script) : m_script(script) {
	m_script->setVariable("x", m_x);
	m_script->setVariable("y", m_y);
	resize(m_script->getVariable("width").toInt(), m_script->getVariable("height").toInt());

	QVariantList pins = m_script->getVariable("pins").toList();
	for (int i = 0; i < pins.size(); ++i) {
		m_pins[i].rect = pins[i].toRect();
	}
}

PythonPeripheral::~PythonPeripheral() {
	delete m_script;
}

void PythonPeripheral::internalTransition() {
	m_script->call("internalTransition");
}

void PythonPeripheral::externalEvent(const std::vector<SimulationEvent *> &events) {
	for (std::vector<SimulationEvent *>::const_iterator it = events.begin(); it != events.end(); ++it) {
		m_script->call("externalEvent", QVariantList() << (*it)->port << (*it)->high);
	}
}

void PythonPeripheral::output(std::vector<SimulationEvent *> &output) {
	QVariantList v = m_script->call("output").toList();
	while (!v.empty()) {
		output.push_back(new SimulationEvent(v[0].toInt(), v[1].toBool()));
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
	for (std::map<int, Pin>::iterator it = m_pins.begin(); it != m_pins.end(); it++) {
		pins << it->second.rect;
	}

	m_script->setVariable("pins", pins);
}

void PythonPeripheral::paint(QWidget *screen) {
	m_script->call("paint");
}

void PythonPeripheral::reset() {
	m_script->call("reset");
}
