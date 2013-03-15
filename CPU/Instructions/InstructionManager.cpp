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

#include "CPU/Instructions/InstructionManager.h"

#include <iostream>
#include <sstream>
#include <map>

#define TYPE_OFFSET 100

static std::map<unsigned int, _msp430_instruction *> *instructions;

void addInstruction(InstructionType type, unsigned int opcode, _msp430_instruction *instruction) {
	if (instructions == 0) {
		instructions = new std::map<unsigned int, _msp430_instruction *>;
	}
	(*instructions)[((int) type) * TYPE_OFFSET + opcode] = instruction;
	std::cout << "Loaded instruction: " << (*instructions)[((int) type) * TYPE_OFFSET + opcode]->name << "\n";
}

_msp430_instruction::_msp430_instruction(const char *name, InstructionType type, unsigned int opcode, InstructionCallback callback) {
	this->name = name;
	this->callback = callback;
	addInstruction(type, opcode, this);
}
