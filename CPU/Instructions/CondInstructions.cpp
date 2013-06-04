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

#include <iostream>
#include <sstream>
#include <map>

namespace MSP430 {

static int execJNZ(RegisterSet *reg, Memory *mem, Instruction *i) {
	if (!reg->getp(2)->isBitSet(SR_Z)) {
		reg->getp(0)->setBigEndian(reg->getp(0)->getBigEndian() + i->offset);
	}
	return 0;
}

static int execJZ(RegisterSet *reg, Memory *mem, Instruction *i) {
	if (reg->getp(2)->isBitSet(SR_Z)) {
		reg->getp(0)->setBigEndian(reg->getp(0)->getBigEndian() + i->offset);
	}
	return 0;
}

static int execJNC(RegisterSet *reg, Memory *mem, Instruction *i) {
	if (!reg->getp(2)->isBitSet(SR_C)) {
		reg->getp(0)->setBigEndian(reg->getp(0)->getBigEndian() + i->offset);
	}
	return 0;
}

static int execJC(RegisterSet *reg, Memory *mem, Instruction *i) {
	if (reg->getp(2)->isBitSet(SR_C)) {
		reg->getp(0)->setBigEndian(reg->getp(0)->getBigEndian() + i->offset);
	}
	return 0;
}

static int execJN(RegisterSet *reg, Memory *mem, Instruction *i) {
	if (reg->getp(2)->isBitSet(SR_N)) {
		reg->getp(0)->setBigEndian(reg->getp(0)->getBigEndian() + i->offset);
	}
	return 0;
}

static int execJGE(RegisterSet *reg, Memory *mem, Instruction *i) {
	if (!(reg->getp(2)->isBitSet(SR_N) ^ reg->getp(2)->isBitSet(SR_V))) {
		reg->getp(0)->setBigEndian(reg->getp(0)->getBigEndian() + i->offset);
	}
	return 0;
}

static int execJL(RegisterSet *reg, Memory *mem, Instruction *i) {
	if (reg->getp(2)->isBitSet(SR_N) ^ reg->getp(2)->isBitSet(SR_V)) {
		reg->getp(0)->setBigEndian(reg->getp(0)->getBigEndian() + i->offset);
	}
	return 0;
}

static int execJMP(RegisterSet *reg, Memory *mem, Instruction *i) {
	reg->getp(0)->setBigEndian(reg->getp(0)->getBigEndian() + i->offset);
	return 0;
}

MSP430_INSTRUCTION("jnz", InstructionCond, 0, &execJNZ);
MSP430_INSTRUCTION("jz", InstructionCond, 1, &execJZ);
MSP430_INSTRUCTION("jnc", InstructionCond, 2, &execJNC);
MSP430_INSTRUCTION("jc", InstructionCond, 3, &execJC);
MSP430_INSTRUCTION("jn", InstructionCond, 4, &execJN);
MSP430_INSTRUCTION("jge", InstructionCond, 5, &execJGE);
MSP430_INSTRUCTION("jl", InstructionCond, 6, &execJL);
MSP430_INSTRUCTION("jmp", InstructionCond, 7, &execJMP);

}
