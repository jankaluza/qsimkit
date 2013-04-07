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

#include "ConnectionNode.h"
#include <QPainter>
#include <QPaintDevice>
#include <QWidget>

ConnectionNode::ConnectionNode() {
	m_type = "ConnectionNode";
	m_advance = 365;
	m_width = 36;
	m_height = 36;

	m_pins[0].rect = QRect(0, 12, 12, 12);
	m_pins[0].name = "West pin";
	m_pins[0].high = 0;

	m_pins[1].rect = QRect(12, 24, 12, 12);
	m_pins[1].name = "South pin";
	m_pins[1].high = 0;

	m_pins[2].rect = QRect(24, 12, 12, 12);
	m_pins[2].name = "East pin";
	m_pins[2].high = 0;

	m_pins[3].rect = QRect(12, 0, 12, 12);
	m_pins[3].name = "North pin";
	m_pins[3].high = 0;
}

ConnectionNode::~ConnectionNode() {

}

void ConnectionNode::internalTransition() {}

void ConnectionNode::externalEvent(double t, const SimulationEventList &events) {
	for (SimulationEventList::const_iterator it = events.begin(); it != events.end(); ++it) {
		for (std::map<int, Connection *>::iterator it2 = m_conns.begin(); it2 != m_conns.end(); ++it2) {
			m_output.insert(SimulationEvent(it2->first, (*it).value));
		}
	}
	m_advance = 0;
}

void ConnectionNode::output(SimulationEventList &output) {
	if (!m_output.empty()) {
// 		qDebug() << "node output";
		output = m_output;
		m_output.clear();
	}
}

double ConnectionNode::timeAdvance() {
	double r = m_advance;
	m_advance = 365;
// 	qDebug() << "node ta=" << r;
	return r;
}


void ConnectionNode::reset() {}

// void ConnectionNode::getAllConnectedObjects(std::vector<ScreenObject *> &objects) {
// 	for (std::map<int, Connection *>::iterator it = m_conns.begin(); it != m_conns.end; ++it) {
// 		Connection *c = it->second;
// 		ConnectionNode *node = dynamic_cast<ConnectionNode *>(c->from);
// 		if (node) {
// 			node->getAllConnectedObject(c->from);
// 		}
// 		else {
// 			objects.push_back(c->from);
// 		}
// 
// 		node = dynamic_cast<ConnectionNode *>(c->to);
// 		if (node) {
// 			node->getAllConnectedObject(c->to);
// 		}
// 		else {
// 			objects.push_back(c->to);
// 		}
// 	}
// }

void ConnectionNode::paint(QWidget *screen) {
	QPainter p(screen);
	QBrush b = p.brush();
	QPen pen = p.pen();
// 	p.drawRect(m_x, m_y, m_width, m_height);
	p.setBrush(QBrush(QColor(0,0,0)));
	p.drawEllipse(m_x+14, m_y+14, 6, 6);
	p.setBrush(b);

 	p.setPen(QPen(QColor(194, 194, 194), 1, Qt::SolidLine));
	for (std::map<int, Pin>::iterator it = m_pins.begin(); it != m_pins.end(); it++) {
		p.drawRect(it->second.rect);
	}

	p.setPen(QPen(Qt::black, 2, Qt::SolidLine));
	p.drawLine(m_pins[0].rect.center(), m_pins[2].rect.center());
	p.drawLine(m_pins[1].rect.center(), m_pins[3].rect.center());
}
