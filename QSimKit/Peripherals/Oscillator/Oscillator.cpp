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

#include "Oscillator.h"

#include <QWidget>
#include <QApplication>
#include <QString>
#include <QFile>
#include <QPen>
#include <QPainter>
#include <QDebug>
#include <QDomDocument>
#include <QInputDialog>
#include <QtCore/qplugin.h>

Oscillator::Oscillator() : m_state(false) {
	resize(36, 24);

	m_pins.push_back(Pin(QRect(24, 0, 10, 10), "XIN", 0));
	m_pins.push_back(Pin(QRect(24, 12, 10, 10), "XOUT", 0));

	m_freq = 7372800;
	m_step = 1.0 / m_freq / 2;

	m_options << "Set frequency";
}

void Oscillator::reset() {

}

const QStringList &Oscillator::getOptions() {
	return m_options;
}

void Oscillator::executeOption(int option) {
	m_freq = QInputDialog::getInt(0, "Set frequency", "Frequency (Hz):", m_freq);
	m_step = 1.0 / m_freq / 2;
}

void Oscillator::save(QTextStream &stream) {
	ScreenObject::save(stream);
	stream << "<frequency>";
	stream << m_freq;
	stream << "</frequency>\n";
}

void Oscillator::load(QDomElement &object, QString &error) {
	m_freq = object.firstChildElement("frequency").text().toInt();
	m_step = 1.0 / m_freq / 2;
}

void Oscillator::internalTransition() {
	if (!m_state) {
		m_state = true;
		m_output.insert(SimulationEvent(0, 3.0));
		m_output.insert(SimulationEvent(1, 3.0));
	}
	else {
		m_state = false;
		m_output.insert(SimulationEvent(0, 0.0));
		m_output.insert(SimulationEvent(1, 0.0));
	}
}

void Oscillator::externalEvent(double e, const SimulationEventList &events) {

}

void Oscillator::output(SimulationEventList &output) {
	if (!m_output.empty()) {
		output.swap(m_output);
	}
}

double Oscillator::timeAdvance() {
	return m_step;
}

void Oscillator::paint(QWidget *screen) {
	QPainter qp(screen);
	// Draw crystal oscillator sign -[]-
	qp.drawRect(m_x, m_y, m_width - 12, m_height);
	qp.drawLine(m_x + 3 + 9, m_y + 7, m_x + 3 + 9, m_y + 2);
	qp.drawLine(m_x + 3, m_y + 7, m_x + 3 + 18, m_y + 7);
	qp.drawRect(m_x + 3, m_y + 9, 18, 6);
	qp.drawLine(m_x + 3, m_y + 17, m_x + 3 + 18, m_y + 17);
	qp.drawLine(m_x + 3 + 9, m_y + 17, m_x + 3 + 9, m_y + 22);

	for (PinList::iterator it = m_pins.begin(); it != m_pins.end(); it++) {
		if (m_state) {
			qp.fillRect(it->rect, QBrush(QColor(0,255,0)));
		}
		qp.drawRect(it->rect);
	}
}

Peripheral *OscillatorInterface::create() {
	return new Oscillator();
}

Q_EXPORT_PLUGIN2(oscillatorperipheral, OscillatorInterface);
