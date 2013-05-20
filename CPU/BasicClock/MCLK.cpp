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

namespace MCU {

MCLK::MCLK(Memory *mem, Variant *variant, DCO *dco, VLO *vlo, LFXT1 *lfxt1) :
m_mem(mem), m_variant(variant), m_source(dco), m_dco(dco), m_vlo(vlo),
m_lfxt1(lfxt1), m_divider(1) {

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
	return m_source->getFrequency() / m_divider;
}

double MCLK::getStep() {
	return m_source->getStep() * m_divider;
}

void MCLK::reset() {
	handleMemoryChanged(m_mem, m_variant->getBCSCTL1());
}


void MCLK::handleMemoryChanged(Memory *memory, uint16_t address) {
	// Set divider and source
	uint16_t ctl2 = m_mem->getBigEndian(m_variant->getBCSCTL2());

	// Choose source - SELMx
	switch ((ctl2 >> 6) & 3) {
		case 0: case 1: m_source = m_dco; break;
		case 2: // TODO: XT2 when present on chip
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

	// Choose divider - DIVMx
	switch((ctl2 >> 4) & 3) {
		case 0: m_divider = 1; break;
		case 1: m_divider = 2; break;
		case 2: m_divider = 4; break;
		case 3: m_divider = 8; break;
		default: break;
	}
}

}

