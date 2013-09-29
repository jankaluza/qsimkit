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

#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include "CPU/Memory/Memory.h"
#include "CPU/Interrupts/InterruptManager.h"
#include "CPU/Pins/PinHandler.h"
#include "CPU/Pins/SignalHandler.h"
#include "CPU/BasicClock/Clock.h"

class Variant;

namespace MSP430 {

class ACLK;
class SMCLK;
class Clock;
class InterruptManager;
class PinManager;
class PinMultiplexer;

class USCI : public ClockHandler, public MemoryWatcher, public InterruptWatcher, public PinHandler, public SignalHandler {
	public:
		typedef enum {USCI_A, USCI_B} Type;

		USCI(PinManager *pinManager, InterruptManager *intManager, Memory *mem,
			 Variant *variant, Type type, uint8_t id, ACLK *aclk, SMCLK *smclk);
		virtual ~USCI();

		void handleMemoryChanged(::Memory *memory, uint16_t address);
		void handleMemoryRead(::Memory *memory, uint16_t address, uint16_t &value);

		void handleInterruptFinished(InterruptManager *intManager, int vector);

		void handlePinInput(const std::string &name, double value);

		void handlePinActivated(const std::string &name);

		void handlePinDeactivated(const std::string &name);

		void handleSignal(const std::string &name, double value);

		void tickRising();
		void tickFalling();

		void reset();

	private:
		void doSPICapture(uint8_t ctl0);
		void doSPIOutput(uint8_t ctl0);
		void handleTickSPI(bool rising, uint8_t ctl0);
		void handleFirstEdgeSPI(uint8_t ctl0);
		void handleSecondEdgeSPI(uint8_t ctl0);
		void generateOutput(std::vector<PinMultiplexer *> &mpxs, bool value);
		void txReady();

	private:
		PinManager *m_pinManager;
		InterruptManager *m_intManager;
		Memory *m_mem;
		Variant *m_variant;
		Clock *m_source;
		uint16_t m_divider;
		ACLK *m_aclk;
		SMCLK *m_smclk;
		uint16_t m_ctl0;
		uint16_t m_ctl1;
		uint16_t m_br0;
		uint16_t m_br1;
		uint16_t m_mctl;
		uint16_t m_stat;
		uint16_t m_rxbuf;
		uint16_t m_txbuf;
		uint16_t m_rxvect;
		uint16_t m_txvect;
		uint16_t m_ifg;
		uint16_t m_ie;
		uint16_t m_counter;
		std::vector<PinMultiplexer *> m_somiMpx;
		std::vector<PinMultiplexer *> m_simoMpx;
		std::vector<PinMultiplexer *> m_clkMpx;
		std::vector<PinMultiplexer *> m_steMpx;
		bool m_sclk;
		bool m_usickpl;
		bool m_input;
		bool m_output;
		bool m_transmitting;
		bool m_txReady;
		uint8_t m_tx;
		uint8_t m_rx;
		uint8_t m_cnt;
		Type m_type;
};

}
