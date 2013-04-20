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

#include "adevs.h"

namespace MCU {

class Event {
	public:
		Event() {}
		virtual ~Event() {}
};

class Memory;
class RegisterSet;
class InstructionDecoder;
class Instruction;

class CPU : public adevs::Atomic<Event *> {
	public:
		CPU();
		virtual ~CPU();

		bool loadA43(const std::string &data);

		/// Internal transition function.
		void delta_int();

		/// Handles external changes (change on PINs or interrupts)
		void delta_ext(double e, const adevs::Bag<Event *>& xb);

		/// Confluent transition function.
		void delta_conf(const adevs::Bag<Event *>& xb);

		/// Output function.
		void output_func(adevs::Bag<Event *>& yb);

		/// Time advance function.
		double ta();

		/// Output value garbage collection.
		void gc_output(adevs::Bag<Event *>& g);

	private:
		double m_cycles;
		double m_instructionCycles;

		Memory *m_mem;
		RegisterSet *m_reg;
		InstructionDecoder *m_decoder;
		Instruction *m_instruction;
};

}
