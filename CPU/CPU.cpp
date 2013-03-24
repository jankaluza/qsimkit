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

#include "CPU/CPU.h"
#include "CPU/Memory/Memory.h"
#include "CPU/Memory/RegisterSet.h"
#include "CPU/Memory/Register.h"
#include "CPU/Instructions/Instruction.h"
#include "CPU/Instructions/InstructionDecoder.h"
#include "CPU/Instructions/InstructionManager.h"

#include <iostream>
#include <sstream>

CPU::CPU() : m_cycles(0), m_instructionCycles(0),
	m_mem(new Memory(512000)), m_reg(new RegisterSet()),
	m_decoder(new InstructionDecoder(m_reg, m_mem)),
	m_instruction(new Instruction) {

	m_reg->addDefaultRegisters();
}

CPU::~CPU() {

}

bool CPU::loadA43(const std::string &data) {
	return m_mem->loadA43(data, m_reg);
}

void CPU::delta_int() {
	m_instructionCycles = m_decoder->decodeCurrentInstruction(m_instruction);
	m_instructionCycles += executeInstruction(m_reg, m_mem, m_instruction);

	m_cycles += m_instructionCycles;
}

void CPU::delta_ext(double e, const adevs::Bag<Event *>& xb) {
}

void CPU::delta_conf(const adevs::Bag<Event *>& xb) {
	delta_int();
	delta_ext(0.0, xb);
}

void CPU::output_func(adevs::Bag<Event *>& yb) {

}

double CPU::ta() {
	return m_instructionCycles;
}

void CPU::gc_output(adevs::Bag<Event *>& g) {

}
