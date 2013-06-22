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
#include "CPU/BasicClock/Clock.h"

class Variant;

namespace MSP430 {

class ACLK;
class SMCLK;
class Clock;
class InterruptManager;
class PinManager;
class PinMultiplexer;

class USI : public ClockHandler, public MemoryWatcher, public InterruptWatcher, public PinHandler {
	public:
		USI(PinManager *pinManager, InterruptManager *intManager, Memory *mem, Variant *variant,
			  ACLK *aclk, SMCLK *smclk, uint16_t usictl, uint16_t usicctl, uint16_t usisr);
		virtual ~USI();

		void handleMemoryChanged(::Memory *memory, uint16_t address);
		void handleMemoryRead(::Memory *memory, uint16_t address, uint16_t &value);

		void handleInterruptFinished(InterruptManager *intManager, int vector);

		void handlePinInput(const std::string &name, double value);

		void handlePinActivated(const std::string &name);

		void handlePinDeactivated(const std::string &name);

		void tickRising();
		void tickFalling();

		void reset();

	private:
		PinManager *m_pinManager;
		InterruptManager *m_intManager;
		Memory *m_mem;
		Variant *m_variant;
		Clock *m_source;
		uint8_t m_divider;
		ACLK *m_aclk;
		SMCLK *m_smclk;
		uint16_t m_usictl;
		uint16_t m_usicctl;
		uint16_t m_usisr;
		uint16_t m_counter;
};

}
