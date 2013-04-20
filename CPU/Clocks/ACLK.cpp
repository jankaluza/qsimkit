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

namespace MCU {

ACLK::ACLK(Memory *mem, Variant *variant, VLO *vlo) :
m_mem(mem), m_variant(variant), m_source(0), m_vlo(vlo) {
#define ADD_WATCHER(METHOD) \
	if (METHOD != 0) { m_mem->addWatcher(METHOD, this); }
	ADD_WATCHER(m_variant->getBCSCTL2());

	reset();
}

ACLK::~ACLK() {

}

unsigned long ACLK::getFrequency() {
	return m_source->getFrequency();
}

double ACLK::getStep() {
	return m_source->getStep();
}

void ACLK::reset() {
	m_source = m_vlo;
}


void ACLK::handleMemoryChanged(Memory *memory, uint16_t address) {
	// Set divider and source
}

}
