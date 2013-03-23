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
#include "CPU/Instructions/InstructionArgument.h"
#include "CPU/Instructions/Instruction.h"
#include "CPU/Memory/RegisterSet.h"
#include "CPU/Memory/Register.h"
#include "CPU/Memory/Memory.h"

#include <iostream>
#include <sstream>
#include <map>

static int execCALL(RegisterSet *reg, Memory *mem, Instruction *i) {
	// Decrease SP, Store current PC, change PC
	uint16_t sp = reg->get(1)->getBigEndian();
	sp -= 2;
	mem->setBigEndian(sp, reg->get(0)->getBigEndian());
	reg->get(1)->setBigEndian(sp);
	reg->get(0)->setBigEndian(i->getDst()->getBigEndian());
	return 3;
}

static int execRETI(RegisterSet *reg, Memory *mem, Instruction *i) {
	uint16_t sp = reg->get(1)->getBigEndian();
	reg->get(1)->setBigEndian(mem->getBigEndian(sp));
	sp += 2;

	reg->get(0)->setBigEndian(mem->getBigEndian(sp));
	sp += 2;

	reg->get(1)->setBigEndian(sp);
	return 4;
}

MSP430_INSTRUCTION("call", Instruction1, 5, &execCALL);
MSP430_INSTRUCTION("reti", Instruction1, 6, &execRETI);
