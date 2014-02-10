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

#include "LFXT1.h"
#include "CPU/Variants/Variant.h"
#include "CPU/Memory/Memory.h"
#include "CPU/Interrupts/InterruptManager.h"
#include "CPU/Pins/PinManager.h"
#include <iostream>

namespace MSP430 {

LFXT1::LFXT1(Memory *mem, Variant *variant, PinManager *pinManager) :
Oscillator("LFXT1"), m_mem(mem), m_variant(variant), m_state(false), m_enabled(false) {

#define ADD_WATCHER(METHOD) \
	if (METHOD != 0) { m_mem->addWatcher(METHOD, this); }

	ADD_WATCHER(m_variant->getBCSCTL1());
	ADD_WATCHER(m_variant->getBCSCTL3());

	pinManager->addPinHandler("XIN", this);

	reset();
}

LFXT1::~LFXT1() {

}

void LFXT1::reset() {
}

bool LFXT1::isChosen() {
	uint16_t value = m_mem->getBigEndian(m_variant->getBCSCTL3(), false);
	// Choose between VLO and LFXT1
	bool xts = m_mem->isBitSet(m_variant->getBCSCTL1(), 1 << 6);
	if (xts) {
		return true;
	}
	else {
		switch((value >> 4) & 3) {
			case 2: return false;
			default:
				return true;
		}
	}
}

void LFXT1::handleMemoryChanged(::Memory *memory, uint16_t address) {
	m_enabled = isChosen();
}

void LFXT1::handlePinInput(const std::string &name, double value) {
	if (!m_enabled) {
		return;
	}

	if (value == HIGH_IMPEDANCE) {
		return;
	}

	bool newState = value > 1.5;
	if (m_state != newState) {
		m_state = newState;
		tick();
	}
}

void LFXT1::handlePinActivated(const std::string &name) {
	
}

void LFXT1::handlePinDeactivated(const std::string &name) {
	
}


}
