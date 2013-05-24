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
#include "CPU/Pins/PinManager.h"
#include "CPU/Interrupts/InterruptManager.h"
#include "CPU/BasicClock/BasicClock.h"
#include "CPU/BasicClock/MCLK.h"

#include "Package.h"
#include "SimulationObjects/Timer/AdevsTimerFactory.h"
#include "SimulationObjects/Timer/Timer.h"
#include "PeripheralItem/MSP430PeripheralItem.h"

#include <QWidget>
#include <QApplication>
#include <QString>
#include <QFile>
#include <QPen>
#include <QPainter>
#include <QDebug>
#include <QDomDocument>

MSP430::MSP430(Variant *variant) :
m_time(0), m_instructionCycles(0),
m_mem(0), m_reg(0), m_decoder(0), m_pinManager(0), m_intManager(0),
m_instruction(new MCU::Instruction), m_variant(variant),
m_timerFactory(new AdevsTimerFactory()), m_ignoreNextStep(false) {

	m_type = "MSP430";

	m_mem = new MCU::Memory(512000);
	m_reg = new MCU::RegisterSet();
	m_reg->addDefaultRegisters();

	m_intManager = new MCU::InterruptManager(m_reg, m_mem);
	m_pinManager = new MCU::PinManager(m_mem, m_intManager, m_variant);
	m_pinManager->setWatcher(this);
	Package::loadPackage(this, m_pinManager, "Packages/msp430x241x.xml", m_pins, m_sides);

	
	m_basicClock = new MCU::BasicClock(m_mem, m_variant, m_intManager, m_pinManager, m_timerFactory);
	reset();

	m_peripheralItem = new MSP430PeripheralItem(this);
}

void MSP430::loadPackage(const QString &file) {
	std::cout << "LOAD PACKAGE " << this << "\n";
	m_pinManager->reset();
	Package::loadPackage(this, m_pinManager, file, m_pins, m_sides);
}

void MSP430::reset() {
	delete m_decoder;

	// TODO; m_mem->reset(); m_reg->reset(); m_intManager->reset();

	m_basicClock->reset();

	m_decoder = new MCU::InstructionDecoder(m_reg, m_mem);

	if (!m_code.empty()) {
		loadA43(m_code);
	}
	
}

void MSP430::handlePinChanged(int id, double value) {
	m_pins[id].value = value;
	m_output.insert(SimulationEvent(id, value));
	onUpdated();
}


bool MSP430::loadA43(const std::string &data) {
	m_code = data;
	return m_mem->loadA43(data, m_reg);
}

void MSP430::getInternalSimulationObjects(std::vector<SimulationObject *> &objects) {
	objects.push_back(dynamic_cast<Timer *>(m_basicClock->getTimerA()));
}

void MSP430::externalEvent(double t, const SimulationEventList &events) {
	for (SimulationEventList::const_iterator it = events.begin(); it != events.end(); ++it) {
		if (!m_pinManager->handlePinInput((*it).port, (*it).value)) {
			qDebug() << "WARN: input on output PIN";
		}
		m_pins[(*it).port].value = (*it).value;
		onUpdated();
	}
}

void MSP430::output(SimulationEventList &output) {
	if (!m_output.empty()) {
		output = m_output;
		m_output.clear();
	}
}

void MSP430::internalTransition() {
	if (!m_ignoreNextStep) {
		if (m_intManager->runQueuedInterrupts()) {
			m_instructionCycles = 5;
		}
		else {
			m_instructionCycles = m_decoder->decodeCurrentInstruction(m_instruction);
			int cycles = executeInstruction(m_reg, m_mem, m_instruction);
			if (cycles == -1) {
				qDebug() << "ERROR: Unknown instruction" << "type" << m_instruction->type << "opcode" << m_instruction->opcode;
				m_instructionCycles = DBL_MAX;
				return;
			}

			m_intManager->handleInstruction(m_instruction);
			m_instructionCycles += cycles;
		}

		m_instructionCycles *= m_basicClock->getMCLK()->getStep();
	}
	else {
		m_ignoreNextStep = false;
	}
}

