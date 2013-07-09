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

#include "Oscillator.h"
#include "CPU/Memory/Memory.h"
#include "CPU/Pins/PinHandler.h"

#include <stdint.h>
#include <string>
#include <vector>

class Variant;

namespace MSP430 {

class PinManager;
class PinMultiplexer;

class LFXT1 : public Oscillator, public MemoryWatcher, public PinHandler {
	public:
		LFXT1(Memory *mem, Variant *variant, PinManager *pinManager);
		virtual ~LFXT1();

		void reset();
		unsigned long getFrequency() {
			return 0;
		}

		double getStep() {
			return 0;
		}

		bool isChosen();

		void handleMemoryChanged(::Memory *memory, uint16_t address);

		void handlePinInput(const std::string &name, double value);

		void handlePinActivated(const std::string &name);

		void handlePinDeactivated(const std::string &name);

	private:
		Memory *m_mem;
		Variant *m_variant;
		bool m_state;
		bool m_enabled;
};

}
