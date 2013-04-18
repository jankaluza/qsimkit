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
class ACLK;
class SMCLK;
class Clock;
class InterruptManager;

class Timer : public Clock, public MemoryWatcher {
	public:
		Timer(InterruptManager *intManager, Memory *mem, Variant *variant,
			  ACLK *aclk, SMCLK *smclk);
		virtual ~Timer();

		void handleMemoryChanged(Memory *memory, uint16_t address);

		void tick();

		void reset();

		unsigned long getFrequency();

		double getStep();

	private:
		InterruptManager *m_intManager;
		Memory *m_mem;
		Variant *m_variant;
		Clock *m_source;
		ACLK *m_aclk;
		SMCLK *m_smclk;
};
