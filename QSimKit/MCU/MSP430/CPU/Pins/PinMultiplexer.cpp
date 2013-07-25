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

#include "PinMultiplexer.h"
#include "PinHandler.h"
#include "PinManager.h"
#include "CPU/Variants/Variant.h"
#include "CPU/Memory/Memory.h"
#include <iostream>
#include <algorithm>

namespace MSP430 {

PinMultiplexer::PinMultiplexer(PinManager *manager, int id, Memory *mem, Variant *variant,
							   uint16_t dir, uint16_t sel, uint8_t index) :
m_manager(manager), m_id(id), m_mem(mem), m_variant(variant), m_dir(dir),
m_sel(sel), m_index(1 << index), m_value(0), m_valueIsInput(false) {
	m_mem->addWatcher(m_dir, this);
	m_mem->addWatcher(m_sel, this);
}

PinMultiplexer::~PinMultiplexer() {

}

void PinMultiplexer::addMultiplexing(Condition &c, const std::string &outputName) {
	m_conds.push_back(c);
	m_outputs.push_back(outputName);

// 	std::cout << m_id << ": Adding multiplexing " << outputName << "\n";

	// By adding this Multiplexing the current handler can change
	handleMemoryChanged(m_mem, 0);
}

bool PinMultiplexer::hasMultiplexing(const std::string &outputName) {
	return std::find(m_outputs.begin(), m_outputs.end(), outputName) != m_outputs.end();
}

void PinMultiplexer::addPinHandler(const std::string &name, PinHandler *handler) {
	m_handlers[name] = handler;
	handler->currentOutputValue = 0;

	// By adding this handler, multiplexing can change
	handleMemoryChanged(m_mem, 0);
}

bool PinMultiplexer::handleInput(double value) {
	if (m_handler) {
		m_handler->handlePinInput(m_handlerName, value);
	}

	m_value = value;
	m_valueIsInput = true;
	return true;
}

void PinMultiplexer::generateOutput(PinHandler *handler, double value) {
	handler->currentOutputValue = value;

	// This is not active handler, it can't generate output
	if (handler != m_handler) {
		return;
	}

	// Generate output only if it's different value then the one we
	// have generated last time
	if (m_valueIsInput || (!m_valueIsInput && m_value != value)) {
		m_manager->generateOutput(m_id, value);
	}

	m_value = value;
	m_valueIsInput = false;
}

void PinMultiplexer::reset() {
	m_value = 0;
	m_valueIsInput = false;
}

double PinMultiplexer::getValue(bool &isInput) {
	isInput = m_valueIsInput;
	return m_value;
}

void PinMultiplexer::handleMemoryChanged(::Memory *memory, uint16_t address) {
	for (int i = 0; i < m_conds.size(); ++i) {
		const Condition &c = m_conds[i];
		bool satisfied = true;
		for (Condition::const_iterator c_it = c.begin(); c_it != c.end(); ++c_it) {
			if (c_it->first == "sel") {
				if (((m_mem->getByte(m_sel) & m_index) == m_index) != c_it->second) {
					satisfied = false;
					break;
				}
			}
			else if (c_it->first == "dir") {
				if (((m_mem->getByte(m_dir) & m_index) == m_index) != c_it->second) {
					satisfied = false;
					break;
				}
			}
		}

		if (satisfied) {
			if (!m_handlerName.empty() && m_handler) {
				m_handler->handlePinDeactivated(m_handlerName);
			}

			m_handlerName = m_outputs[i];
			m_handler = m_handlers[m_handlerName];

			if (m_handler) {
				m_handler->handlePinActivated(m_handlerName);
				m_handler->handlePinInput(m_handlerName, m_value);
				if (!m_valueIsInput && m_value != m_handler->currentOutputValue) {
					generateOutput(m_handler, m_handler->currentOutputValue);
				}
			}
			break;
		}
	}
}

}
