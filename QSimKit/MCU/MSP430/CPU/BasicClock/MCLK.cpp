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

#include "MCLK.h"
#include "CPU/Variants/Variant.h"
#include "CPU/Memory/Memory.h"
#include "CPU/Interrupts/InterruptManager.h"
#include <iostream>

#include "DCO.h"
#include "LFXT1.h"
#include "VLO.h"
#include "XT2.h"

namespace MSP430 {

MCLK::MCLK(Memory *mem, Variant *variant, DCO *dco, VLO *vlo, LFXT1 *lfxt1, XT2 *xt2) :
m_mem(mem), m_variant(variant), m_source(dco), m_dco(dco), m_vlo(vlo),
m_lfxt1(lfxt1), m_xt2(xt2), m_divider(1), m_counter(0), m_rising(false) {

#define ADD_WATCHER(METHOD) \
	if (METHOD != 0) { m_mem->addWatcher(METHOD, this); }

	ADD_WATCHER(m_variant->getBCSCTL1());
	ADD_WATCHER(m_variant->getBCSCTL2());
	ADD_WATCHER(m_variant->getBCSCTL3());

	reset();
}

MCLK::~MCLK() {

}

unsigned long MCLK::getFrequency() {
	if (!m_source) {
		return 0;
	}

	return m_source->getFrequency() / m_divider;
}

std::string MCLK::getSourceName() {
	if (!m_source) {
		return "None";
	}

	return m_source->getName();
}

void MCLK::tickRising() {
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

void MCLK::tickFalling() {
	// Ignore falling tick if divider is one, because falling tick is generated
	// on second rising tick in this case.
	if (m_divider != 1) {
		return;
	}

	tickRising();
}

void MCLK::reset() {
	handleMemoryChanged(m_mem, m_variant->getBCSCTL1());
}

double MCLK::getStep() {
	return m_source->getStep();
}

void MCLK::handleMemoryChanged(::Memory *memory, uint16_t address) {
	// Set divider and source
	uint16_t ctl2 = m_mem->getBigEndian(m_variant->getBCSCTL2());

	if (m_source) {
		m_source->removeHandler(this);
	}
	// Choose source - SELMx
	switch ((ctl2 >> 6) & 3) {
		case 0: case 1: m_source = m_dco; break;
		case 2: m_source = m_xt2; break;
		case 3:
			if (m_lfxt1->isChosen()) {
				m_source = m_lfxt1;
			}
			else {
				m_source = m_vlo;
			}
			break;
		default:
			break;
	}
	m_source->addHandler(this);

	// Choose divider - DIVMx
	switch((ctl2 >> 4) & 3) {
		case 0: m_divider = 1; break;
		case 1: m_divider = 2; break;
		case 2: m_divider = 4; break;
		case 3: m_divider = 8; break;
		default: break;
	}

	m_counter = m_divider;
}

}

