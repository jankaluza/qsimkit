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

class Variant;

namespace MSP430 {

class Memory;
class RegisterSet;
class InstructionDecoder;
class Instruction;
class InterruptManager;
class DCO;
class MCLK;
class VLO;
class ACLK;
class SMCLK;
class LFXT1;
class XT2;
class Timer;
class TimerFactory;
class PinManager;
class ClockPinHandler;

/// Implements and includes all modules from Basic Clock Module.
class BasicClock {
	public:
		BasicClock(Memory *mem, Variant *variant, InterruptManager *intManager,
				   PinManager *pinManager, TimerFactory *factory);
		virtual ~BasicClock();

		void reset();

		DCO *getDCO() {
			return m_dco;
		}

		VLO *getVLO() {
			return m_vlo;
		}

		LFXT1 *getLFXT1() {
			return m_lfxt1;
		}

		XT2 *getXT2() {
			return m_xt2;
		}

		MCLK *getMCLK() {
			return m_mclk;
		}

		Timer *getTimerA() {
			return m_timerA0;
		}

		Timer *getTimerA1() {
			return m_timerA1;
		}

		Timer *getTimerB() {
			return m_timerB;
		}

		ACLK *getACLK() {
			return m_aclk;
		}

		SMCLK *getSMCLK() {
			return m_smclk;
		}

	private:
		Memory *m_mem;
		Variant *m_variant;
		DCO *m_dco;
		MCLK *m_mclk;
		VLO *m_vlo;
		ACLK *m_aclk;
		SMCLK *m_smclk;
		LFXT1 *m_lfxt1;
		XT2 *m_xt2;
		Timer *m_timerA0;
		Timer *m_timerA1;
		Timer *m_timerB;
		InterruptManager *m_intManager;
		TimerFactory *m_factory;
		PinManager *m_pinManager;
		ClockPinHandler *m_smclkHandler;
		ClockPinHandler *m_mclkHandler;
		ClockPinHandler *m_aclkHandler;
};

}
