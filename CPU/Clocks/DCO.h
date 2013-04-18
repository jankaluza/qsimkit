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

class DCO : public Clock, public MemoryWatcher {
	public:
		DCO(Memory *mem, Variant *variant);
		virtual ~DCO();

		void handleMemoryChanged(Memory *memory, uint16_t address);

		virtual void reset();

		virtual unsigned long getFrequency() {
			return m_freq;
		}

		double getStep();

	private:
		Memory *m_mem;
		Variant *m_variant;
		unsigned long m_freq;
		double m_step;
};
