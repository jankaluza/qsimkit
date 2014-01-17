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
		   Type type, uint8_t id, ACLK *aclk, SMCLK *smclk) :
m_pinManager(pinManager), m_intManager(intManager), m_mem(mem), m_variant(variant), m_source(0),
m_divider(1), m_aclk(aclk), m_smclk(smclk),
m_counter(0), m_rising(false), m_sclk(false), m_usickpl(false), m_input(false),
m_output(false), m_transmitting(false), m_txReady(false), m_type(type), m_rxRead(false) {

	std::string prefix;

	switch(type) {
		case USCI_A:
			if (id == 0) {
				m_ctl0 = variant->getUCA0CTL0();
				m_ctl1 = variant->getUCA0CTL1();
				m_br0 = variant->getUCA0BR0();
				m_br1 = variant->getUCA0BR1();
				m_mctl = variant->getUCA0MCTL();
				m_stat = variant->getUCA0STAT();
				m_rxbuf = variant->getUCA0RXBUF();
				m_txbuf = variant->getUCA0TXBUF();
				prefix = "UCA0";
			}
			else {
				m_ctl0 = variant->getUCA1CTL0();
				m_ctl1 = variant->getUCA1CTL1();
				m_br0 = variant->getUCA1BR0();
				m_br1 = variant->getUCA1BR1();
				m_mctl = variant->getUCA1MCTL();
				m_stat = variant->getUCA1STAT();
				m_rxbuf = variant->getUCA1RXBUF();
				m_txbuf = variant->getUCA1TXBUF();
				prefix = "UCA1";
			}
			break;
		case USCI_B:
			if (id == 0) {
				m_ctl0 = variant->getUCB0CTL0();
				m_ctl1 = variant->getUCB0CTL1();
				m_br0 = variant->getUCB0BR0();
				m_br1 = variant->getUCB0BR1();
				m_mctl = variant->getUCB0MCTL();
				m_stat = variant->getUCB0STAT();
				m_rxbuf = variant->getUCB0RXBUF();
				m_txbuf = variant->getUCB0TXBUF();
				prefix = "UCB0";
			}
			else {
				m_ctl0 = variant->getUCB1CTL0();
				m_ctl1 = variant->getUCB1CTL1();
				m_br0 = variant->getUCB1BR0();
				m_br1 = variant->getUCB1BR1();
				m_mctl = 0;
				m_stat = variant->getUCB1STAT();
				m_rxbuf = variant->getUCB1RXBUF();
				m_txbuf = variant->getUCB1TXBUF();
				prefix = "UCB1";
			}
			break;
	};

	if (id == 0) {
		m_rxvect = variant->getUSCIAB0RX_VECTOR();
		m_txvect = variant->getUSCIAB0TX_VECTOR();
		m_ifg = variant->getUC0IFG();
		m_ie = variant->getUC0IE();
	}
	else {
		m_rxvect = variant->getUSCIAB1RX_VECTOR();
		m_txvect = variant->getUSCIAB1TX_VECTOR();
		m_ifg = variant->getUC1IFG();
		m_ie = variant->getUC1IE();
	}

	m_mem->addWatcher(m_ctl0, this);
	m_mem->addWatcher(m_ctl1, this);
	m_mem->addWatcher(m_br0, this);
	m_mem->addWatcher(m_br1, this);
	m_mem->addWatcher(m_txbuf, this);
	m_mem->addWatcher(m_rxbuf, this, MemoryWatcher::Read);

	m_somiMpx = m_pinManager->addPinHandler(prefix + "SOMI", this);
	m_simoMpx = m_pinManager->addPinHandler(prefix + "SIMO", this);
	m_clkMpx = m_pinManager->addPinHandler(prefix + "CLK", this);
	m_steMpx = m_pinManager->addPinHandler(prefix + "STE", this);

	reset();
}

USCI::~USCI() {

}

void USCI::doSPICapture(uint8_t ctl0) {
// 	std::cout << "CAPTURE\n";
// 	std::cout << "capture\n";
	// Check LSB vs. MSB
	if (ctl0 & (1 << 5)) {
		m_rx = m_rx << 1;
		// MSB mode
		std::cout << "MSB CAPTURE " << m_input << "\n";
		if (m_input) {
			m_rx |= 1;
		}
		else {
			m_rx &= ~(1);
		}
	}
	else {
		// LSB mode
		m_rx = m_rx >> 1;
		if (ctl0 & (1 << 4)) {
			// 7 bit mode
			if (m_input) {
				m_rx |= (1 << 6);
			}
			else {
				m_rx &= ~(1 << 6);
			}
			m_rx &= 127;
		}
		else {
			// 8 bit mode
			if (m_input) {
				m_rx |= (1 << 7);
			}
			else {
				m_rx &= ~(1 << 7);
			}
		}
	}

	std::cout << "RXBUF = " << (uint16_t) m_rx << "\n";
	m_cnt--;
	if (m_cnt == 0) {
		std::cout << "FINAL RXBUF = " << (uint16_t) m_rx << "\n";
		m_mem->setByte(m_rxbuf, m_rx, false);

		// Set UCOE (overflow) bit if RXBUF was not read
		if (!m_rxRead) {
			m_mem->setBit(m_stat, (1 << 5), true);
		}
		m_rxRead = false;

		// generate interrupt
		if (m_type == USCI_A) {
			if (m_mem->getByte(m_ie, false) & 1) {
				m_mem->setByte(m_ifg, m_mem->getByte(m_ifg, false) | 1, false);
				m_intManager->queueInterrupt(m_rxvect);
			}
		}
		else {
			if (m_mem->getByte(m_ie, false) & 4) {
				m_mem->setByte(m_ifg, m_mem->getByte(m_ifg, false) | 4, false);
				m_intManager->queueInterrupt(m_rxvect);
			}
		}

		// if there is another byte ready, start tranmission again
		m_transmitting = false;
		if (m_txReady) {
			txReady();
		}

		// Clear UCBUSY in case we won't transmit anything now
		if (!m_transmitting) {
			m_mem->setBit(m_stat, 1, false);
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
			m_output = m_tx & (1 << 6);
		}
		else {
// 			std::cout << "8BIT\n";
			m_output = m_tx & (1 << 7);
		}
		m_tx = m_tx << 1;
	}
	else {
		// Load LSB bit
		m_output = m_tx & 1;
		m_tx = m_tx >> 1;
	}

// 	std::cout << "OUTPUT " << m_output << " buf=" << (uint16_t) m_tx << "\n";
	generateOutput(m_simoMpx, m_output);

	if (m_cnt == 0 || m_cnt == 1) {
		// generate interrupt
		if (m_type == USCI_A) {
			if (m_mem->getByte(m_ie, false) & 2) {
				m_mem->setByte(m_ifg, m_mem->getByte(m_ifg, false) | 2, false);
				m_intManager->queueInterrupt(m_txvect);
			}
		}
		else {
			if (m_mem->getByte(m_ie, false) & 8) {
				m_mem->setByte(m_ifg, m_mem->getByte(m_ifg, false) | 8, false);
				m_intManager->queueInterrupt(m_txvect);
			}
		}
	}
}

