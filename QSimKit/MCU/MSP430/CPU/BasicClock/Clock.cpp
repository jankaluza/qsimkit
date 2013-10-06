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

#include "Clock.h"
#include <iostream>
#include <algorithm>

namespace MSP430 {
	
Clock::Clock() {
}

Clock::~Clock() {
	
}

void Clock::addHandler(ClockHandler *handler, Mode mode) {
	// Start the clock with first handler added
	if (m_handlers.empty() && m_fallingHandlers.empty()) {
		start();
	}

	switch (mode) {
		case Rising:
			m_handlers.push_back(handler);
			break;
		case Falling:
			m_fallingHandlers.push_back(handler);
			break;
		case RisingFalling:
			m_handlers.push_back(handler);
			m_fallingHandlers.push_back(handler);
			break;
	}
}

void Clock::removeHandler(ClockHandler *handler) {
	std::vector<ClockHandler *>::iterator it = std::find(m_handlers.begin(), m_handlers.end(), handler);
	if (it != m_handlers.end()) {
		m_handlers.erase(it);
	}

	it = std::find(m_fallingHandlers.begin(), m_fallingHandlers.end(), handler);
	if (it != m_fallingHandlers.end()) {
		m_fallingHandlers.erase(it);
	}

	// pause clock when it's not used
	if (m_handlers.empty() || m_fallingHandlers.empty()) {
		pause();
	}
}

void Clock::callRisingHandlers() {
	for (std::vector<ClockHandler *>::const_iterator it = m_handlers.begin(); it != m_handlers.end(); ++it) {
		(*it)->tickRising();
	}
}

void Clock::callFallingHandlers() {
	for (std::vector<ClockHandler *>::const_iterator it = m_handlers.begin(); it != m_handlers.end(); ++it) {
		(*it)->tickFalling();
	}
}

}
