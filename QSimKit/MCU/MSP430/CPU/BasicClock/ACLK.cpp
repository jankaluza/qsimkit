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

#include "ACLK.h"
#include "CPU/Variants/Variant.h"
#include "CPU/Memory/Memory.h"
#include "CPU/Interrupts/InterruptManager.h"
#include <iostream>

#include "VLO.h"
#include "LFXT1.h"

namespace MSP430 {

ACLK::ACLK(Memory *mem, Variant *variant, VLO *vlo, LFXT1 *lfxt1) :
m_mem(mem), m_variant(variant), m_source(0), m_vlo(vlo),
m_lfxt1(lfxt1), m_divider(1), m_counter(0), m_rising(false) {

#define ADD_WATCHER(METHOD) \
	if (METHOD != 0) { m_mem->addWatcher(METHOD, this); }

	ADD_WATCHER(m_variant->getBCSCTL1());
	ADD_WATCHER(m_variant->getBCSCTL3());

	reset();
}

ACLK::~ACLK() {

}

unsigned long ACLK::getFrequency() {
	if (!m_source) {
		return 0;
	}

	return m_source->getFrequency() / m_divider;
}

std::string ACLK::getSourceName() {
	if (!m_source) {
		return "None";
	}

	return m_source->getName();
}

void ACLK::tickRising() {
	if (++m_counter >= (m_divider >> 1)) {
		m_counter = 0;

		m_rising = !m_rising;
		if (m_rising) {
			callRisingHandlers();
		}
		else {
			callFallingHandlers();
		}
	}
}

void ACLK::tickFalling() {
	// Ignore falling tick if divider is one, because falling tick is generated
	// on second rising tick in this case.
	if (m_divider != 1) {
		return;
	}

	tickRising();
}

void ACLK::reset() {
	handleMemoryChanged(m_mem, m_variant->getBCSCTL1());
	handleMemoryChanged(m_mem, m_variant->getBCSCTL3());
	if (m_source) {
		m_source->removeHandler(this);
	}
	m_source = m_vlo;
	m_source->addHandler(this);
}


void ACLK::handleMemoryChanged(::Memory *memory, uint16_t address) {
	uint16_t value = m_mem->getBigEndian(address);
	if (address == m_variant->getBCSCTL1()) {
		// Set divider according DIVAx bits
		switch((value >> 4) & 3) {
			case 0: m_divider = 1; break;
			case 1: m_divider = 2; break;
			case 2: m_divider = 4; break;
			case 3: m_divider = 8; break;
			default: break;
		}

		m_counter = m_divider;
	}
	else if (address == m_variant->getBCSCTL3()) {
		if (m_source) {
			m_source->removeHandler(this);
		}

		// Choose between VLO and LFXT1
		if (m_lfxt1->isChosen()) {
			m_source = m_lfxt1;
		}
		else {
			m_source = m_vlo;
		}
		m_source->addHandler(this);
	}
}

}
