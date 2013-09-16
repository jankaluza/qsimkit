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
			 ACLK *aclk, SMCLK *smclk) :
m_pinManager(pinManager), m_intManager(intManager), m_mem(mem), m_variant(variant), m_source(0),
m_divider(1), m_aclk(aclk), m_smclk(smclk), m_usictl(variant->getUSICTL()), m_usicctl(variant->getUSICCTL()),
m_usisr(variant->getUSISR()), m_counter(0), m_sclk(false), m_usickpl(false), m_input(false),
m_output(false) {

	m_mem->addWatcher(m_usicctl, this);
	m_mem->addWatcher(m_usicctl + 1, this);
	m_mem->addWatcher(m_usisr, this);
	m_mem->addWatcher(m_usictl, this);

	m_sdiMpx = m_pinManager->addPinHandler("SDI", this);
	m_sdoMpx = m_pinManager->addPinHandler("SDO", this);
	m_sclkMpx = m_pinManager->addPinHandler("SCLK", this);

	reset();
}

USI::~USI() {

}

void USI::doSPICapture(uint8_t usictl0, uint8_t usictl1, uint8_t usicnt) {
// 	std::cout << "capture\n";
	uint16_t usisr = m_mem->getBigEndian(m_usisr, false);
	if (usictl0 & (1 << 4)) {
		// LSB mode -> shift right
		usisr = usisr >> 1;
		if (usicnt & (1 << 6)) { 
			// 16 bit mode
			if (m_input) {
				usisr |= (1 << 15);
			}
			else {
				usisr &= ~(1 << 15);
			}
		}
		else {
			// 8 bit mode
			if (m_input) {
				usisr |= (1 << 8);
			}
			else {
				usisr &= ~(1 << 8);
			}
			usisr &= 0xff;
		}
	}
	else {
		// MSB mode -> shift left
		usisr = usisr << 1;
		if (m_input) {
			usisr |= 1;
		}
		else {
			usisr &= ~(1);
		}
		if (!(usicnt & (1 << 6))) { 
			usisr &= 0xff;
		}
	}

	m_mem->setByte(m_usisr, usisr);
	usicnt--;
	m_mem->setByte(m_usicctl + 1, usicnt, false);
	if ((usicnt & 31) == 0) {
// 		std::cout << "USISR = " << usisr << "\n";
		m_mem->setByte(m_usictl + 1, usictl1 | 1);
		m_intManager->queueInterrupt(m_variant->getUSI_VECTOR());
	}
// 	else {
// 		std::cout << "sample = " << m_input << " " << usisr << "\n";
// 	}
}

void USI::doSPIOutput(uint8_t usictl0, uint8_t usictl1, uint8_t usicnt) {
// 	std::cout << "output\n";
	uint16_t usisr = m_mem->getBigEndian(m_usisr, false);
	// Check LSB vs. MSB
	if (usictl0 & (1 << 4)) {
		// Load LSB bit
		m_output = usisr & 1;
	}
	else {
		// Check if we are working in 16 bit mode
		if (usicnt & (1 << 6)) {
			m_output = usisr & (1 << 15);
		}
		else {
			m_output = usisr & (1 << 7);
		}
	}

	// generate output only when USIOE and pin enabled
	if ((usictl0 & 2) && (usictl0 & (1 << 6))) {
		generateOutput(m_sdoMpx, m_output);
	}
}

void USI::handleFirstEdgeSPI(uint8_t usictl0, uint8_t usictl1, uint8_t usicnt) {
	// Check USICKPH:
	if (usictl1 & (1 << 7)) {
		doSPICapture(usictl0, usictl1, usicnt);
	}
	else {
		doSPIOutput(usictl0, usictl1, usicnt);
	}
}

void USI::handleSecondEdgeSPI(uint8_t usictl0, uint8_t usictl1, uint8_t usicnt) {
	// Check USICKPH:
	if (usictl1 & (1 << 7)) {
		doSPIOutput(usictl0, usictl1, usicnt);
	}
	else {
		doSPICapture(usictl0, usictl1, usicnt);
	}
}

void USI::handleTickSPI(bool rising, uint8_t usictl0, uint8_t usictl1) {
	uint8_t usicnt = m_mem->getByte(m_usicctl + 1, false);
	uint8_t cnt = usicnt & 31;
	// Master starts clocking data in/out when IFG = 0 and CNT > 0
	// Slave checks only CNT > 0 ???
	if ((!(usictl0 & (1 << 3)) || (usictl1 & 1) == 0) && cnt > 0) {
		// Rising is first edge when m_usickpl == !rising, otherwise
		// it's second edge
		bool first_edge = m_usickpl == !rising;
		if (first_edge) {
			handleFirstEdgeSPI(usictl0, usictl1, usicnt);
		}
		else {
			handleSecondEdgeSPI(usictl0, usictl1, usicnt);
		}

		// Master generates output clock
		if (usictl0 & (1 << 3) && (usictl0 & (1 << 5))) {
			m_sclk = rising;
			generateOutput(m_sclkMpx, m_sclk != m_usickpl);
		}
	}
}

