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

#include "Timer.h"
#include "CPU/Variants/Variant.h"
#include "CPU/Memory/Memory.h"
#include "CPU/Interrupts/InterruptManager.h"
#include <iostream>

#include "ACLK.h"

namespace MCU {

Timer::Timer(InterruptManager *intManager, Memory *mem, Variant *variant,
			 ACLK *aclk, SMCLK *smclk) :
m_intManager(intManager), m_mem(mem), m_variant(variant), m_source(0),
m_aclk(aclk), m_smclk(smclk), m_up1(true), m_up2(true) {
#define ADD_WATCHER(METHOD) \
	if (METHOD != 0) { m_mem->addWatcher(METHOD, this); }
	ADD_WATCHER(m_variant->getBCSCTL2());

	reset();
}

Timer::~Timer() {

}

void Timer::changeTAR(uint16_t address, uint8_t mode, bool &up) {
	switch (mode) {
		case 0:
			break;
		case 1:
			m_mem->setBigEndian(address, m_mem->getBigEndian(address) + 1);
			break;
		case 2:
			m_mem->setBigEndian(address, m_mem->getBigEndian(address) + 1);
			break;
		case 3:
			if (up) {
				m_mem->setBigEndian(address, m_mem->getBigEndian(address) + 1);
			}
			else {
				m_mem->setBigEndian(address, m_mem->getBigEndian(address) - 1);
			}
			break;
		default:
			break;
	}
}

void Timer::tick() {
	uint8_t mode = (m_mem->getByte(m_variant->getTA0CTL()) >> 4) & 3;
	uint16_t address = m_variant->getTA0R();

	changeTAR(address, mode, m_up1);

	mode = (m_mem->getByte(m_variant->getTA1CTL()) >> 4) & 3;
	if (mode) {
		address = m_variant->getTA1R();
		changeTAR(address, mode, m_up2);
	}
}

unsigned long Timer::getFrequency() {
	return m_source->getFrequency();
}

double Timer::getStep() {
	return m_source->getStep();
}

void Timer::reset() {
	m_source = m_aclk;
}


void Timer::handleMemoryChanged(Memory *memory, uint16_t address) {
	// Set divider and source
}

}
