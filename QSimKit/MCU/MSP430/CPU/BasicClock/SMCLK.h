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

#include "Clock.h"
#include "Oscillator.h"

#include <stdint.h>
#include <string>
#include <vector>
#include "CPU/Memory/Memory.h"

class Variant;

namespace MSP430 {

class DCO;
class XT2;
class Oscillator;

class SMCLK : public Clock, public OscillatorHandler, public MemoryWatcher {
	public:
		SMCLK(Memory *mem, Variant *variant, DCO *dco, XT2 *xt2);
		virtual ~SMCLK();

		void handleMemoryChanged(::Memory *memory, uint16_t address);

		void reset();

		void tickRising();
		void tickFalling();

		void start();
		void pause();

		unsigned long getFrequency();

		std::string getSourceName();

	private:
		Memory *m_mem;
		Variant *m_variant;
		Oscillator *m_source;
		DCO *m_dco;
		XT2 *m_xt2;
		uint8_t m_divider;
		uint8_t m_counter;
		bool m_rising;
		bool m_running;
};

}