void USCI::generateOutput(std::vector<PinMultiplexer *> &mpxs, bool value) {
	for (std::vector<PinMultiplexer *>::iterator it = mpxs.begin(); it != mpxs.end(); ++it) {
// 		std::cout << "outputing\n";
		(*it)->generateOutput(this, value ? 3.0 : 0.0);
	}
}

void USCI::handleFirstEdgeSPI(uint8_t ctl0) {
// 	std::cout << "FIRST EDGE\n";
	// Check UCCKPH:
	if (ctl0 & (1 << 7)) {
		doSPICapture(ctl0);
	}
	else {
		doSPIOutput(ctl0);
	}
}

void USCI::handleSecondEdgeSPI(uint8_t ctl0) {
// 	std::cout << "SECOND EDGE << " << (uint16_t) ctl0 << " " << (ctl0 & (1 << 7)) << "\n";
	// Check UCCKPH:
	if (ctl0 & (1 << 7)) {
		doSPIOutput(ctl0);
	}
	else {
		doSPICapture(ctl0);
	}
}

void USCI::handleTickSPI(bool rising, uint8_t ctl0) {
	uint8_t ctl1 = m_mem->getByte(m_ctl1, false);

	// Logic is held in reset state
	if (ctl1 & 1) {
		return;
	}

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
	if (++m_counter >= (m_divider >> 1)) {
		m_counter = 0;

		uint8_t ctl0 = m_mem->getByte(m_ctl0, false);

		// We are slave, do not handle ticks from our own CLK
		if ((ctl0 & (1 << 3)) == 0) {
			return;
		}

		// we are not transmitting
		if (!m_transmitting) {
			return;
		}

		m_rising = !m_rising;
		handleTickSPI(m_rising, ctl0);
	}
}

void USCI::tickFalling() {
	// Ignore falling tick if divider is one, because falling tick is generated
	// on second rising tick in this case.
	if (m_divider != 1) {
		return;
	}

	tickRising();
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

	std::cout << "STARTING TRANSMITION\n";

	// There is no transmission in progress, so just move data into m_tx and
	// start the transmission
	m_tx = m_mem->getByte(m_txbuf, false);
	m_txReady = false;
	m_transmitting = true;

	// Set UCBUSY flag
	m_mem->setBit(m_stat, 1, true);

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
		m_divider = m_mem->getByte(m_br0, false) + m_mem->getByte(m_br1, false) * 256;
		m_counter = m_divider;
	}
	else if (address == m_txbuf) {
		std::cout << "user wrote to TXBUF\n";

		// clear interrup flag
		if (m_type == USCI_A) {
			m_mem->setBit(m_ifg, 2, false);
		}
		else {
			m_mem->setBit(m_ifg, 8, false);
		}

		txReady();
	}
}

void USCI::handleInterruptFinished(InterruptManager *intManager, int vector) {

}

void USCI::handleMemoryRead(::Memory *memory, uint16_t address, uint8_t &value) {
	std::cout << "READ RXBUF\n";
	// Clear UCOE
	m_mem->setBit(m_stat, (1 << 5), false);

	// Clear interrup flag
	if (m_type == USCI_A) {
		m_mem->setBit(m_ifg, 1, false);
	}
	else {
		m_mem->setBit(m_ifg, 4, false);
	}

	m_rxRead = true;
}


void USCI::handlePinInput(const std::string &name, double value) {
	switch(name[5]) {
		// UCA0S'O'MI
		case 'O':
		// UCA0S'I'MO
		case 'I':
			m_input = value > 1.5;
			return;
		// UCA0C'L'K
		case 'L' :
			handleTickSPI(value > 1.5, m_mem->getByte(m_ctl0, false));
			return;
		default:
			break;
	}
}

void USCI::handlePinActivated(const std::string &name) {
	
}

void USCI::handlePinDeactivated(const std::string &name) {
	
}

}
