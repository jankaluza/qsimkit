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
	
Oscillator::Oscillator(const std::string &name) : m_name(name),
m_rising(true), m_inTick(false), m_willAddRemove(false) {
}

Oscillator::~Oscillator() {
	
}

void Oscillator::addHandler(OscillatorHandler *handler) {
	// If this method is called from the handler itself, it would segfaulf
	// if we add the handler right now. Postpone adding in the end of tick().
	if (m_inTick) {
		m_toAdd.push_back(handler);
		m_willAddRemove = true;
		return;
	}

	if (m_handlers.empty()) {
		start();
	}

	m_handlers.push_back(handler);
}

void Oscillator::removeHandler(OscillatorHandler *handler) {
	// If this method is called from the handler itself, it would segfaulf
	// if we remove it right now. Postpone removal in the end of tick().
	if (m_inTick) {
		m_toRemove.push_back(handler);
		m_willAddRemove = true;
		return;
	}

	std::vector<OscillatorHandler *>::iterator it = std::find(m_handlers.begin(), m_handlers.end(), handler);
	if (it != m_handlers.end()) {
		m_handlers.erase(it);
	}

	if (m_handlers.empty()) {
		pause();
	}
}

void Oscillator::tick() {
	m_inTick = true;
	for (std::vector<OscillatorHandler *>::const_iterator it = m_handlers.begin(); it != m_handlers.end(); ++it) {
		if (m_rising) {
			(*it)->tickRising();
		}
		else {
			(*it)->tickFalling();
		}
	}
	m_rising = !m_rising;
	m_inTick = false;

	if (!m_willAddRemove) {
		return;
	}

	// Remove handlers which called removeHandler() during their callbacks.
	if (!m_toRemove.empty()) {
		for (std::vector<OscillatorHandler *>::const_iterator it = m_toRemove.begin(); it != m_toRemove.end(); ++it) {
			removeHandler(*it);
		}
		m_toRemove.clear();
	}

	// Add handlers which called addHandler() during their callbacks.
	if (!m_toAdd.empty()) {
		for (std::vector<OscillatorHandler *>::const_iterator it = m_toAdd.begin(); it != m_toAdd.end(); ++it) {
			addHandler(*it);
		}
		m_toAdd.clear();
	}
}

}
