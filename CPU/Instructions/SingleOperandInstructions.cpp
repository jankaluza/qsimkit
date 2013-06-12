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

namespace MSP430 {

#define SET_N(REG, R, B) REG->getp(2)->setBit(SR_N, R & B);
#define SET_Z(REG, R, B) REG->getp(2)->setBit(SR_Z, (R & B) == 0);
#define SET_C(REG, R, B) REG->getp(2)->setBit(SR_C, R < 0 || R > B);

static int execRRC(RegisterSet *reg, Memory *mem, Instruction *i) {
	if (i->bw) {
		int32_t d, r, c;
		d = i->getDst()->getByte();
		c = reg->getp(2)->isBitSet(SR_C);
		r = (c << 7) | ((d >> 1) & 0x7f);
		i->getDst()->setByte(r);
		i->getDst()->callWatchers();
		
		SET_N(reg, r, 0x80);
		SET_Z(reg, r, 0x80);
		reg->getp(2)->setBit(SR_C, d & 1);
		reg->getp(2)->setBit(SR_V, 0);
	}
	else {
		int32_t d, r, c;
		d = i->getDst()->getByte();
		c = reg->getp(2)->isBitSet(SR_C);
		r = (c << 15) | ((d >> 1) & 0x7fff);
		i->getDst()->setByte(r);
		i->getDst()->callWatchers();
		
		SET_N(reg, r, 0x8000);
		SET_Z(reg, r, 0x8000);
		reg->getp(2)->setBit(SR_C, d & 1);
		reg->getp(2)->setBit(SR_V, 0);
	}

	return 0;
}

static int execRRA(RegisterSet *reg, Memory *mem, Instruction *i) {
	if (i->bw) {
		int32_t d, r;
		d = i->getDst()->getByte();
		r = (d & 0x080) | ((d >> 1) & 0x7f);
		i->getDst()->setByte(r);
		i->getDst()->callWatchers();
		
		SET_N(reg, r, 0x80);
		SET_Z(reg, r, 0x80);
		reg->getp(2)->setBit(SR_C, d & 1);
		reg->getp(2)->setBit(SR_V, 0);
	}
	else {
		int32_t d, r;
		d = i->getDst()->getBigEndian();
		r = (d & 0x08000) | ((d >> 1) & 0x7fff);
		i->getDst()->setBigEndian(r);
		i->getDst()->callWatchers();
		
		SET_N(reg, r, 0x8000);
		SET_Z(reg, r, 0x8000);
		reg->getp(2)->setBit(SR_C, d & 1);
		reg->getp(2)->setBit(SR_V, 0);
	}

	return 0;
}

static int execPUSH(RegisterSet *reg, Memory *mem, Instruction *i) {
	// Decrease SP, Store current PC, change PC
	uint16_t sp = reg->get(1)->getBigEndian() - 2;
	reg->get(1)->setBigEndian(sp);
	if (i->bw) {
		mem->set(sp, i->getDst()->getByte());
	}
	else {
		mem->setBigEndian(sp, i->getDst()->getBigEndian());
	}

	reg->get(1)->callWatchers();
	
	return 0;
}

static int execCALL(RegisterSet *reg, Memory *mem, Instruction *i) {
	// Decrease SP, Store current PC, change PC
	uint16_t sp = reg->get(1)->getBigEndian();
	sp -= 2;
	mem->setBigEndian(sp, reg->get(0)->getBigEndian());
	reg->get(1)->setBigEndian(sp);
	reg->get(1)->callWatchers();
	reg->get(0)->setBigEndian(i->getDst()->getBigEndian());
	return 0;
}

static int execRETI(RegisterSet *reg, Memory *mem, Instruction *i) {
	uint16_t sp = reg->get(1)->getBigEndian();

	// POP SR from stack
	reg->get(2)->setBigEndian(mem->getBigEndian(sp));
	reg->get(2)->callWatchers();
	sp += 2;

	// POP PC from stack
	reg->get(0)->setBigEndian(mem->getBigEndian(sp));
	sp += 2;

	reg->get(1)->setBigEndian(sp);
	reg->get(1)->callWatchers();
	return 0;
}

MSP430_INSTRUCTION("rrc",  Instruction1, 0, &execRRC);
MSP430_INSTRUCTION("rra",  Instruction1, 2, &execRRA);
MSP430_INSTRUCTION("push", Instruction1, 4, &execPUSH);
MSP430_INSTRUCTION("call", Instruction1, 5, &execCALL);
MSP430_INSTRUCTION("reti", Instruction1, 6, &execRETI);

}
