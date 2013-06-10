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

#include "GPPinHandler.h"
#include "PinMultiplexer.h"
#include "CPU/Variants/Variant.h"
#include "CPU/Memory/Memory.h"
#include "CPU/Interrupts/InterruptManager.h"
#include <iostream>

namespace MSP430 {
	
GPPinHandler::GPPinHandler(Memory *mem, PinMultiplexer *mpx, InterruptManager *intManager,
						   uint16_t dir, uint16_t in, uint16_t out,
						   uint16_t ie, uint16_t ies, uint16_t ifg, uint16_t intvec,
						   uint8_t id) :
m_mem(mem), m_mpx(mpx), m_intManager(intManager), m_dir(dir), m_in(in), m_out(out), m_ie(ie),
m_ies(ies), m_ifg(ifg), m_intvec(intvec), m_id(id), m_active(true), m_oldValue(false) {
	m_id = 1 << m_id;

	m_mem->addWatcher(m_out, this);
	m_mem->addWatcher(m_dir, this);
}

GPPinHandler::~GPPinHandler() {
	
}

void GPPinHandler::handleMemoryChanged(Memory *memory, uint16_t address) {
	if (!m_active) {
		return;
	}

	// We are input PIN, so no output
	if (m_mem->isBitSet(m_dir, m_id) == false) {
		return;
	}

	bool value = m_mem->isBitSet(m_out, m_id);
	if (m_oldValue != value) {
		m_mpx->generateOutput(this, value ? 1.0 : 0.0);
		m_oldValue = value;
	}
}

void GPPinHandler::handlePinInput(const std::string &name, double value) {
	bool old_value = m_mem->isBitSet(m_in, m_id);
	// Checks if the pin is input and stores the value in the
	// proper address in memory
	if (!(m_mem->getByte(m_dir) & m_id)) {
		m_mem->setBit(m_in, m_id, value >= 2.0); \
	}

	// No interrupt handling for this PIN.
	if (m_ie == 0) {
		return;
	}

	// 1. Check if interrupts are enabled for given pin (PxIE)
	// 2. Check if the condition for interrupt was met (PxIES)
	// 3. Set the PxIFG value according to given pin
	// 4. Inform InterruptManager that we have interrupt here.
	if (m_mem->getByte(m_ie) & m_id) {
		// Interrupt enabled
		if (m_mem->getByte(m_ies) & m_id) {
			// Low to high
			if (old_value == 0 && value >= 2.0) {
				m_mem->setBit(m_ifg, m_id, 1);
				m_intManager->queueInterrupt(m_intvec);
			}
		}
		else {
			// High to low
			if (old_value == 1 && value < 1.0) {
				m_mem->setBit(m_ifg, m_id, 1);
				m_intManager->queueInterrupt(m_intvec);
			}
		}
	}
}

void GPPinHandler::handlePinActivated(const std::string &name) {
	m_active = true;
}

void GPPinHandler::handlePinDeactivated(const std::string &name) {
	m_active = false;
}

}
