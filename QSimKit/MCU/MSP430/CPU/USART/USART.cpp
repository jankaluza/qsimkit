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

#include "USART.h"
#include "CPU/Variants/Variant.h"
#include "CPU/Memory/Memory.h"
#include "CPU/Interrupts/InterruptManager.h"
#include "CPU/Pins/PinManager.h"
#include "CPU/Pins/PinMultiplexer.h"
#include "CPU/BasicClock/ACLK.h"
#include "CPU/BasicClock/SMCLK.h"
#include <iostream>

namespace MSP430 {

#define IS_MASTER(CTL) ((CTL & (1 << 1)) != 0)

USART::USART(PinManager *pinManager, InterruptManager *intManager, Memory *mem, Variant *variant,
		   uint8_t id, ACLK *aclk, SMCLK *smclk) :
m_pinManager(pinManager), m_intManager(intManager), m_mem(mem), m_variant(variant), m_source(0),
m_divider(1), m_aclk(aclk), m_smclk(smclk),
m_counter(0), m_rising(0), m_sclk(false), m_usickpl(false), m_input(false),
m_output(false), m_transmitting(false), m_txReady(false), m_rxRead(false) {

	if (id == 0) {
		m_ctl = variant->getU0CTL();
		m_tctl = variant->getU0TCTL();
		m_rctl = variant->getU0RCTL();
		m_mctl = variant->getU0MCTL();
		m_br0 = variant->getU0BR0();
		m_br1 = variant->getU0BR1();;
		m_rxbuf = variant->getU0RXBUF();
		m_txbuf = variant->getU0TXBUF();
		m_ifg = variant->getU0IFG();
		m_ie = variant->getU0IE();
		m_me = variant->getU0ME();
		m_rxvect = variant->getUSART0RX_VECTOR();
		m_txvect = variant->getUSART0TX_VECTOR();
		m_utxie = variant->getUTXIE0();
		m_urxie = variant->getURXIE0();
		m_uspie = variant->getUSPIE0();
		m_utxifg = variant->getUTXIFG0();
		m_urxifg = variant->getURXIFG0();
		m_somiMpx = m_pinManager->addPinHandler("SOMI0", this);
		m_simoMpx = m_pinManager->addPinHandler("SIMO0", this);
		m_clkMpx = m_pinManager->addPinHandler("UCLK0", this);
		m_steMpx = m_pinManager->addPinHandler("STE0", this);
	}
	else {
		m_ctl = variant->getU1CTL();
		m_tctl = variant->getU1TCTL();
		m_rctl = variant->getU1RCTL();
		m_mctl = variant->getU1MCTL();
		m_br0 = variant->getU1BR0();
		m_br1 = variant->getU1BR1();;
		m_rxbuf = variant->getU1RXBUF();
		m_txbuf = variant->getU1TXBUF();
		m_ifg = variant->getU1IFG();
		m_ie = variant->getU1IE();
		m_me = variant->getU1ME();
		m_rxvect = variant->getUSART1RX_VECTOR();
		m_txvect = variant->getUSART1TX_VECTOR();
		m_utxie = variant->getUTXIE1();
		m_urxie = variant->getURXIE1();
		m_uspie = variant->getUSPIE1();
		m_utxifg = variant->getUTXIFG1();
		m_urxifg = variant->getURXIFG1();
		m_somiMpx = m_pinManager->addPinHandler("SOMI1", this);
		m_simoMpx = m_pinManager->addPinHandler("SIMO1", this);
		m_clkMpx = m_pinManager->addPinHandler("UCLK1", this);
		m_steMpx = m_pinManager->addPinHandler("STE1", this);
	}

	m_mem->addWatcher(m_tctl, this);
	m_mem->addWatcher(m_br0, this);
	m_mem->addWatcher(m_br1, this);
	m_mem->addWatcher(m_txbuf, this);
	m_mem->addWatcher(m_rxbuf, this, MemoryWatcher::Read);

	reset();
}

USART::~USART() {

}

void USART::doSPICapture(uint8_t ctl) {
// 	std::cout << "CAPTURE\n";
// 	std::cout << "capture\n";
	// ONLY MSB for USART
	if (true) {
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
		if (ctl & (1 << 4)) {
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
			m_mem->setBit(m_rctl, (1 << 5), true);
		}
		m_rxRead = false;

		if (m_mem->getByte(m_ie, false) & m_urxie) {
			m_mem->setByte(m_ifg, m_mem->getByte(m_ifg, false) | m_urxifg, false);
			m_intManager->queueInterrupt(m_rxvect);
		}

		// if there is another byte ready, start tranmission again
		m_transmitting = false;
		if (m_txReady) {
			txReady();
		}

		// We won't be transmitting anything, so tx is empty
		if (!m_transmitting) {
			m_mem->setBit(m_tctl, 1, true);
		}
	}
}

void USART::doSPIOutput(uint8_t ctl) {
// 	std::cout << "output\n";
	// Only MSB in USART
	if (true) {
		// MSB
		// Check if we are working in 8 bit mode
		if ((ctl & (1 << 4)) == 0) {
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

	std::cout << "OUTPUT " << m_output << " buf=" << (uint16_t) m_tx << "\n";
	generateOutput(m_simoMpx, m_output);

	if (m_cnt == 0 || m_cnt == 1) {
		// generate interrupt
		std::cout << "CAN GENERATE INTERRUPT\n";
		if (m_mem->getByte(m_ie, false) & m_utxie) {
			std::cout << "GENERATE INTERRUPT\n";
			m_mem->setByte(m_ifg, m_mem->getByte(m_ifg, false) | m_utxifg, false);
			m_intManager->queueInterrupt(m_txvect);
		}
	}
}

void USART::generateOutput(std::vector<PinMultiplexer *> &mpxs, bool value) {
	for (std::vector<PinMultiplexer *>::iterator it = mpxs.begin(); it != mpxs.end(); ++it) {
		(*it)->generateOutput(this, value ? 3.0 : 0.0);
	}
}

void USART::handleFirstEdgeSPI(uint8_t ctl) {
	uint8_t tctl = m_mem->getByte(m_tctl, false);

	// Check UCCKPH:
	if (tctl & (1 << 7)) {
		doSPICapture(ctl);
	}
	else {
		doSPIOutput(ctl);
	}
}

void USART::handleSecondEdgeSPI(uint8_t ctl) {
	uint8_t tctl = m_mem->getByte(m_tctl, false);

	// Check UCCKPH:
	if (tctl & (1 << 7)) {
		doSPIOutput(ctl);
	}
	else {
		doSPICapture(ctl);
	}
}

void USART::handleTickSPI(bool rising, uint8_t ctl) {
// 	uint8_t ctl1 = m_mem->getByte(m_ctl1, false);

	// Logic is held in reset state
	if (ctl & 1) {
		return;
	}

	bool first_edge;
	if (IS_MASTER(ctl)) {
		// Master does not take care of ckpl
		first_edge = rising;
	}
	else {
		// Rising is first edge when m_usickpl == !rising, otherwise
		// it's second edge
		first_edge = m_usickpl == !rising;
	}

	if (first_edge) {
		handleFirstEdgeSPI(ctl);
	}
	else {
		handleSecondEdgeSPI(ctl);
	}

	// Master generates output clock
	if (IS_MASTER(ctl)) {
		m_sclk = rising;
		generateOutput(m_clkMpx, m_sclk != m_usickpl);
	}
}

void USART::tickRising() {
	if (++m_counter >= (m_divider >> 1)) {
		m_counter = 0;

		uint8_t ctl = m_mem->getByte(m_ctl, false);

		// We are slave, do not handle ticks from our own CLK
		if (!IS_MASTER(ctl)) {
			return;
		}

		// we are not transmitting
		if (!m_transmitting) {
			return;
		}

		m_rising = !m_rising;
		handleTickSPI(m_rising, ctl);
	}
}

void USART::tickFalling() {
	// Ignore falling tick if divider is one, because falling tick is generated
	// on second rising tick in this case.
	if (m_divider != 1) {
		return;
	}

	tickRising();
}

void USART::handleSignal(const std::string &name, double value) {
	if (value > 1.5) {
		tickRising();
	}
	else {
		tickFalling();
	}
}

void USART::reset() {
	if (m_source) {
		m_source->removeHandler(this);
	}
	m_source = m_aclk;
	m_source->addHandler(this, Clock::Rising);

	// Set default values
	m_mem->setByte(m_ctl, 1);
	m_mem->setByte(m_tctl, 1);
}

void USART::txReady() {
	// We are transmitting, so postpone moving to m_tx until the transmition
	// finishes
	if (m_transmitting) {
		m_txReady = true;
		return;
	}

	// USPIE is not enabled, so don't start transmition
	if (m_mem->getByte(m_me, false) & m_uspie == 0) {
		// TODO: Should we do anything with txReady here?
		return;
	}

	std::cout << "STARTING TRANSMITION\n";

	// There is no transmission in progress, so just move data into m_tx and
	// start the transmission
	m_tx = m_mem->getByte(m_txbuf, false);
	m_txReady = false;
	m_transmitting = true;

	// Set TXEPT to 0, because we are transmitting and tx is not empty
	m_mem->setBit(m_tctl, 1, false);

	// 7-bit vs. 8-bit transmit
	uint8_t ctl = m_mem->getByte(m_ctl, false);
	if ((ctl & (1 << 4)) == 0) {
		m_cnt = 7;
	}
	else {
		m_cnt = 8;
	}
}

void USART::handleMemoryChanged(::Memory *memory, uint16_t address) {
// 	std::cout << "\n mem changed = " << address << " " << m_txbuf << "\n";
	if (address == m_tctl) {
		uint8_t val = m_mem->getByte(address, false);

		if (m_source) {
			m_source->removeHandler(this);
		}

		// source
		switch((val >> 4) & 3) {
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
		m_mem->setBit(m_ifg, m_utxifg, false);
		txReady();
	}
}

void USART::handleInterruptFinished(InterruptManager *intManager, int vector) {

}

void USART::handleMemoryRead(::Memory *memory, uint16_t address, uint8_t &value) {
	std::cout << "READ RXBUF " << (int) m_ifg << " " << (int) m_urxifg << "\n";
	// Clear UCOE
	m_mem->setBit(m_rctl, (1 << 5), false);

	// Clear interrup flag
	m_mem->setBit(m_ifg, m_urxifg, false);

	m_rxRead = true;
}


void USART::handlePinInput(const std::string &name, double value) {
	switch(name[1]) {
		// S'O'MI
		case 'O':
		// S'I'MO
		case 'I':
			m_input = value > 1.5;
			return;
		// C'L'K
		case 'L' :
			handleTickSPI(value > 1.5, m_mem->getByte(m_ctl, false));
			return;
		default:
			break;
	}
}

void USART::handlePinActivated(const std::string &name) {
	
}

void USART::handlePinDeactivated(const std::string &name) {
	
}

}
