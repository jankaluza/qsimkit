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
#include "CPU/Pins/PinMultiplexer.h"
#include "CPU/Interrupts/InterruptManager.h"
#include "CPU/BasicClock/BasicClock.h"
#include "CPU/BasicClock/MCLK.h"
#include "CPU/BasicClock/Timer.h"
#include "CPU/USI/USI.h"
#include "CPU/USCI/USCIModules.h"
#include "CPU/USART/USARTModules.h"

#include "Package.h"
#include "CodeUtil.h"
#include "SimulationObjects/Timer/AdevsTimerFactory.h"
#include "SimulationObjects/Timer/DCO.h"
#include "SimulationObjects/Timer/VLO.h"
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
#include <QMessageBox>
#include <QtCore/qplugin.h>

MCU_MSP430::MCU_MSP430(const QString &variant) :
m_time(0), m_instructionCycles(0),
m_mem(0), m_reg(0), m_decoder(0), m_pinManager(0), m_intManager(0),
m_instruction(new MSP430::Instruction), m_variant(0),
m_timerFactory(new AdevsTimerFactory()), m_ignoreNextStep(false), m_counter(-1),
m_syncing(0) {

	m_variantStr = variant;
	m_variant = ::getVariant(variant.toStdString().c_str());
	if (!m_variant) {
		return;
	}

	m_fileWatcher = new QFileSystemWatcher(this);
	connect(m_fileWatcher, SIGNAL(fileChanged(const QString &)), this, SLOT(handleFileChanged(const QString &)) );

	m_name = "MSP430";

	m_mem = new MSP430::Memory(512000);
	m_reg = new MSP430::RegisterSet();
	m_reg->addDefaultRegisters();

	m_intManager = new MSP430::InterruptManager(m_reg, m_mem, m_variant);
	m_pinManager = new MSP430::PinManager(m_mem, m_intManager, m_variant);
	m_pinManager->setWatcher(this);

	QString error;
	loadPackage(m_variantStr, error);

	m_basicClock = new MSP430::BasicClock(m_mem, m_variant, m_intManager, m_pinManager, m_timerFactory);
	m_basicClock->getMCLK()->addHandler(this, MSP430::Clock::Rising);

	m_usi = 0;
	if (m_variant->getUSICTL() != 0) {
		m_usi = new MSP430::USI(m_pinManager, m_intManager, m_mem, m_variant,
								m_basicClock->getACLK(), m_basicClock->getSMCLK());
	}

	m_usci = new MSP430::USCIModules(m_pinManager, m_intManager, m_mem, m_variant,
									 m_basicClock->getACLK(), m_basicClock->getSMCLK());
	m_usart = new MSP430::USARTModules(m_pinManager, m_intManager, m_mem, m_variant,
									 m_basicClock->getACLK(), m_basicClock->getSMCLK());

	reset();

	m_peripheralItem = new MSP430PeripheralItem(this);

	m_options << "Load ELF";
	m_options << "Load A43 (IHEX)";
}

QString MCU_MSP430::getFeatures() {
	QString ret;

	ret += "<h3>" + m_variantStr + "</h3>";
	ret += "<ul>";
	if (m_basicClock->getTimerA()) {
		ret += "<li>Timer_A" + QString::number(m_basicClock->getTimerA()->getCCRCount()) + " (TA0.0, TA0.CCI0A, TA0.CCI0B, ...)</li>";
	}
	if (m_basicClock->getTimerA1()) {
		ret += "<li>Timer1_A" + QString::number(m_basicClock->getTimerA1()->getCCRCount()) + " (TA1.0, TA1.CCI0A, TA1.CCI0B, ...)</li>";
	}
	if (m_basicClock->getTimerB()) {
		ret += "<li>Timer_B" + QString::number(m_basicClock->getTimerB()->getCCRCount()) + " (TB0.0, TB0.CCI0A, TB0.CCI0B, ...)</li>";
	}
	if (m_usi) {
		ret += "<li>USI (SDI, SDO, SCLK)</li>";
	}

	ret += "</ul>";

	return ret;
}

