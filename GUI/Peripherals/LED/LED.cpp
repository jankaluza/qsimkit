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

#include "LED.h"

#include <QWidget>
#include <QApplication>
#include <QString>
#include <QFile>
#include <QPen>
#include <QPainter>
#include <QDebug>
#include <QDomDocument>
#include <QtCore/qplugin.h>

LED::LED() : m_state(0) {
	resize(30, 36);

	Pin p;
	p.rect = QRect(12, 24, 12, 12);
	p.name = "LED";
	p.high = 0;

	m_pins[0] = p;
}

void LED::reset() {

}

void LED::internalTransition() {

}

void LED::externalEvent(const std::vector<SimulationEvent *> &events) {
	for (std::vector<SimulationEvent *>::const_iterator it = events.begin(); it != events.end(); ++it) {
		m_state = (*it)->high;
	}
}

void LED::output(std::vector<SimulationEvent *> &output) {

}

double LED::timeAdvance() {
	return DBL_MAX;
}

void LED::paint(QPainter &qp) {
	QPen pen(Qt::black, 2, Qt::SolidLine);
	qp.setPen(pen);
	qp.drawEllipse(m_x + 6, m_y, m_width - 6, m_height - 12);

	int even = -1;
	for (std::map<int, Pin>::iterator it = m_pins.begin(); it != m_pins.end(); it++) {
		if (m_state) {
			qp.fillRect(it->second.rect, QBrush(QColor(0,255,0)));
		}
		qp.drawRect(it->second.rect);
	}
}

Peripheral *LEDInterface::create() {
	return new LED();
}

Q_EXPORT_PLUGIN2(ledperipheral, LEDInterface);
