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
#include "SignalManager.h"

class Variant;

namespace MSP430 {

class Memory;
class InterruptManager;
class PinMultiplexer;
class PinHandler;

typedef enum {
	P1 = 0,
	P2,
	P3,
	P4,
	P5,
	P6,
	P7,
	P8,
	UNKNOWN,
} PinType;

class PinWatcher {
	public:
		virtual void handlePinChanged(int id, double value) = 0;
};

class PinManager : public SignalManager {
	public:
		PinManager(Memory *mem, InterruptManager *intManager, Variant *variant);
		virtual ~PinManager();


		PinMultiplexer *addPin(PinType type, int subtype = -1);

		void reset();

		std::vector<PinMultiplexer *> addPinHandler(const std::string &name, PinHandler *handler);

		bool handlePinInput(int id, double value);
		void generateOutput(int id, double value);

		void setWatcher(PinWatcher *watcher) {
			m_watcher = watcher;
		}

	private:
		Memory *m_mem;
		Variant *m_variant;
		PinWatcher *m_watcher;
		InterruptManager *m_intManager;
		std::vector<PinMultiplexer *> m_multiplexers;
};

}
