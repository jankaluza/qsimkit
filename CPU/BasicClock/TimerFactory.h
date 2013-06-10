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

#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include "CPU/BasicClock/Oscillator.h"

class Variant;

namespace MSP430 {

class Memory;
class DCO;
class VLO;
class LFXT1;

class TimerFactory {
	public:
		virtual ~TimerFactory() {}
		virtual DCO *createDCO(Memory *mem, Variant *variant) = 0;
		virtual VLO *createVLO() = 0;
		virtual LFXT1 *createLFXT1(Memory *mem, Variant *variant) = 0;
};

}
