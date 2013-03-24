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

MSP430::MSP430(Variant *variant) : m_cycles(0), m_instructionCycles(0),
m_mem(new Memory(512000)), m_reg(new RegisterSet()),
m_decoder(new InstructionDecoder(m_reg, m_mem)),
m_instruction(new Instruction), m_variant(variant) {

	m_reg->addDefaultRegisters();

	addMemoryWatchers();
}

void MSP430::addMemoryWatchers() {
#define ADD_WATCHER(METHOD) \
	if (METHOD != 0) m_mem->addWatcher(METHOD, this);

	ADD_WATCHER(m_variant->getP1DIR());
	ADD_WATCHER(m_variant->getP1OUT());

#undef ADD_WATCHER
}

bool MSP430::loadA43(const std::string &data) {
	return m_mem->loadA43(data, m_reg);
}

#define REFRESH_GP(PREFIX, VAR, ADDRESS) { \
	uint16_t b = m_mem->getBigEndian(ADDRESS);\
	for (int i = 0; i < 8; i++) { \
		QString p2 = QString(PREFIX) + "." + QString::number(i); \
		if (b & (1 << i)) { \
			m_states[m_map[p2]].high = true; \
		} \
		else { \
			m_states[m_map[p2]].high = false; \
		} \
	} \
}

void MSP430::handleMemoryChanged(Memory *memory, uint16_t address) {
	if (address == m_variant->getP1OUT()) {
		REFRESH_GP("P1", in, m_variant->getP1OUT());
	}

	onUpdated();
}

void MSP430::internalTransition() {
	qDebug() << m_cycles << "next simulation step";
	m_instructionCycles = m_decoder->decodeCurrentInstruction(m_instruction);
	m_instructionCycles += executeInstruction(m_reg, m_mem, m_instruction);

	m_cycles += m_instructionCycles;
}

double MSP430::timeAdvance() {
	return m_instructionCycles;
}

bool MSP430::loadXML(const QString &file) {
	int font_w = QApplication::fontMetrics().width("99") + 2;
	int font_h = QApplication::fontMetrics().height() + 2;
	int width = font_w * 2;
	int height = font_w * 2;
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
			y = font_w;
		}
		else if (side == 'r') {
			x = width - font_w;
			y = height - 2 * font_w;
		}
		else if (side == 'd') {
			x = font_w;
			y = height - font_w;
		}
		else if (side == 'u') {
			x = width - 2 * font_w;
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
			qDebug() << n;
			m_names[id] = n;
			m_pins[id] = QRect(x, y, font_w, font_h);

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

void MSP430::paint(QPainter &qp) {
	int font_w = QApplication::fontMetrics().width("99");
	int font_h = QApplication::fontMetrics().height();

	QPen pen(Qt::black, 2, Qt::SolidLine);
	qp.setPen(pen);
	qp.drawRect(m_x,m_y, width() - 2, height() - 2);

	QPen pen2(Qt::black, 1, Qt::SolidLine);
	qp.setPen(pen2);

	for (std::map<int, QRect>::iterator it = m_pins.begin(); it != m_pins.end(); it++) {
		if (m_states[it->first].high) {
			qp.fillRect(it->second.adjusted(m_x, m_y, m_x, m_y), QBrush(QColor(0,255,0)));
		}
		qp.drawRect(it->second.adjusted(m_x, m_y, m_x, m_y));
		qp.drawText(it->second.adjusted(m_x, m_y, m_x, m_y), Qt::AlignCenter, QString::number(it->first));
		if (m_sides[it->first] == 'l') {
			qp.drawText(QRect(m_x + it->second.x() + it->second.width() + 4, m_y + it->second.y(), 100, font_h), Qt::AlignCenter, m_names[it->first]);
		}
		
	}
}
