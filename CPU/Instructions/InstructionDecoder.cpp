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
	m_srcMemArg = new MemoryArgument(mem, 0);
	m_srcConstArg = new ConstantArgument(0);
	m_srcIndexedArg = new IndexedArgument(mem, 0, 0);
	m_srcIndirectAutoArg = new IndirectAutoincrementArgument(mem, 0, 0);
	m_dstMemArg = new MemoryArgument(mem, 0);
	m_dstIndexedArg = new IndexedArgument(mem, 0, 0);
}

InstructionDecoder::~InstructionDecoder() {
	delete m_srcMemArg;
	delete m_srcConstArg;
	delete m_srcIndexedArg;
	delete m_srcIndirectAutoArg;
	delete m_dstMemArg;
	delete m_dstIndexedArg;
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
				arg = m_srcMemArg;
				m_srcMemArg->reinitialize(m_mem->getBigEndian(pc));
				pc += 2;
				cycles += 2; // fetch + read from memory
				break;
			// Const 4
			case 2:
				arg = m_srcConstArg;
				m_srcConstArg->reinitialize(0x0400);
				break;
			// Const 8
			case 3:
				arg = m_srcConstArg;
				m_srcConstArg->reinitialize(0x0800);
				break;
			default:
				break;
		}
	}
	else if (source_reg == 3) {
		switch (as) {
			// Const 0
			case 0:
				arg = m_srcConstArg;
				m_srcConstArg->reinitialize(0);
				break;
			// Const 1
			case 1:
				arg = m_srcConstArg;
				m_srcConstArg->reinitialize(0x0100);
				break;
			// Const 2
			case 2:
				arg = m_srcConstArg;
				m_srcConstArg->reinitialize(0x0200);
				break;
			// Const -1
			case 3:
				arg = m_srcConstArg;
				m_srcConstArg->reinitialize(0xffff);
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
				arg = m_srcIndexedArg;
				m_srcIndexedArg->reinitialize(m_reg->get(source_reg), m_mem->getBigEndian(pc));
				pc += 2;
				cycles += 2; // fetch + read
				break;
			// Indirect
			case 2:
				// simulate Indirect with indexed with offset 0
				arg = m_srcIndexedArg;
				m_srcIndexedArg->reinitialize(m_reg->get(source_reg), 0);
				cycles += 1; // target mem read
				break;
			case 3:
				if (source_reg == 0) {
					// Immediate mode
					arg = m_srcConstArg;
					m_srcConstArg->reinitialize(m_mem->get(pc));
					pc += 2;
					cycles += 1; // fetch
				}
				else {
					// Indirect autoincrement
					arg = m_srcIndirectAutoArg;
					m_srcIndirectAutoArg->reinitialize(m_reg->get(source_reg), bw);
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
			arg = m_dstMemArg;
			m_dstMemArg->reinitialize(m_mem->getBigEndian(pc));
			pc += 2;
			cycles += 3; // fetch, read from memory, write back
		}
		else {
			// Indexed
			arg = m_dstIndexedArg;
			m_dstIndexedArg->reinitialize(m_reg->get(dest_reg), m_mem->getBigEndian(pc));
			pc += 2;
			cycles += 3; // fetch, read from memory, write back
		}
	}

	return arg;
}

int InstructionDecoder::decodeCurrentInstruction(Instruction *instruction) {
	Register *pc_reg = m_reg->get(0);
	uint16_t pc = pc_reg->getBigEndian();

	uint16_t data = m_mem->getBigEndian(pc);
	int cycles = 1; // instruction fetch
	pc += 2;

	if ((data & 0xf000) == 0x1000) {
		// Single operand instructions
		instruction->type = Instruction1;
		instruction->opcode = (data >>7) & 7;
		uint8_t dest_reg = data & 15;
		uint8_t ad = (data >> 4) & 3;
		bool bw = (data >> 6) & 1;

		InstructionArgument *dst = getSourceArg(cycles, pc, bw, ad, dest_reg);
		instruction->setDst(dst);

		instruction->bw = bw;
	}
	else if ((data & 0xe000) == 0x2000) {
		// Jump instructions
		instruction->type = InstructionCond;
		instruction->opcode = (data >>10) & 7;

		int16_t offset = ((data & 0x3ff) << 1); // PC offset * 2
		// negative offset
		if (offset & 0x400) {
			offset = -((~offset + 1) & 0x7ff);
		}

		instruction->offset = offset; // + 2; // PC_new = PC_old + offset * 2 + 2
		cycles += 1;
	}
	else {
		// Two operand instructions
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

	pc_reg->setBigEndian(pc);

	return cycles;
}
