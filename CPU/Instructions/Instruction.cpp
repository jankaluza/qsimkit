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

#include "CPU/Instructions/Instruction.h"
#include "CPU/Instructions/InstructionArgument.h"
#include "CPU/Memory/Register.h"

namespace MCU {

Instruction::Instruction () : m_src(0), m_dst(0) {

}

Instruction::~Instruction() {

}

void Instruction::setSrc(InstructionArgument *src) {
	m_src = src;
}

InstructionArgument *Instruction::getSrc() {
	return m_src;
}

void Instruction::setDst(InstructionArgument *dst) {
	m_dst = dst;
}

InstructionArgument *Instruction::getDst() {
	return m_dst;
}

}
