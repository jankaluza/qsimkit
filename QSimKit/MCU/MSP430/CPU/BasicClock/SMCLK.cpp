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

#include "SMCLK.h"
#include "CPU/Variants/Variant.h"
#include "CPU/Memory/Memory.h"
#include "CPU/Interrupts/InterruptManager.h"
#include <iostream>

#include "DCO.h"
#include "XT2.h"

namespace MSP430 {

SMCLK::SMCLK(Memory *mem, Variant *variant, DCO *dco, XT2 *xt2) :
m_mem(mem), m_variant(variant), m_source(dco), m_dco(dco), m_xt2(xt2),
m_divider(1), m_counter(0), m_rising(false), m_running(false) {
#define ADD_WATCHER(METHOD) \
	if (METHOD != 0) { m_mem->addWatcher(METHOD, this); }
	ADD_WATCHER(m_variant->getBCSCTL2());

	reset();
}

SMCLK::~SMCLK() {

}

unsigned long SMCLK::getFrequency() {
	if (!m_source) {
		return 0;
	}

	return m_source->getFrequency() / m_divider;
}

std::string SMCLK::getSourceName() {
	if (!m_source) {
		return "None";
	}

	return m_source->getName();
}

void SMCLK::tickRising() {
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

void SMCLK::tickFalling() {
	// Ignore falling tick if divider is one, because falling tick is generated
	// on second rising tick in this case.
	if (m_divider != 1) {
		return;
	}

	tickRising();
}

void SMCLK::start() {
	if (m_running) {
		return;
	}

// 	std::cout << "Starting SMCLK\n";

	if (m_source) {
		m_source->addHandler(this);
		m_running = true;
	}
}

void SMCLK::pause() {
	m_running = false;
// 	std::cout << "Pausing SMCLK\n";

	if (m_source) {
		m_source->removeHandler(this);
	}
}

void SMCLK::reset() {
	handleMemoryChanged(m_mem, m_variant->getBCSCTL2());
	if (m_source) {
		m_source->removeHandler(this);
	}
	m_source = m_dco;
// 	m_source->addHandler(this);
}


void SMCLK::handleMemoryChanged(::Memory *memory, uint16_t address) {
	uint16_t ctl2 = m_mem->getBigEndian(m_variant->getBCSCTL2());

	// Choose divider - DIVSx
	switch((ctl2 >> 1) & 3) {
		case 0: m_divider = 1; break;
		case 1: m_divider = 2; break;
		case 2: m_divider = 4; break;
		case 3: m_divider = 8; break;
		default: break;
	}

	m_counter = m_divider;

	if (m_source) {
		m_source->removeHandler(this);
	}
	// Choose source - SELSx
	switch ((ctl2 >> 3) & 1) {
		case 0: m_source = m_dco; break;
		case 1: m_source = m_xt2; break;
		default:
			break;
	}
	m_source->addHandler(this);
}

}
