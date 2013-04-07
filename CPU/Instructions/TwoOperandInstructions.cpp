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

static int execMOV(RegisterSet *reg, Memory *mem, Instruction *i) {
	if (i->bw) {
		i->getDst()->setByte(i->getSrc()->getByte());
		i->getDst()->callWatchers();
	}
	else {
		i->getDst()->set(i->getSrc()->get());
		i->getDst()->callWatchers();
	}
	return 0;
}

#define SET_ADD_OVERFLOW(REG, R, D, S, B) \
	REG->get(2)->setBit(SR_V, (!(R & B) && (S & B) && (D & B)) ||\
	((R & B) && !(S & B) && !(D & B)));

#define SET_N(REG, R, B) REG->get(2)->setBit(SR_N, R & B);
#define SET_Z(REG, R, B) REG->get(2)->setBit(SR_Z, (R & B) == 0);
#define SET_C(REG, R, B) REG->get(2)->setBit(SR_C, R < 0 || R > B);

static int add(RegisterSet *reg, Memory *mem, Instruction *i, bool carry) {
	if (i->bw) {
		int32_t d, s, r;
		d = (int32_t) i->getDst()->getByte();
		s = (int32_t) i->getSrc()->getByte();
		r = d + s + carry;
		i->getDst()->setByte(r);
		i->getDst()->callWatchers();

		SET_ADD_OVERFLOW(reg, r, d, s, 0x80);
		SET_N(reg, r, 0x80);
		SET_Z(reg, r, 0xff);
		SET_C(reg, r, 0xff);
		reg->get(2)->callWatchers();
	}
	else {
		int32_t d, s, r;
		d = (int32_t) i->getDst()->getBigEndian();
		s = (int32_t) i->getSrc()->getBigEndian();
		r = d + s + carry;
		i->getDst()->setBigEndian(r);
		i->getDst()->callWatchers();

		SET_ADD_OVERFLOW(reg, r, d, s, 0x8000);
		SET_N(reg, r, 0x8000);
		SET_Z(reg, r, 0xffff);
		SET_C(reg, r, 0xffff);
		reg->get(2)->callWatchers();
	}
	return 0;	
}

static int execADD(RegisterSet *reg, Memory *mem, Instruction *i) {
	return add(reg, mem, i, false);
}

static int execADDC(RegisterSet *reg, Memory *mem, Instruction *i) {
	return add(reg, mem, i, true);
}

#define SET_SUB_OVERFLOW(REG, R, D, S, B) \
	REG->get(2)->setBit(!(R & B) && !(S & B) && (D & B) ||\
	((R & B) && (S & B) && !(D & B)));

#define SET_SUB_C(REG, R, B) REG->get(2)->setBit(SR_C, !(R < 0 || R > B));

static int sub(RegisterSet *reg, Memory *mem, Instruction *i, bool store, bool carry) {
	if (i->bw) {
		int8_t d, s, r;
		d = (int16_t) i->getDst()->getByte();
		s = (int16_t) i->getSrc()->getByte();
		r = d - s + carry;
		if (store) {
			i->getDst()->setByte(r);
			i->getDst()->callWatchers();
		}

		SET_SUB_OVERFLOW(reg, r, d, s, 0x80);
		SET_N(reg, r, 0x80);
		SET_Z(reg, r, 0xff);
		SET_SUB_C(reg, r, 0xff);
		reg->get(2)->callWatchers();
	}
	else {
		int16_t d, s, r;
		d = (int16_t) i->getDst()->getBigEndian();
		s = (int16_t) i->getSrc()->getBigEndian();
		r = d - s + carry;
		if (store) {
			i->getDst()->setBigEndian(r);
			i->getDst()->callWatchers();
		}

		SET_SUB_OVERFLOW(reg, r, d, s, 0x8000);
		SET_N(reg, r, 0x8000);
		SET_Z(reg, r, 0xffff);
		SET_SUB_C(reg, r, 0xffff);
		reg->get(2)->callWatchers();
	}
	return 0;
}

static int execSUBC(RegisterSet *reg, Memory *mem, Instruction *i) {
	return sub(reg, mem, i, true, true);
}

static int execSUB(RegisterSet *reg, Memory *mem, Instruction *i) {
	return sub(reg, mem, i, true, false);
}

static int execCMP(RegisterSet *reg, Memory *mem, Instruction *i) {
	return sub(reg, mem, i, false, false);
}

MSP430_INSTRUCTION("mov", Instruction2, 4, &execMOV);
MSP430_INSTRUCTION("add", Instruction2, 5, &execADD);
MSP430_INSTRUCTION("addc", Instruction2, 6, &execADDC);
MSP430_INSTRUCTION("subc", Instruction2, 7, &execSUBC);
MSP430_INSTRUCTION("sub", Instruction2, 8, &execSUB);
MSP430_INSTRUCTION("cmp", Instruction2, 9, &execCMP);