double MSP430::timeAdvance() {
	if (!m_output.empty()) {
// 		qDebug() << "MSP430 ta=" << 0;
		m_ignoreNextStep = true;
		return 0;
	}
// 	qDebug() << "MSP430 ta=" << m_instructionCycles;
	return m_instructionCycles;
}

void MSP430::executeOption(int option) {
	
}

void MSP430::save(QTextStream &stream) {
	ScreenObject::save(stream);
	stream << "<code>";
	stream << QString::fromStdString(m_code);
	stream << "</code>\n";
	stream << "<variant>";
	stream << QString(m_variant->getName());
	stream << "</variant>\n";
	stream << "<elf>";
	stream << m_elf.toBase64();
	stream << "</elf>";
}

void MSP430::load(QDomElement &object) {
	loadA43(object.firstChildElement("code").text().toStdString());
	setELF(QByteArray::fromBase64(object.firstChildElement("elf").text().toAscii()));
}

void MSP430::setPinType(const QString &n, MCU::PinType &type, int &subtype) {
#define SET_GP_PIN(PREFIX, T) { \
		if (n.startsWith(PREFIX)) { \
			type = T;\
			subtype = n.right(1).toInt();\
		} \
	}

	SET_GP_PIN("P1.", MCU::P1)
	SET_GP_PIN("P2.", MCU::P2)
	SET_GP_PIN("P3.", MCU::P3)
	SET_GP_PIN("P4.", MCU::P4)
	SET_GP_PIN("P5.", MCU::P5)
	SET_GP_PIN("P6.", MCU::P6)
	SET_GP_PIN("P7.", MCU::P7)
	SET_GP_PIN("P8.", MCU::P8)
}


void MSP430::paint(QWidget *screen) {
	QPainter qp(screen);
	int pin_size = 10;
	QPen pen(Qt::black, 2, Qt::SolidLine);
	qp.setPen(pen);
	qp.drawRect(m_x + pin_size + 1, m_y + pin_size, width() - 2*pin_size - 1, height() - 2*pin_size);
	qp.fillRect(QRect(m_x + pin_size + 1, m_y + pin_size, width() - 2*pin_size - 1, height() - 2*pin_size), QBrush(QColor(226, 206, 255)));

	QPen pen2(Qt::black, 1, Qt::SolidLine);
	qp.setPen(pen2);

	int even = -1;
	int id = 0;
	for (PinList::iterator it = m_pins.begin(); it != m_pins.end(); ++it, ++id) {
		if (it->value >= 0.5) {
			qp.fillRect(it->rect, QBrush(QColor(0,255,0)));
		}
		qp.drawRect(it->rect);
		if (m_sides[id] == 'l') {
			qp.drawText(it->rect.adjusted(pin_size - 3, 0 , 2*pin_size, 0 + 2), Qt::AlignCenter, QString::number(id + 1));
		}
		else if (m_sides[id] == 'r') {
			qp.drawText(it->rect.adjusted(-pin_size - 5, 0, -pin_size, 0 + 2), Qt::AlignCenter, QString::number(id + 1));
			even = -1;
		}
		else if (m_sides[id] == 'd') {
			if (even == -1) {
				even = 0;
			}
			if (even) {
				if (m_sides[id + 1] != 'r') {
					qp.drawText(it->rect.adjusted(0, -2 * pin_size - 13, 5, -pin_size), Qt::AlignCenter, QString::number(id + 1));
				}
			}
			else {
				qp.drawText(it->rect.adjusted(0, -pin_size - 13, 5, 0), Qt::AlignCenter, QString::number(id + 1));
			}
			even = not even;
		}
		else if (m_sides[id] == 'u') {
			if (even == -1) {
				even = 0;
			}
			if (even) {
				if (id + 1 != m_pins.size()) {
					qp.drawText(it->rect.adjusted(0, 2*pin_size + 18, 5, pin_size), Qt::AlignCenter, QString::number(id + 1));
				}
			}
			else {
				qp.drawText(it->rect.adjusted(0, pin_size + 18, 5, 0), Qt::AlignCenter, QString::number(id + 1));
			}
			even = not even;
		}
	}
}
