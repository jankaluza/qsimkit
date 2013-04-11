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
#include "CPU/Memory/Memory.h"

class Variant;
class Memory;

typedef enum {
	UNKNOWN,
	P1,
	P2,
	P3,
	P4,
	P5,
	P6,
	P7,
	P8,
} PinType;


class InternalPin {
	public:
		InternalPin() : type(P1), subtype(-1), value(0) {}
		InternalPin(PinType type, int subtype) : type(type), subtype(subtype), value(0) {}

	PinType type;
	int subtype;
	double value;
};

class PinWatcher {
	public:
		virtual void handlePinChanged(int id, double value) = 0;
};

class PinManager : public MemoryWatcher {
	public:
		PinManager(Memory *mem, Variant *variant);
		virtual ~PinManager();

		void setMemory(Memory *mem);

		void addPin(PinType type, int subtype = -1);

		bool handlePinInput(int id, double value);

		void handleMemoryChanged(Memory *memory, uint16_t address);

		void setWatcher(PinWatcher *watcher) {
			m_watcher = watcher;
		}

	private:
		Memory *m_mem;
		Variant *m_variant;
		std::vector<InternalPin> m_pins;
		std::map<int, int> m_gpCache;
		PinWatcher *m_watcher;
};
