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
#include "CPU/Variants/VariantManager.h"
#include "CPU/Pins/PinManager.h"
#include "CPU/Interrupts/InterruptManager.h"
#include "CPU/BasicClock/BasicClock.h"
#include "CPU/BasicClock/MCLK.h"

#include "Package.h"
#include "CodeUtil.h"
#include "SimulationObjects/Timer/AdevsTimerFactory.h"
#include "SimulationObjects/Timer/DCO.h"
#include "SimulationObjects/Timer/VLO.h"
#include "SimulationObjects/Timer/LFXT1.h"
#include "PeripheralItem/MSP430PeripheralItem.h"

#include <QWidget>
#include <QApplication>
#include <QString>
#include <QFile>
#include <QPen>
#include <QPainter>
#include <QDebug>
#include <QDomDocument>
#include <QFileDialog>
#include <QtCore/qplugin.h>

MCU_MSP430::MCU_MSP430(const QString &variant) :
m_time(0), m_instructionCycles(0),
m_mem(0), m_reg(0), m_decoder(0), m_pinManager(0), m_intManager(0),
m_instruction(new MSP430::Instruction), m_variant(0),
m_timerFactory(new AdevsTimerFactory()), m_ignoreNextStep(false), m_counter(-1),
m_syncing(0) {

	m_variant = ::getVariant(variant.toStdString().c_str());
	if (!m_variant) {
		return;
	}

	QString package = QString("Packages/") + variant + ".xml";

	m_name = "MSP430";

	m_mem = new MSP430::Memory(512000);
	m_reg = new MSP430::RegisterSet();
	m_reg->addDefaultRegisters();

	m_intManager = new MSP430::InterruptManager(m_reg, m_mem, m_variant);
	m_pinManager = new MSP430::PinManager(m_mem, m_intManager, m_variant);
	m_pinManager->setWatcher(this);
	Package::loadPackage(this, m_pinManager, package, m_pins, m_sides);

	m_basicClock = new MSP430::BasicClock(m_mem, m_variant, m_intManager, m_pinManager, m_timerFactory);
	m_basicClock->getMCLK()->addHandler(this);
	reset();

	m_peripheralItem = new MSP430PeripheralItem(this);

	m_options << "Load ELF";
	m_options << "Load A43 (IHEX)";
}

void MCU_MSP430::reset() {
	delete m_decoder;

	m_mem->reset();
	//m_reg->reset(); TODO
	m_intManager->reset();
	m_pinManager->reset();
	m_basicClock->reset();

	m_decoder = new MSP430::InstructionDecoder(m_reg, m_mem);

	if (!m_code.isEmpty()) {
		m_mem->loadA43(m_code.toStdString(), m_reg);
	}

	 m_counter = 0;
	 m_instructionCycles = m_decoder->decodeCurrentInstruction(m_instruction);
}

RegisterSet *MCU_MSP430::getRegisterSet() {
	return m_reg;
}

QString MCU_MSP430::getVariant() {
	if (!m_variant) {
		return "";
	}

	return m_variant->getName();
}

QStringList MCU_MSP430::getVariants() {
	QStringList ret;
	std::vector<_msp430_variant *> variants = ::getVariants();
	for (std::vector<_msp430_variant *>::iterator it = variants.begin(); it != variants.end(); it++) {
		ret << QString((*it)->name);
	}
	return ret;
}

void MCU_MSP430::handlePinChanged(int id, double value) {
	m_pins[id].value = value;
	bool reschedule = m_output.empty();
	m_output.insert(SimulationEvent(id, value));
	if (reschedule) {
		m_wrapper->setContext(m_instruction->original_pc);
		m_wrapper->reschedule();
	}
	onUpdated();
}


void MCU_MSP430::getInternalSimulationObjects(std::vector<SimulationObject *> &objects) {
	objects.push_back(dynamic_cast<DCO *>(m_basicClock->getDCO()));
	objects.push_back(dynamic_cast<VLO *>(m_basicClock->getVLO()));
	objects.push_back(dynamic_cast<LFXT1 *>(m_basicClock->getLFXT1()));
}

