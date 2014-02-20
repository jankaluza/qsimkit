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

#include "BreakpointManager.h"
#include "MCU/Register.h"
#include "MCU/RegisterSet.h"
#include "MCU/MCU.h"

BreakpointManager::BreakpointManager() : m_mcu(0), m_break(0) {
	// We need at least PC register at the beginning.
	m_breaks.append(QList<uint16_t>());
}

BreakpointManager::~BreakpointManager() {

}

bool BreakpointManager::handleRegisterChanged(Register *reg, int id, uint16_t value) {
	if (m_break) {
		return true;
	}
	QList<uint16_t> &b = m_breaks[id];
	if (b.empty())
		return true;

	QList<uint16_t>::iterator it;
	it = qBinaryFind(b.begin(), b.end(), reg->getBigEndian());
	if (it != b.end()) {
		m_break = true;
		return true;
	}

	return true;
}

void BreakpointManager::handleMemoryChanged(Memory *memory, uint16_t address) {
	if (m_break) {
		return;
	}

	if (m_membreaks[address].any || m_membreaks[address].val == memory->getBigEndian(address)) {
		m_break = true;
	}
}

void BreakpointManager::setMCU(MCU *mcu) {
	m_mcu = mcu;
	m_breaks.clear();
	m_membreaks.clear();

	for (int i = 0; i < m_mcu->getRegisterSet()->size(); ++i) {
		m_breaks.append(QList<uint16_t>());
	}
}

void BreakpointManager::addMemoryBreak(uint16_t addr, uint16_t value) {
	m_mcu->getMemory()->addWatcher(addr, this);
	MemoryBreak v = {value, false};
	m_membreaks[addr] = v;

	onMemoryBreakAdded(addr);
}

void BreakpointManager::addMemoryBreak(uint16_t addr) {
	m_mcu->getMemory()->addWatcher(addr, this);
	MemoryBreak v = {0, true};
	m_membreaks[addr] = v;

	onMemoryBreakAdded(addr);
}

void BreakpointManager::removeMemoryBreak(uint16_t addr) {
	m_mcu->getMemory()->removeWatcher(addr, this);
	m_membreaks.remove(addr);

	onMemoryBreakRemoved(addr);
}

void BreakpointManager::addRegisterBreak(int reg, uint16_t value) {
	m_mcu->getRegisterSet()->get(reg)->addWatcher(this);
	m_breaks[reg].append(value);
	qSort(m_breaks[reg]);

	onRegisterBreakAdded(reg, value);
}

void BreakpointManager::removeRegisterBreak(int reg, uint16_t value) {
	m_breaks[reg].removeAll(value);
	if (m_breaks[reg].empty()) {
		m_mcu->getRegisterSet()->get(reg)->removeWatcher(this);
	}

	onRegisterBreakRemoved(reg, value);
}

bool BreakpointManager::shouldBreak() {
	if (m_break) {
		m_break = false;
		return true;
	}
	return false;
}

void BreakpointManager::save(QTextStream &stream) {
	stream << "<registerbreakpoints>\n";
	for (int i = 0; i < m_breaks.size(); ++i) {
		stream << "<register id='" << i << "'>\n";
		foreach(uint16_t value, m_breaks[i]) {
			stream << "<value>" << value << "</value>\n";
		}
		stream << "</register>\n";
	}
	stream << "</registerbreakpoints>\n";

	stream << "<memorybreakpoints>\n";
	QHashIterator<uint16_t, MemoryBreak> i(m_membreaks);
	while (i.hasNext()) {
		i.next();
		stream << "<break address='" << i.key() << "' any='" << i.value().any << "' value='" << i.value().val << "'/>\n";
	}
	stream << "</memorybreakpoints>\n";
}

bool BreakpointManager::load(QDomDocument &doc) {
	QDomElement root = doc.firstChild().toElement();

	QDomElement registers = root.firstChildElement("registerbreakpoints");
	for(QDomNode node = registers.firstChild(); !node.isNull(); node = node.nextSibling()) {
		QDomElement reg = node.toElement();
		int id = reg.attribute("id").toInt();

		for(QDomNode node2 = reg.firstChild(); !node2.isNull(); node2 = node2.nextSibling()) {
			addRegisterBreak(id, node2.toElement().text().toAscii().toUInt());
		}
	}

	return true;
}
