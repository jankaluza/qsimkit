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

namespace MCU {

class Memory;
class RegisterSet;
class Instruction;
class InterruptManager;

class InterruptWatcher {
	public:
		virtual void handleInterruptFinished(InterruptManager *intManager, int vector) = 0;
};

class InterruptManager {
	public:
		InterruptManager(RegisterSet *reg, Memory *mem);
		virtual ~InterruptManager();

		void queueInterrupt(int vector);

		bool runQueuedInterrupts();

		void runInterrupt(int vector);

		void handleInstruction(Instruction *instruction);

		bool hasQueuedInterrupts();

		void clearQueuedInterrupts();

		void addWatcher(int vector, InterruptWatcher *watcher);

	private:
		RegisterSet *m_reg;
		Memory *m_mem;
		std::vector<int> m_interrupts;
		std::vector<int> m_runningInterrupts;
		std::map<int, std::vector<InterruptWatcher *> > m_watchers;
};

}
