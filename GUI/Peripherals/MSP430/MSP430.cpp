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

#include "MSP430.h"

#include "CPU/Memory/Memory.h"
#include "CPU/Memory/RegisterSet.h"
#include "CPU/Memory/Register.h"
#include "CPU/Instructions/Instruction.h"
#include "CPU/Instructions/InstructionDecoder.h"
#include "CPU/Instructions/InstructionManager.h"
#include "CPU/Variants/Variant.h"

#include <QWidget>
#include <QApplication>
#include <QString>
#include <QFile>
#include <QPen>
#include <QPainter>
#include <QDebug>
#include <QDomDocument>

MSP430::MSP430(Variant *variant, unsigned long frequency) :
m_time(0), m_instructionCycles(0),
m_mem(0), m_reg(0), m_decoder(0),
m_instruction(new Instruction), m_variant(variant) {

	m_step = 1.0/frequency;
	reset();
}

void MSP430::reset() {
	delete m_mem;
	delete m_reg;
	delete m_decoder;

	m_mem = new Memory(512000);
	m_reg = new RegisterSet();
	m_decoder = new InstructionDecoder(m_reg, m_mem);


	m_reg->addDefaultRegisters();

	addMemoryWatchers();

	if (!m_code.empty()) {
		loadA43(m_code);
	}
	
}

void MSP430::addMemoryWatchers() {
#define ADD_WATCHER(METHOD) \
	if (METHOD != 0) m_mem->addWatcher(METHOD, this);

	ADD_WATCHER(m_variant->getP1DIR());
	ADD_WATCHER(m_variant->getP1OUT());

#undef ADD_WATCHER
}

bool MSP430::loadA43(const std::string &data) {
	m_code = data;
	return m_mem->loadA43(data, m_reg);
}

#define REFRESH_GP(PREFIX, VAR, ADDRESS) { \
	uint16_t b = m_mem->getBigEndian(ADDRESS);\
	for (int i = 0; i < 8; i++) { \
		QString p2 = QString(PREFIX) + "." + QString::number(i); \
		if (b & (1 << i)) { \
			m_states[m_map[p2]].high = true; \
			m_output.push_back(new SimulationEvent(m_map[p2], true)); \
		} \
		else { \
			m_states[m_map[p2]].high = false; \
			m_output.push_back(new SimulationEvent(m_map[p2], false)); \
		} \
	} \
}

void MSP430::handleMemoryChanged(Memory *memory, uint16_t address) {
	if (address == m_variant->getP1OUT()) {
		REFRESH_GP("P1", in, m_variant->getP1OUT());
	}

	onUpdated();
}

void MSP430::externalEvent(const std::vector<SimulationEvent *> &) {

}

void MSP430::output(std::vector<SimulationEvent *> &output) {
	output.swap(m_output);
}

void MSP430::internalTransition() {
	m_instructionCycles = m_decoder->decodeCurrentInstruction(m_instruction);
	m_instructionCycles += executeInstruction(m_reg, m_mem, m_instruction);
	m_instructionCycles *= m_step;

	m_time += m_instructionCycles;
}

double MSP430::timeAdvance() {
	return m_instructionCycles;
}

