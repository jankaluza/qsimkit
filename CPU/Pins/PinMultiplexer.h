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

namespace MSP430 {

class Memory;
class PinHandler;
class PinManager;

class PinMultiplexer : public MemoryWatcher {
	public:

		typedef std::map<std::string, uint8_t> Condition;

		PinMultiplexer(PinManager *manager, int id, Memory *mem,
					   Variant *variant, uint16_t dir, uint16_t sel, uint8_t index);

		virtual ~PinMultiplexer();

		void addMultiplexing(Condition &c, const std::string &outputName);

		bool hasMultiplexing(const std::string &outputName);

		void addPinHandler(const std::string &name, PinHandler *handler);

		bool handleInput(double value);

		void generateOutput(PinHandler *handler, double value);

		void reset();

		void handleMemoryChanged(Memory *memory, uint16_t address);

		double getValue(bool &isInput);

	private:
		PinManager *m_manager;
		int m_id;
		Memory *m_mem;
		Variant *m_variant;
		uint16_t m_dir;
		uint16_t m_sel;
		uint8_t m_index;
		double m_value;
		bool m_valueIsInput;
		
		std::vector<Condition> m_conds;
		std::vector<std::string> m_outputs;
		std::map<std::string, PinHandler *> m_handlers;
		PinHandler *m_handler;
		std::string m_handlerName;
};

}
