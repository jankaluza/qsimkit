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

#include "Oscillator.h"
#include <iostream>
#include <algorithm>

namespace MSP430 {
	
Oscillator::Oscillator() {
}

Oscillator::~Oscillator() {
	
}

void Oscillator::addHandler(OscillatorHandler *handler) {
	m_handlers.push_back(handler);
}

void Oscillator::removeHandler(OscillatorHandler *handler) {
	std::vector<OscillatorHandler *>::iterator it = std::find(m_handlers.begin(), m_handlers.end(), handler);
	if (it != m_handlers.end()) {
		m_handlers.erase(it);
	}
}

void Oscillator::tick() {
	for (std::vector<OscillatorHandler *>::const_iterator it = m_handlers.begin(); it != m_handlers.end(); ++it) {
		(*it)->tick();
	}
}

}
