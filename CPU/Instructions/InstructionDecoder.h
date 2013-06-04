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

#include "CPU/Instructions/InstructionManager.h"

namespace MSP430 {

class RegisterSet;
class Memory;
class InstructionArgument;
class Instruction;
class MemoryArgument;
class ConstantArgument;
class IndexedArgument;
class IndirectAutoincrementArgument;

class InstructionDecoder {
	public:
		InstructionDecoder(RegisterSet *reg, Memory *mem);
		virtual ~InstructionDecoder();

		int decodeCurrentInstruction(Instruction *instruction);

	private:
		InstructionArgument *getSourceArg(int &cycles, uint16_t &pc, bool bw, uint8_t as, uint8_t source_reg);
		InstructionArgument *getDestArg(int &cycles, uint16_t &pc, bool bw, uint8_t ad, uint8_t dest_reg);

	private:
		RegisterSet *m_reg;
		Memory *m_mem;
		MemoryArgument *m_srcMemArg;
		ConstantArgument *m_srcConstArg;
		IndexedArgument *m_srcIndexedArg;
		IndirectAutoincrementArgument *m_srcIndirectAutoArg;
		MemoryArgument *m_dstMemArg;
		IndexedArgument *m_dstIndexedArg;
};

}
