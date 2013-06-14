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

namespace MSP430 {

class _msp430_instruction;
class RegisterSet;
class Memory;
class Instruction;

typedef int (*InstructionCallback) (RegisterSet *reg, Memory *mem, Instruction *instruction);

typedef enum {
	Instruction1,
	InstructionCond,
	Instruction2,
} InstructionType;

void addInstruction(InstructionType type, unsigned int opcode, _msp430_instruction *instruction);

int executeInstruction(RegisterSet *reg, Memory *mem, Instruction *instruction);

class _msp430_instruction {
	public:
		_msp430_instruction(const char *name, InstructionType type, unsigned int opcode, InstructionCallback callback);
		const char *name;
		InstructionCallback callback;
};

#define MSP430_INSTRUCTION(NAME, TYPE, OPCODE, CALLBACK) \
	static const char *instruction_name_##OPCODE = NAME;\
	_msp430_instruction _msp430_instruction##TYPE##_##OPCODE(instruction_name_##OPCODE, TYPE, OPCODE, CALLBACK);

}
