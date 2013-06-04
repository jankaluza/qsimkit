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

#include "InterruptManager.h"
#include "CPU/Variants/Variant.h"
#include "CPU/Memory/Memory.h"
#include "CPU/Memory/RegisterSet.h"
#include "CPU/Memory/Register.h"
#include "CPU/Instructions/Instruction.h"
#include <iostream>
#include <algorithm>

namespace MSP430 {

InterruptManager::InterruptManager(RegisterSet *reg, Memory *mem) : m_reg(reg), m_mem(mem) {

}

InterruptManager::~InterruptManager() {

}

void InterruptManager::queueInterrupt(int vector) {
	if (std::find(m_interrupts.begin(), m_interrupts.end(), vector) == m_interrupts.end()) {
		m_interrupts.push_back(vector);
		std::sort(m_interrupts.begin(), m_interrupts.end());
	}
}

void InterruptManager::handleInstruction(Instruction *instruction) {
	if (instruction->type == Instruction1 && instruction->opcode == 6) {
		int vector = m_runningInterrupts.back();
		m_runningInterrupts.pop_back();
		if (m_watchers.find(vector) != m_watchers.end()) {
			std::vector<InterruptWatcher *> &watchers = m_watchers[vector];			
			for (std::vector<InterruptWatcher *>::const_iterator it = watchers.begin(); it != watchers.end(); ++it) {
				(*it)->handleInterruptFinished(this, vector);
			}
		}
	}
}

void InterruptManager::runInterrupt(int vector) {
// 	std::cerr << "running interrupt vector " << vector << "\n";
	uint16_t v;
	Register *sp = m_reg->getp(1);

	// Push PC on stack
	v = sp->getBigEndian() - 2;
	sp->setBigEndian(v);
	m_mem->setBigEndian(v, m_reg->get(0)->getBigEndian());

	// Push SR on stack
	v = sp->getBigEndian() - 2;
	sp->setBigEndian(v);
	m_mem->setBigEndian(v, m_reg->get(2)->getBigEndian());

	// TODO: clear interrupt request flags for single-source interrupts.
	// We don't handle any interrupt like that yet

	// Clear SR
	m_reg->get(2)->setBigEndian(0);
	m_reg->get(2)->callWatchers();

	// Load content of interrupt vector to PC
	m_reg->get(0)->setBigEndian(m_mem->getBigEndian(0xffc0 + vector));

	m_runningInterrupts.push_back(vector);
}

bool InterruptManager::runQueuedInterrupts() {
	if (m_interrupts.empty() || !m_runningInterrupts.empty()) {
		return false;
	}

	int vector = m_interrupts.back();
	m_interrupts.pop_back();
	runInterrupt(vector);

	return true;
}

bool InterruptManager::hasQueuedInterrupts() {
	return !m_interrupts.empty();
}

void InterruptManager::clearQueuedInterrupts() {
	m_interrupts.clear();
}

void InterruptManager::addWatcher(int vector, InterruptWatcher *watcher) {
	m_watchers[vector].push_back(watcher);
}

}
