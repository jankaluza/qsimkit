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

#include <iostream>
#include <sstream>
#include <map>

static int execMOV(Instruction *i) {
	if (i->bw) {
		i->getDst()->setByte(i->getSrc()->getByte());
	}
	else {
		i->getDst()->set(i->getSrc()->get());
	}
	return 0;
}

static int execADD(Instruction *i) {
	if (i->bw) {
		i->getDst()->setByte(i->getDst()->getByte() + i->getSrc()->getByte());
	}
	else {
		i->getDst()->setBigEndian(i->getDst()->getBigEndian() + i->getSrc()->getBigEndian());
	}
	return 0;
}

MSP430_INSTRUCTION("mov", Instruction2, 4, &execMOV);
MSP430_INSTRUCTION("add", Instruction2, 5, &execADD);
