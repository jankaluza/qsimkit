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

#include "SignalManager.h"
#include "SignalHandler.h"
#include <iostream>
#include <algorithm>

namespace MSP430 {

SignalManager::SignalManager() {
	
}

SignalManager::~SignalManager() {
	
}

void SignalManager::addSignalHandler(const std::string &name, SignalHandler *handler) {
	m_handlers[name].push_back(handler);
}

void SignalManager::removeSignalHandler(const std::string &name, SignalHandler *handler) {
	std::map<std::string, std::vector<SignalHandler *> >::iterator it = m_handlers.find(name);
	if (it == m_handlers.end()) {
		return;
	}

	std::vector<SignalHandler *> &h = it->second;
	std::vector<SignalHandler *>::iterator it2 = std::find(h.begin(), h.end(), handler);
	if (it2 == h.end()) {
		return;
	}

	h.erase(it2);
	if (h.empty()) {
		m_handlers.erase(name);
	}
}

void SignalManager::generateSignal(const std::string &name, double value) {
	std::map<std::string, std::vector<SignalHandler *> >::iterator it = m_handlers.find(name);
	if (it == m_handlers.end()) {
		return;
	}

	std::vector<SignalHandler *> &h = it->second;
	for (std::vector<SignalHandler *>::iterator it2 = h.begin(); it2 != h.end(); ++it2) {
		(*it2)->handleSignal(name, value);
	}
	
}

}