bool MSP430::loadXML(const QString &file) {
	int font_w = 10;
	int font_h = 10;
	int width = 48;
	int height = 48;
    int errorLine, errorColumn;
    QString errorMsg;

	QFile modelFile(file);
	QDomDocument document;
	if (!document.setContent(&modelFile, &errorMsg, &errorLine, &errorColumn))
	{
			QString error("Syntax error line %1, column %2:\n%3");
			error = error
					.arg(errorLine)
					.arg(errorColumn)
					.arg(errorMsg);
			qDebug() << error;
			return false;
	}

	QDomElement rootElement = document.firstChild().toElement();
	QDomNode package = rootElement.firstChild();
	if (package.nodeName() != "package") {
		return false;
	}

	for(QDomNode node = package.firstChild(); !node.isNull(); node = node.nextSibling()) {
		QDomElement element = node.toElement();
		QChar side = element.nodeName()[0];
		for(QDomNode pin = element.firstChild(); !pin.isNull(); pin = pin.nextSibling()) {
			if (side == 'd') {
				width += font_w + 2;
			}
			else if (side == 'l') {
				height += font_w + 2;
			}
		}
	}

	for(QDomNode node = package.firstChild(); !node.isNull(); node = node.nextSibling()) {
		QDomElement element = node.toElement();
		QChar side = element.nodeName()[0];

		int x = 0;
		int y = 0;
		if (side == 'l') {
			x = 0;
			y = 25;
		}
		else if (side == 'r') {
			x = width - font_w;
			y = height - font_w - 24;
		}
		else if (side == 'd') {
			x = 2*font_w;
			y = height - font_w;
		}
		else if (side == 'u') {
			x = width - font_w - 24;
			y = 0;
		}

		for(QDomNode pin = element.firstChild(); !pin.isNull(); pin = pin.nextSibling()) {
			int id = pin.toElement().attribute("id").toInt();
			m_sides[id] = side;
			QString n;
			for(QDomNode name = pin.firstChild(); !name.isNull(); name = name.nextSibling()) {
				n += name.toElement().text() + "/";
				m_map[name.toElement().text()] = id;
			}
// 			qDebug() << n;
			m_names[id] = n;
			m_pins[id].rect = QRect(x, y, font_w, font_h);
			m_pins[id].high = false;
			m_pins[id].name = n;
			

			PinState p;
			p.high = false;
			p.in = false;
			m_states[id] = p;

			if (side == 'd') {
				x += font_w + 2;
			}
			else if (side == 'l') {
				y += font_w + 2;
			}
			else if (side == 'r') {
				y -= font_w + 2;
			}
			else if (side == 'u') {
				x -= font_w + 2;
			}
		}
	}

	resize(width, height);
	qDebug() << "loaded " << width << height;
	return true;
}

void MSP430::paint(QWidget *screen) {
	QPainter qp(screen);
	int font_w = 10;
	int font_h = 10;

	QPen pen(Qt::black, 2, Qt::SolidLine);
	qp.setPen(pen);
	qp.drawRect(m_x + font_w + 1, m_y + font_w, width() - 2*font_w - 1, height() - 2*font_w);
	qp.fillRect(QRect(m_x + font_w + 1, m_y + font_w, width() - 2*font_w - 1, height() - 2*font_w), QBrush(QColor(226, 206, 255)));

	QPen pen2(Qt::black, 1, Qt::SolidLine);
	qp.setPen(pen2);

	int even = -1;
	for (std::map<int, Pin>::iterator it = m_pins.begin(); it != m_pins.end(); it++) {
// 		qDebug() << it->second.rect;
		if (m_states[it->first].high) {
			qp.fillRect(it->second.rect, QBrush(QColor(0,255,0)));
		}
		qp.drawRect(it->second.rect);
		if (m_sides[it->first] == 'l') {
			qp.drawText(it->second.rect.adjusted(0 + font_w - 3, 0 , 0 + 2*font_w, 0 + 2), Qt::AlignCenter, QString::number(it->first));
		}
		else if (m_sides[it->first] == 'r') {
			qp.drawText(it->second.rect.adjusted(0 - font_w - 5, 0, 0 - font_w, 0 + 2), Qt::AlignCenter, QString::number(it->first));
			even = -1;
		}
		else if (m_sides[it->first] == 'd') {
			if (even == -1) {
				even = 0;
			}
			if (even) {
				std::map<int, Pin>::iterator next = it;
				next++;
				if (m_sides[next->first] != 'r') {
					qp.drawText(it->second.rect.adjusted(0, 0 - font_h - 13 - font_h, 0 + 5, 0 - font_h), Qt::AlignCenter, QString::number(it->first));
				}
			}
			else {
				qp.drawText(it->second.rect.adjusted(0, 0 - font_h - 13, 0 + 5, 0), Qt::AlignCenter, QString::number(it->first));
			}
			even = not even;
		}
		else if (m_sides[it->first] == 'u') {
			if (even == -1) {
				even = 0;
			}
			if (even) {
				std::map<int, Pin>::iterator next = it;
				next++;
				if (next != m_pins.end()) {
					qp.drawText(it->second.rect.adjusted(0, 0 + font_h + 18 + font_h, 0 + 5, 0 + font_h), Qt::AlignCenter, QString::number(it->first));
				}
			}
			else {
				qp.drawText(it->second.rect.adjusted(0, 0 + font_h + 18, 0 + 5, 0), Qt::AlignCenter, QString::number(it->first));
			}
			even = not even;
		}
// 		}
// 		if (m_sides[it->first] == 'l') {
// 			qp.drawText(QRect(0 + it->second.x() + it->second.width() + 4, 0 + it->second.y(), 100, font_h), Qt::AlignCenter, m_names[it->first]);
// 		}
		
	}
}
