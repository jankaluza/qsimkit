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

#include "CPU/Instructions/InstructionDecoder.h"
#include "CPU/Instructions/InstructionArgument.h"
#include "CPU/Instructions/ConstantArgument.h"
#include "CPU/Instructions/MemoryArgument.h"
#include "CPU/Instructions/IndirectAutoincrementArgument.h"
#include "CPU/Instructions/IndexedArgument.h"
#include "CPU/Instructions/Instruction.h"
#include "CPU/Memory/RegisterSet.h"
#include "CPU/Memory/Register.h"
#include "CPU/Memory/Memory.h"

#include <iostream>
#include <sstream>

// [1111110000000000]2
#define INSTRUCTION1_MASK 64512
#define INSTRUCTION1_MAGIC 4096
#define IS_INSTRUCTION1(X) (X & INSTRUCTION1_MASK == INSTRUCTION1_MAGIC)
// [1110000000000000]2
#define INSTRUCTION_COND_MASK 57344
#define INSTRUCTION_COND_MAGIC 8192
#define IS_INSTRUCTION_COND(X) (X & INSTRUCTION_COND_MASK == INSTRUCTION_COND_MAGIC)

InstructionDecoder::InstructionDecoder(RegisterSet *reg, Memory *mem) :
	m_reg(reg), m_mem(mem) {

}

InstructionDecoder::~InstructionDecoder() {

}

InstructionArgument *InstructionDecoder::getSourceArg(int &cycles, uint16_t &pc, bool bw, uint8_t as, uint8_t source_reg) {
	InstructionArgument *arg = 0;

	if (source_reg == 2) {
		switch (as) {
			// Normal access
			case 0:
				arg = m_reg->get(2);
				break;
			// Absolute mode
			case 1:
				arg = new MemoryArgument(m_mem, m_mem->getBigEndian(pc));
				pc += 2;
				cycles += 2; // fetch + read from memory
				break;
			// Const 4
			case 2:
				arg = new ConstantArgument(4);
				break;
			// Const 8
			case 3:
				arg = new ConstantArgument(8);
				break;
			default:
				break;
		}
	}
	else if (source_reg == 3) {
		switch (as) {
			// Const 0
			case 0:
				arg = new ConstantArgument(0);
				break;
			// Const 1
			case 1:
				arg = new ConstantArgument(1);
				break;
			// Const 2
			case 2:
				arg = new ConstantArgument(2);
				break;
			// Const -1
			case 3:
				arg = new ConstantArgument(-1);
				break;
			default:
				break;
		}
	}
	else {
		switch (as) {
			// Register direct
			case 0:
				arg = m_reg->get(source_reg);
				break;
			// Indexed mode
			case 1:
				arg = new IndexedArgument(m_mem, m_reg->get(source_reg), m_mem->getBigEndian(pc));
				pc += 2;
				cycles += 2; // fetch + read
				break;
			// Indirect
			case 2:
				// simulate Indirect with indexed with offset 0
				arg = new IndexedArgument(m_mem, m_reg->get(source_reg), 0);
				cycles += 1; // target mem read
				break;
			case 3:
				if (source_reg == 0) {
					// Immediate mode
					arg = new ConstantArgument(m_mem->get(pc));
					pc += 2;
					cycles += 1; // fetch
				}
				else {
					// Indirect autoincrement
					arg = new IndirectAutoincrementArgument(m_mem, m_reg->get(source_reg), bw);
					cycles += 1; // read
				}
				break;
			default:
				break;
		}
	}

	return arg;
}

InstructionArgument *InstructionDecoder::getDestArg(int &cycles, uint16_t &pc, bool bw, uint8_t ad, uint8_t dest_reg) {
	InstructionArgument *arg = 0;

	if (ad == 0) {
		arg = m_reg->get(dest_reg);
		if (dest_reg == 0) {
			cycles += 1; // Modifying PC needs 1 more cycle
		}
	}
	else {
		if (dest_reg == 2) {
			// Absolute address
			arg = new MemoryArgument(m_mem, m_mem->getBigEndian(pc));
			pc += 2;
			cycles += 3; // fetch, read from memory, write back
		}
		else {
			// Indexed
			arg = new IndexedArgument(m_mem, m_reg->get(dest_reg), m_mem->getBigEndian(pc));
			pc += 2;
			cycles += 3; // fetch, read from memory, write back
		}
	}

	return arg;
}

int InstructionDecoder::decodeCurrentInstruction(Instruction *instruction) {
	Register *pc_reg = m_reg->get(0);
	uint16_t pc = pc_reg->get();

	uint16_t data = m_mem->getBigEndian(pc);
	int cycles = 1; // instruction fetch
	pc += 2;

	if (IS_INSTRUCTION1(data)) {
		instruction->type = Instruction1;
	}
	else if (IS_INSTRUCTION_COND(data)) {
		instruction->type = InstructionCond;
	}
	else {
		instruction->type = Instruction2;
		instruction->opcode = data >>12;
		uint8_t dest_reg = data & 15;
		uint8_t source_reg = (data >> 8) & 15;

		uint8_t as = (data >> 4) & 3;
		uint8_t ad = (data >> 7) & 1;
		bool bw = (data >> 6) & 1;

		InstructionArgument *src = getSourceArg(cycles, pc, bw, as, source_reg);
		instruction->setSrc(src);

		InstructionArgument *dst = getDestArg(cycles, pc, bw, ad, dest_reg);
		instruction->setDst(dst);

		instruction->bw = bw;
	}

	pc_reg->set(pc);

	return cycles;
}
