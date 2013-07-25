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
#include "PinHandler.h"

namespace MSP430 {

class PinMultiplexer;
class InterruptManager;

class GPPinHandler : public PinHandler, public MemoryWatcher {
	public:
		GPPinHandler(Memory *mem, PinMultiplexer *mpx,
					 InterruptManager *intManager, uint16_t dir,
					 uint16_t in, uint16_t out, uint16_t ie, uint16_t ies,
					 uint16_t ifg, uint16_t intvec, uint8_t id);
		virtual ~GPPinHandler();

		void handlePinInput(const std::string &name, double value);

		void handlePinActivated(const std::string &name);

		void handlePinDeactivated(const std::string &name);

		void handleMemoryChanged(::Memory *memory, uint16_t address);

	private:
		Memory *m_mem;
		PinMultiplexer *m_mpx;
		InterruptManager *m_intManager;
		uint16_t m_dir;
		uint16_t m_in;
		uint16_t m_out;
		uint16_t m_ie;
		uint16_t m_ies;
		uint16_t m_ifg;
		uint16_t m_intvec;
		uint8_t m_id;
		bool m_active;
		bool m_oldValue;
};

}