void MCU_MSP430::externalEvent(double t, const SimulationEventList &events) {
	for (SimulationEventList::const_iterator it = events.begin(); it != events.end(); ++it) {
		if (!m_pinManager->handlePinInput((*it).port, (*it).value)) {
			qDebug() << "WARN: input on output PIN";
		}
		m_pins[(*it).port].value = (*it).value;
		onUpdated();
	}
}

void MCU_MSP430::output(SimulationEventList &output) {
	if (!m_output.empty()) {
		output = m_output;
		m_output.clear();
	}
}

void MCU_MSP430::tick() {
// 	std::cout << "tick\n";
	if (++m_counter == m_instructionCycles) {
// 		std::cout << "executed\n";
		int cycles = executeInstruction(m_reg, m_mem, m_instruction);
		if (cycles == -1) {
			qDebug() << "ERROR: Unknown instruction" << "type" << m_instruction->type << "opcode" << m_instruction->opcode;
			m_instructionCycles = DBL_MAX;
			return;
		}

		m_intManager->handleInstruction(m_instruction);

		m_counter = 0;
		if (m_intManager->runQueuedInterrupts()) {
			m_instructionCycles = m_decoder->decodeCurrentInstruction(m_instruction);
			m_instructionCycles += 5;
		}
		else {
			m_instructionCycles = m_decoder->decodeCurrentInstruction(m_instruction);
		}
	}
}

void MCU_MSP430::internalTransition() {
	return;
}

double MCU_MSP430::timeAdvance() {
	if (!m_output.empty()) {
// 		qDebug() << "MCU_MSP430 ta=" << 0;
		m_ignoreNextStep = true;
		return 0;
	}
// 	qDebug() << "MCU_MSP430 ta=" << m_instructionCycles;
	return 365;
}

void MCU_MSP430::executeOption(int option) {
	switch (option) {
		case 0:
			loadELFOption();
			break;
		case 1:
			loadA43Option();
			break;
		default:
			break;
	}
}

void MCU_MSP430::save(QTextStream &stream) {
	ScreenObject::save(stream);
	stream << "<code>";
	stream << m_code;
	stream << "</code>\n";
	stream << "<variant>";
	stream << QString(m_variant->getName());
	stream << "</variant>\n";
	stream << "<elf>";
	stream << m_elf.toBase64();
	stream << "</elf>";
}

void MCU_MSP430::load(QDomElement &object) {
	loadA43(object.firstChildElement("code").text());
	loadELF(QByteArray::fromBase64(object.firstChildElement("elf").text().toAscii()));
}

void MCU_MSP430::paint(QWidget *screen) {
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

	if (m_code.isEmpty()) {
		QPen pen(Qt::black, 1, Qt::SolidLine);
		qp.setPen(pen);
		qp.drawText(m_x + 40, m_y , width() - 80, height(), Qt::AlignCenter | Qt::TextWordWrap,
					"No code loaded. Load the code by right clicking on this MCU and selecting \"Load ELF\".");
	}
}

bool MCU_MSP430::loadA43(const QString &data) {
	m_code = data;

	bool ret =  m_mem->loadA43(data.toStdString(), m_reg);
	if (ret) {
		onCodeLoaded();
	}
	return ret;
}

void MCU_MSP430::loadA43Option() {
	QString filename = QFileDialog::getOpenFileName();
	if (filename.isEmpty()) {
		return;
	}

	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	m_elf.clear();
	loadA43(file.readAll().data());
}

void MCU_MSP430::loadELF(const QByteArray &elf) {
	m_elf = elf;

	if (!m_elf.isEmpty()) {
		QString a43 = CodeUtil::ELFToA43(elf);
		loadA43(a43);
	}
}

void MCU_MSP430::loadELFOption() {
	QString filename = QFileDialog::getOpenFileName();
	if (filename.isEmpty()) {
		return;
	}

	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
		return;

	loadELF(file.readAll());
}

DisassembledCode MCU_MSP430::getDisassembledCode() {
	return CodeUtil::disassemble(m_elf, m_code);
}

MCU *MSP430Interface::create(const QString &variant) {
	return new MCU_MSP430(variant);
}

Q_EXPORT_PLUGIN2(msp430mcu, MSP430Interface);
