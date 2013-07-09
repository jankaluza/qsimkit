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

#include "XT2.h"
#include "CPU/Variants/Variant.h"
#include "CPU/Memory/Memory.h"
#include "CPU/Interrupts/InterruptManager.h"
#include "CPU/Pins/PinManager.h"
#include <iostream>

namespace MSP430 {

XT2::XT2(Memory *mem, Variant *variant, PinManager *pinManager) :
m_mem(mem), m_variant(variant), m_state(false) {

#define ADD_WATCHER(METHOD) \
	if (METHOD != 0) { m_mem->addWatcher(METHOD, this); }

// 	ADD_WATCHER(m_variant->getBCSCTL1());
// 	ADD_WATCHER(m_variant->getBCSCTL3());

	pinManager->addPinHandler("XT2IN", this);

	reset();
}

XT2::~XT2() {

}

void XT2::reset() {
}

void XT2::handleMemoryChanged(::Memory *memory, uint16_t address) {
}

void XT2::handlePinInput(const std::string &name, double value) {
	bool newState = value > 1.5;
	if (m_state != newState) {
		m_state = newState;
		tick();
	}
}

void XT2::handlePinActivated(const std::string &name) {
	
}

void XT2::handlePinDeactivated(const std::string &name) {
	
}


}
