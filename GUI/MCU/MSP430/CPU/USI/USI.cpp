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

#include "USI.h"
#include "CPU/Variants/Variant.h"
#include "CPU/Memory/Memory.h"
#include "CPU/Interrupts/InterruptManager.h"
#include "CPU/Pins/PinManager.h"
#include "CPU/Pins/PinMultiplexer.h"
#include "CPU/BasicClock/ACLK.h"
#include "CPU/BasicClock/SMCLK.h"
#include <iostream>

namespace MSP430 {

USI::USI(PinManager *pinManager, InterruptManager *intManager, Memory *mem, Variant *variant,
			 ACLK *aclk, SMCLK *smclk, uint16_t usictl, uint16_t usicctl, uint16_t usisr) :
m_pinManager(pinManager), m_intManager(intManager), m_mem(mem), m_variant(variant), m_source(0),
m_divider(1), m_aclk(aclk), m_smclk(smclk), m_usictl(usictl), m_usicctl(usicctl),
m_usisr(usisr), m_counter(0) {

	m_mem->addWatcher(m_usicctl, this);
	m_sdiMpx = m_pinManager->addPinHandler("SDI", this);
	m_sdoMpx = m_pinManager->addPinHandler("SDO", this);
	m_sclkMpx = m_pinManager->addPinHandler("SCLK", this);

	reset();
}

USI::~USI() {

}

void USI::tickRising() {
	if (++m_counter >= m_divider) {
		m_counter = 0;
	}
}

void USI::tickFalling() {
	
}

void USI::reset() {
	if (m_source) {
		m_source->removeHandler(this);
	}
	m_source = m_aclk;
	m_source->addHandler(this, Clock::Rising);
}

void USI::handleMemoryChanged(::Memory *memory, uint16_t address) {
	// USICKCTL - Clock Control
	if (address == m_usicctl) {
		uint8_t val = m_mem->getByte(address);

		// divider
		m_divider = 1 << ((val >> 5) & 7);

		if (m_source) {
			m_source->removeHandler(this);
		}

		// source
		switch((val >> 2) & 7) {
			case 0:
				// TODO: SCLK
				break;
			case 1:
				m_source = m_aclk;
				break;
			case 2: case 3:
				m_source = m_smclk;
				break;
			case 4:
				// USISWCLK - software clock
				m_source = 0;
				break;
			case 5:
				// TODO: TACCR0
				break;
			case 6:
				// TODO: TACCR1
				break;
			case 7:
				// TODO: TACCR2
				break;
		}

		if (m_source) {
			m_source->addHandler(this, Clock::Rising);
		}
	}
}

void USI::handleInterruptFinished(InterruptManager *intManager, int vector) {

}

void USI::handleMemoryRead(::Memory *memory, uint16_t address, uint16_t &value) {

}


void USI::handlePinInput(const std::string &name, double value) {

}

void USI::handlePinActivated(const std::string &name) {
	
}

void USI::handlePinDeactivated(const std::string &name) {
	
}

}
