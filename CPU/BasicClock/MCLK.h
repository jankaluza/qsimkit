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

#include <stdint.h>
#include <string>
#include <vector>
#include "CPU/Memory/Memory.h"

class Variant;

namespace MCU {

class DCO;
class VLO;
class LFXT1;
class Clock;

class MCLK : public Clock, public MemoryWatcher {
	public:
		MCLK(Memory *mem, Variant *variant, DCO *dco, VLO *vlo, LFXT1 *lfxt1);
		virtual ~MCLK();

		void handleMemoryChanged(Memory *memory, uint16_t address);

		void reset();

		unsigned long getFrequency();

		double getStep();

	private:
		Memory *m_mem;
		Variant *m_variant;
		Clock *m_source;
		DCO *m_dco;
		VLO *m_vlo;
		LFXT1 *m_lfxt1;
		uint8_t m_divider;
};

}
