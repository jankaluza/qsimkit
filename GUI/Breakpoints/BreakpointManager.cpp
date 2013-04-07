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
#include "Breakpoint.h"
#include "CPU/Memory/Register.h"
#include "CPU/Memory/RegisterSet.h"
#include "Peripherals/MSP430/MSP430.h"

BreakpointManager::BreakpointManager() : m_cpu(0), m_break(0) {

}

BreakpointManager::~BreakpointManager() {

}

void BreakpointManager::handleRegisterChanged(Register *reg, int id, uint16_t value) {
	if (m_break) {
		return;
	}
	QList<uint16_t> &b = m_breaks[id];
	if (b.empty())
		return;

	QList<uint16_t>::iterator it;
	it = qBinaryFind(b.begin(), b.end(), reg->getBigEndian());
	if (it != b.end()) {
		m_break = true;
		return;
	}
}

void BreakpointManager::setCPU(MSP430 *cpu) {
	m_cpu = cpu;

	for (int i = 0; i < m_cpu->getRegisterSet()->size(); ++i) {
		m_breaks.append(QList<uint16_t>());
	}
}

void BreakpointManager::addRegisterBreak(int reg, uint16_t value) {
	m_cpu->getRegisterSet()->get(reg)->addWatcher(this);
	m_breaks[reg].append(value);
	qSort(m_breaks[reg]);
}

void BreakpointManager::removeRegisterBreak(int reg, uint16_t value) {
	m_breaks[reg].removeAll(value);
	if (m_breaks[reg].empty()) {
		m_cpu->getRegisterSet()->get(reg)->removeWatcher(this);
	}
}

bool BreakpointManager::shouldBreak() {
	if (m_break) {
		m_break = false;
		return true;
	}
	return false;

// 	if (m_breaks.empty()) {
// 		return false;
// 	}
// 
// 	QList<uint16_t>::iterator it;
// 	it = qBinaryFind(m_breaks.begin(), m_breaks.end(), m_cpu->getRegisterSet()->get(0)->getBigEndian());
// 	if (it != m_breaks.end()) {
// 		return true;
// 	}
// 
// 	return false;
}

void BreakpointManager::addBreakpoint(Breakpoint *b) {
	m_breakpoints.append(b);
}

void BreakpointManager::removeBreakpoint(Breakpoint *b) {
	m_breakpoints.removeAll(b);
}