void USI::tickRising() {
	if (++m_counter >= m_divider) {
		m_counter = 0;

		uint8_t usictl0 = m_mem->getByte(m_usictl);
		uint8_t usictl1 = m_mem->getByte(m_usictl + 1);

		// Logic is held in reset state
		if (usictl0 & 1) {
			return;
		}

		// I2C
		if (usictl1 & (1 << 6)) {
			
		}
		else {
			handleTickSPI(true, usictl0, usictl1);
		}
	}
}

void USI::tickFalling() {
	if (m_counter == (m_divider >> 2)) {
		uint8_t usictl0 = m_mem->getByte(m_usictl);
		uint8_t usictl1 = m_mem->getByte(m_usictl + 1);

		// Logic is held in reset state
		if (usictl0 & 1) {
			return;
		}

		// I2C
		if (usictl1 & (1 << 6)) {
			
		}
		else {
			handleTickSPI(false, usictl0, usictl1);
		}
	}
}

void USI::handleSignal(const std::string &name, double value) {
	if (value > 1.5) {
		tickRising();
	}
	else {
		tickFalling();
	}
}

void USI::reset() {
	if (m_source) {
		m_source->removeHandler(this);
	}
	m_source = m_aclk;
	m_source->addHandler(this, Clock::RisingFalling);

	// Set default values
	m_mem->setByte(m_usictl, 1);
	m_mem->setByte(m_usictl + 1, 1);
}

void USI::generateOutput(std::vector<PinMultiplexer *> &mpxs, bool value) {
	for (std::vector<PinMultiplexer *>::iterator it = mpxs.begin(); it != mpxs.end(); ++it) {
// 		std::cout << "outputing\n";
		(*it)->generateOutput(this, value ? 3.0 : 0.0);
	}
}

void USI::maybeOutputMSB() {
	uint8_t usictl0 = m_mem->getByte(m_usictl);
	uint8_t usictl1 = m_mem->getByte(m_usictl + 1);
	// if USICKPH ==1, output is enabled and we are not in reset state,
	// MSB/LSB should be visible on SDO right when we load it.
	if (usictl1 & (1 << 7) && usictl0 & 2 && !(usictl0 & 1)) {
		uint8_t usicnt = m_mem->getByte(m_usicctl + 1, false);
		doSPIOutput(usictl0, usictl1, usicnt);
	}
}

void USI::handleMemoryChanged(::Memory *memory, uint16_t address) {
	// USICKCTL - Clock Control
	if (address == m_usicctl) {
		uint8_t val = m_mem->getByte(address, false);

		// divider
		m_divider = 1 << ((val >> 5) & 7);

		if (m_source) {
			m_source->removeHandler(this);
		}

		m_pinManager->removeSignalHandler("TA0.0", this);
		m_pinManager->removeSignalHandler("TA0.1", this);
		m_pinManager->removeSignalHandler("TA0.2", this);

		// source
		switch((val >> 2) & 7) {
			case 0:
				// TODO: SCLK
				m_source = 0;
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
				m_source = 0;
				m_pinManager->addSignalHandler("TA0.0", this);
				break;
			case 6:
				m_source = 0;
				m_pinManager->addSignalHandler("TA0.1", this);
				break;
			case 7:
				m_source = 0;
				m_pinManager->addSignalHandler("TA0.2", this);
				break;
		}

		if (m_source) {
			m_source->addHandler(this, Clock::RisingFalling);
		}

		// clock polarity (USICKPL)
		bool usickpl = val & 2;
		if (usickpl != m_usickpl) {
			m_usickpl = usickpl;

			uint8_t usictl0 = m_mem->getByte(m_usictl);
			if ((usictl0 & (1 << 5))) {
				generateOutput(m_sclkMpx, m_sclk != m_usickpl);
			}
		}
	}
	else if (address == m_usisr) {
		maybeOutputMSB();
	}
	else if (address == m_usictl) {
		maybeOutputMSB();
	}
	else if (address == m_usicctl + 1) {
		uint8_t val = m_mem->getByte(address, false);
		if ((val & 31) == 0) {
			uint8_t usictl1 = m_mem->getByte(m_usicctl + 1);
			m_mem->setByte(m_usictl + 1, usictl1 | 1);
			m_intManager->queueInterrupt(m_variant->getUSI_VECTOR());
		}
		else if ((val & 32) == 0) {
			// USIIFGCC is 0, so USIIFG is cleared
			uint8_t usictl1 = m_mem->getByte(m_usicctl + 1);
			m_mem->setBit(m_usictl + 1, 1, false);
		}
	}
}

void USI::handleInterruptFinished(InterruptManager *intManager, int vector) {

}

void USI::handleMemoryRead(::Memory *memory, uint16_t address, uint16_t &value) {

}


void USI::handlePinInput(const std::string &name, double value) {
	if (name == "SDI") {
		m_input = value > 1.5;
// 		std::cout << "SDI input " << value << "\n";
		return;
	}

	if (name == "SCLK") {
		uint8_t usictl0 = m_mem->getByte(m_usictl);
		// SCLK pin not enabled
		if (!(usictl0 & (1 << 5))) {
			return;
		}

		uint8_t usictl1 = m_mem->getByte(m_usictl + 1);

		// I2C
		if (usictl0 & (1 << 6)) {
			
		}
		else {
			handleTickSPI(value > 1.5, usictl0, usictl1);
		}
		return;
	}
}

void USI::handlePinActivated(const std::string &name) {
	
}

void USI::handlePinDeactivated(const std::string &name) {
	
}

}
