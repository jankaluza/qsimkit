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

#include "USCI.h"
#include "CPU/Variants/Variant.h"
#include "CPU/Memory/Memory.h"
#include "CPU/Interrupts/InterruptManager.h"
#include "CPU/Pins/PinManager.h"
#include "CPU/Pins/PinMultiplexer.h"
#include "CPU/BasicClock/ACLK.h"
#include "CPU/BasicClock/SMCLK.h"
#include <iostream>

namespace MSP430 {

USCI::USCI(PinManager *pinManager, InterruptManager *intManager, Memory *mem, Variant *variant,
			 ACLK *aclk, SMCLK *smclk, uint16_t ctl0, uint16_t ctl1, uint16_t br0,
			  uint16_t br1, uint16_t mctl, uint16_t stat, uint16_t rxbuf, uint16_t txbuf) :
m_pinManager(pinManager), m_intManager(intManager), m_mem(mem), m_variant(variant), m_source(0),
m_divider(1), m_aclk(aclk), m_smclk(smclk),
m_ctl0(ctl0), m_ctl1(ctl1), m_br0(br0), m_br1(br1), m_mctl(mctl), m_stat(stat), m_rxbuf(rxbuf),
m_txbuf(txbuf), m_counter(0), m_sclk(false), m_usickpl(false), m_input(false),
m_output(false), m_transmitting(false), m_txReady(false) {

	m_mem->addWatcher(m_ctl0, this);
	m_mem->addWatcher(m_ctl1, this);
	m_mem->addWatcher(m_br0, this);
	m_mem->addWatcher(m_br1, this);
	m_mem->addWatcher(m_txbuf, this);
// 	m_mem->addWatcher(m_usicctl + 1, this);
// 	m_mem->addWatcher(m_usisr, this);
// 	m_mem->addWatcher(m_usictl, this);
// 
// 	m_sdiMpx = m_pinManager->addPinHandler("SDI", this);
// 	m_sdoMpx = m_pinManager->addPinHandler("SDO", this);
// 	m_sclkMpx = m_pinManager->addPinHandler("SCLK", this);

	reset();
}

USCI::~USCI() {

}

void USCI::doSPICapture(uint8_t ctl0) {
// 	std::cout << "capture\n";
	// Check LSB vs. MSB
	if (ctl0 & (1 << 5)) {
		// MSB mode
		if (m_input) {
			m_rx |= 1;
		}
		else {
			m_rx &= ~(1);
		}
		m_rx = m_rx << 1;
		m_rx &= 0xf7;
	}
	else {
		// LSB mode
		if (ctl0 & (1 << 4)) {
			// 7 bit mode
			if (m_input) {
				m_rx |= (1 << 7);
			}
			else {
				m_rx &= ~(1 << 7);
			}
		}
		else {
			// 8 bit mode
			if (m_input) {
				m_rx |= (1 << 8);
			}
			else {
				m_rx &= ~(1 << 8);
			}
			m_rx &= 0xff;
		}
		m_rx = m_rx >> 1;
	}

	m_cnt--;
	if (m_cnt == 0) {
// 		std::cout << "USISR = " << usisr << "\n";
		m_mem->setByte(m_rxbuf, m_rx, false);
// 		m_intManager->queueInterrupt(m_variant->getUSI_VECTOR());
		m_transmitting = false;
		// if there is another byte ready, start tranmission again
		if (m_txReady) {
			txReady();
		}

	}
}

void USCI::doSPIOutput(uint8_t ctl0) {
// 	std::cout << "output\n";
	// Check LSB vs. MSB
	if (ctl0 & (1 << 5)) {
		// MSB
		// Check if we are working in 8 bit mode
		if (ctl0 & (1 << 4)) {
			m_output = m_tx & (1 << 7);
		}
		else {
			m_output = m_tx & (1 << 8);
		}
		m_tx = m_tx << 1;
	}
	else {
		// Load LSB bit
		m_output = m_tx & 1;
		m_tx = m_tx >> 1;
	}

	generateOutput(m_simoMpx, m_output);
}

void USCI::generateOutput(std::vector<PinMultiplexer *> &mpxs, bool value) {
	for (std::vector<PinMultiplexer *>::iterator it = mpxs.begin(); it != mpxs.end(); ++it) {
// 		std::cout << "outputing\n";
		(*it)->generateOutput(this, value ? 3.0 : 0.0);
	}
}

void USCI::handleFirstEdgeSPI(uint8_t ctl0) {
	// Check UCCKPH:
	if (ctl0 & (1 << 7)) {
		doSPICapture(ctl0);
	}
	else {
		doSPIOutput(ctl0);
	}
}

void USCI::handleSecondEdgeSPI(uint8_t ctl0) {
	// Check UCCKPH:
	if (ctl0 & (1 << 7)) {
		doSPIOutput(ctl0);
	}
	else {
		doSPICapture(ctl0);
	}
}

void USCI::handleTickSPI(bool rising, uint8_t ctl0) {
	// Rising is first edge when m_usickpl == !rising, otherwise
	// it's second edge
	bool first_edge = m_usickpl == !rising;
	if (first_edge) {
		handleFirstEdgeSPI(ctl0);
	}
	else {
		handleSecondEdgeSPI(ctl0);
	}

	// Master generates output clock
	if (ctl0 & (1 << 3)) {
		m_sclk = rising;
		generateOutput(m_clkMpx, m_sclk != m_usickpl);
	}
}

void USCI::tickRising() {
	if (++m_counter >= m_divider) {
		m_counter = 0;

		uint8_t ctl0 = m_mem->getByte(m_ctl0, false);

		// Logic is held in reset state
		if (ctl0 & 1) {
			return;
		}

		// we are not transmitting
		if (!m_transmitting) {
			return;
		}

		handleTickSPI(true, ctl0);
	}
}

void USCI::tickFalling() {
	if (m_counter == (m_divider >> 2)) {
		uint8_t ctl0 = m_mem->getByte(m_ctl0, false);

		// Logic is held in reset state
		if (ctl0 & 1) {
			return;
		}

		// we are not transmitting
		if (!m_transmitting) {
			return;
		}

		handleTickSPI(false, ctl0);
	}
}

void USCI::handleSignal(const std::string &name, double value) {
	if (value > 1.5) {
		tickRising();
	}
	else {
		tickFalling();
	}
}

void USCI::reset() {
	if (m_source) {
		m_source->removeHandler(this);
	}
	m_source = m_aclk;
	m_source->addHandler(this, Clock::Rising);

	// Set default values
	m_mem->setByte(m_ctl1, 1);
}

void USCI::txReady() {
	// We are transmitting, so postpone moving to m_tx until the transmition
	// finishes
	if (m_transmitting) {
		m_txReady = true;
		return;
	}

	// There is no transmission in progress, so just move data into m_tx and
	// start the transmission
	m_tx = m_mem->getByte(m_txbuf, false);
	m_txReady = false;
	m_transmitting = true;

	// 7-bit vs. 8-bit transmit
	uint8_t ctl0 = m_mem->getByte(m_ctl0, false);
	if (ctl0 & (1 << 4)) {
		m_cnt = 7;
	}
	else {
		m_cnt = 8;
	}
}

void USCI::handleMemoryChanged(::Memory *memory, uint16_t address) {
	if (address == m_ctl1) {
		uint8_t val = m_mem->getByte(address, false);

		// divider
// 		m_divider = 1 << ((val >> 5) & 7);

		if (m_source) {
			m_source->removeHandler(this);
		}

		// source
		switch((val >> 6) & 3) {
			case 0:
				// N/A
				m_source = 0;
				break;
			case 1:
				m_source = m_aclk;
				break;
			case 2: case 3:
				m_source = m_smclk;
				break;
		}

		if (m_source) {
			m_source->addHandler(this, Clock::Rising);
		}


	}
	else if (address == m_ctl0) {
		uint8_t val = m_mem->getByte(address, false);

		// Check UC7BIT and if set, reset UCMSB
		if (val & (1 << 4)) {
			m_mem->setByte(address, val | (1 << 5), false);
		}

		// clock polarity (UCCKPL)
		bool usickpl = val & (1 << 6);
		if (usickpl != m_usickpl) {
			m_usickpl = usickpl;

			// TODO: When clock polarity changes, switch clock output
// 			uint8_t usictl0 = m_mem->getByte(m_usictl);
// 			if ((usictl0 & (1 << 5))) {
// 				generateOutput(m_sclkMpx, m_sclk != m_usickpl);
// 			}
		}
	}
	else if (address == m_br0 || address == m_br1) {
		m_divider = m_br0 + m_br1 * 256;
	}
	else if (address == m_txbuf) {
		txReady();
	}
}

void USCI::handleInterruptFinished(InterruptManager *intManager, int vector) {

}

void USCI::handleMemoryRead(::Memory *memory, uint16_t address, uint16_t &value) {

}


void USCI::handlePinInput(const std::string &name, double value) {

}

void USCI::handlePinActivated(const std::string &name) {
	
}

void USCI::handlePinDeactivated(const std::string &name) {
	
}

}