void MCU_MSP430::handleFileChanged(const QString &path) {
	QMessageBox::StandardButton ret = QMessageBox::question(0, tr("Loaded file changed"),
		"Loaded ELF/A43 file changed in the meantime. Do you want to reload it?",
		QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

	if (ret != QMessageBox::Yes) {
		return;
	}

	if (path == m_elfPath) {
		loadELFOption(m_elfPath);
	}
	else if (path == m_a43Path) {
		loadA43Option(m_a43Path);
	}
}

void MCU_MSP430::reset() {
	delete m_decoder;

	m_mem->reset();
	//m_reg->reset(); TODO
	m_intManager->reset();
	m_pinManager->reset();
	m_basicClock->reset();
	m_usart->reset();
	m_usci->reset();
	if (m_usi) m_usi->reset();

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

Memory *MCU_MSP430::getMemory() {
	return m_mem;
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
	if (reschedule && m_wrapper) {
		m_wrapper->setContext(m_instruction->original_pc);
		m_wrapper->reschedule();
	}

	update();
}


void MCU_MSP430::getInternalSimulationObjects(std::vector<SimulationObject *> &objects) {
	objects.push_back(dynamic_cast<DCO *>(m_basicClock->getDCO()));
	objects.push_back(dynamic_cast<VLO *>(m_basicClock->getVLO()));
}

void MCU_MSP430::externalEvent(double t, const SimulationEventList &events) {
	int i = 0;
	for (SimulationEventList::const_iterator it = events.begin(); i != events.size(); ++it, ++i) {
		SimulationEvent &ev = *it;
		if (!m_pinManager->handlePinInput(ev.port, ev.value)) {
			qDebug() << "WARN: input on output PIN";
		}
		m_pins[ev.port].value = ev.value;
	}
	update();
}

void MCU_MSP430::output(SimulationEventList &output) {
	if (!m_output.empty()) {
		output.swap(m_output);
	}
}

void MCU_MSP430::tickRising() {
	if (++m_counter == m_instructionCycles) {
		int error = executeInstruction(m_reg, m_mem, m_instruction);
		if (error == -1) {
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
	stream << "<code>" << m_code << "</code>\n";
	stream << "<variant>" << QString(m_variant->getName()) << "</variant>\n";
	stream << "<a43path>" << m_a43Path << "</a43path>\n";
	stream << "<elfpath>" << m_elfPath << "</elfpath>\n";
	stream << "<elf>" << m_elf.toBase64() << "</elf>\n";
}

bool MCU_MSP430::loadPackage(QString &variant, QString &error) {
	QString package = QApplication::applicationDirPath() + QString("/Packages/") + variant + ".xml";

	QDir pluginsDir(QApplication::applicationDirPath());
	if (!pluginsDir.exists("Packages")) {
		package = QString(PACKAGES) + "/Packages/" + variant + ".xml";
	}

	if (!Package::loadPackage(this, m_pinManager, package, m_pins, m_sides)) {
		error = QString("MSP430 variant '%1' cannot be loaded.").arg(variant);
		return false;
	}

	return true;
}

void MCU_MSP430::load(QDomElement &object, QString &error) {
	if (m_pins.empty()) {
		error = QString("MSP430 variant '%1' cannot be loaded.").arg(m_variantStr);
		return;
	}

	CodeUtil::checkPaths();

	loadA43(object.firstChildElement("code").text());
	loadELF(QByteArray::fromBase64(object.firstChildElement("elf").text().toAscii()));
	m_a43Path = object.firstChildElement("a43path").text().toAscii();
	m_elfPath = object.firstChildElement("elfpath").text().toAscii();

	// Check if the ELF/A43 file changed in the meantime and try to reload it
	if (!m_elfPath.isEmpty()) {
		m_fileWatcher->addPath(m_elfPath);
		QFile file(m_elfPath);
		if (file.open(QIODevice::ReadOnly)) {
			if (file.readAll() != m_elf) {
				handleFileChanged(m_elfPath);
			}
		}
	}
	else {
		m_fileWatcher->addPath(m_a43Path);
		QFile file(m_a43Path);
		if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			if (file.readAll().data() != m_code) {
				handleFileChanged(m_a43Path);
			}
		}
	}
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
	std::vector<MSP430::PinMultiplexer *>::const_iterator it2 = m_pinManager->getMultiplexers().begin();
	for (PinList::iterator it = m_pins.begin(); it != m_pins.end() && it2 != m_pinManager->getMultiplexers().end(); ++it, ++id, ++it2) {
		bool isInput;
		double value = *it2 ? (*it2)->getValue(isInput) : 0;
		if (value >= 1.5 && value != HIGH_IMPEDANCE) {
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
			even = !even;
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
			even = !even;
		}
	}

	if (m_pins.empty()) {
		QPen pen(Qt::black, 1, Qt::SolidLine);
		qp.setPen(pen);
		qp.drawText(m_x - 70, m_y + 20, 140, 200, Qt::AlignHCenter | Qt::TextWordWrap,
					"Package data for this variant has not been created yet.");
		return;
	}

	if (m_code.isEmpty()) {
		QPen pen(Qt::black, 1, Qt::SolidLine);
		qp.setPen(pen);
		if (width() < 200) {
			qp.drawText(m_x - 70, m_y + height() , width() + 140, height(), Qt::AlignHCenter | Qt::TextWordWrap,
						"No code loaded. Load the code by right clicking on this MCU and selecting \"Load ELF\".");
		}
		else {
			qp.drawText(m_x + 40, m_y , width() - 80, height(), Qt::AlignCenter | Qt::TextWordWrap,
						"No code loaded. Load the code by right clicking on this MCU and selecting \"Load ELF\".");
		}
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

void MCU_MSP430::loadA43Option(const QString &f) {
	QString filename = f;
	if (filename.isEmpty()) {
		filename = QFileDialog::getOpenFileName();
		if (filename.isEmpty()) {
			return;
		}
	}

	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	CodeUtil::checkPaths();

	m_elf.clear();
	loadA43(file.readAll().data());

	m_fileWatcher->removePath(m_elfPath);
	m_fileWatcher->removePath(m_a43Path);
	m_fileWatcher->addPath(filename);
	m_a43Path = filename;
	m_elfPath = "";
}

void MCU_MSP430::loadELF(const QByteArray &elf) {
	m_elf = elf;

	if (!m_elf.isEmpty()) {
		QString error;
		QString a43 = CodeUtil::ELFToA43(elf, error);
		if (!error.isEmpty()) {
			QMessageBox::critical(0, tr("Loading error"), error);
			return;
		}
		loadA43(a43);
	}
}

void MCU_MSP430::loadELFOption(const QString &f) {
	QString filename = f;
	if (filename.isEmpty()) {
		filename = QFileDialog::getOpenFileName();
		if (filename.isEmpty()) {
			return;
		}
	}

	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
		return;

	CodeUtil::checkPaths();

	loadELF(file.readAll());

	m_fileWatcher->removePath(m_elfPath);
	m_fileWatcher->removePath(m_a43Path);
	m_fileWatcher->addPath(filename);
	m_elfPath = filename;
	m_a43Path = "";
}

DisassembledFiles MCU_MSP430::getDisassembledCode() {
	QString error;
	DisassembledFiles files = CodeUtil::disassemble(m_elf, m_code, error);
	if (!error.isEmpty()) {
		QMessageBox::critical(0, tr("Loading error"), error);
	}

	if (m_basicClock->getMCLK()->getStep() > 1) {
		QMessageBox::critical(0, tr("MSP430 Variant error"),
			tr("One MCLK tick for this variant takes more than 1 second which is suspicious. Variant is not implemented properly.")
		);
	}

	return files;
}

DebugData *MCU_MSP430::getDebugData() {
	QString error;
	DebugData *dd = CodeUtil::getDebugData(m_elf, error);
	if (!error.isEmpty()) {
		delete dd;
		QMessageBox::critical(0, tr("Loading error"), error);
	}

	return dd;
}

MCU *MSP430Interface::create(const QString &variant) {
	return new MCU_MSP430(variant);
}

Q_EXPORT_PLUGIN2(msp430mcu, MSP430Interface);
