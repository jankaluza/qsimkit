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
#include "CPU/Interrupts/InterruptManager.h"

class Variant;

namespace MCU {

class ACLK;
class SMCLK;
class Clock;
class InterruptManager;
class PinManager;

class Timer : public Clock, public MemoryWatcher, public InterruptWatcher {
	public:
		Timer(PinManager *pinManager, InterruptManager *intManager, Memory *mem, Variant *variant,
			  ACLK *aclk, SMCLK *smclk, uint16_t tactl, uint16_t tar,
			  uint16_t taiv);
		virtual ~Timer();

		void handleMemoryChanged(Memory *memory, uint16_t address);
		void handleMemoryRead(Memory *memory, uint16_t address, uint16_t &value);

		void handleInterruptFinished(InterruptManager *intManager, int vector);

		void addCCR(uint16_t tacctl, uint16_t taccr);

		void tick();

		void reset();

		unsigned long getFrequency();

		double getStep();

	private:
		void checkCCRInterrupts(uint16_t tar);
		void changeTAR(uint8_t mode);

		typedef struct {
			uint16_t tacctl;
			uint16_t taccr;
		} CCR;

		PinManager *m_pinManager;
		InterruptManager *m_intManager;
		Memory *m_mem;
		Variant *m_variant;
		Clock *m_source;
		uint8_t m_divider;
		ACLK *m_aclk;
		SMCLK *m_smclk;
		bool m_up;
		uint16_t m_tactl;
		uint16_t m_tar;
		uint16_t m_taiv;
		std::vector<CCR> m_ccr;
};

}
