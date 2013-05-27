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

#include "CPU/Memory/Register.h"
#include <algorithm>

namespace MCU {

Register::Register(int id, const std::string &name, uint16_t value, const std::string &desc) :
	m_id(id), m_value(value), m_name(name), m_desc(desc) { }

Register::~Register() {

}

uint16_t Register::get() {
	return m_value;
}

uint16_t Register::getBigEndian() {
	uint16_t w;
	uint8_t *ptr = (uint8_t *) &w;
	uint8_t *ptr2 = (uint8_t *) &m_value;
	*ptr++ = *(ptr2 + 1);
	*ptr++ = *ptr2;
	return w;
}

void Register::set(uint16_t value) {
	m_value = value;
}

void Register::setBigEndian(uint16_t value) {
	uint8_t *ptr = (uint8_t *) &m_value;
	uint8_t *ptr2 = (uint8_t *) &value;
	*ptr++ = *(ptr2 + 1);
	*ptr++ = *ptr2;
}

uint8_t Register::getByte() {
	return (uint8_t) m_value;
}

void Register::setByte(uint8_t value) {
	uint8_t *ptr = (uint8_t *) &m_value;
	*ptr = value;
}

bool Register::isBitSet(uint16_t bit) {
	return getBigEndian() & bit;
}

bool Register::setBit(uint16_t bit, bool value) {
	uint16_t v = getBigEndian();
	if (value) {
		v = v | bit;
	}
	else {
		v = v & (~bit);
	}

	setBigEndian(v);
}

void Register::addWatcher(RegisterWatcher *watcher) {
	if (std::find(m_watchers.begin(), m_watchers.end(), watcher) == m_watchers.end()) {
		m_watchers.push_back(watcher);
	}
}

void Register::callWatchers() {
	if (m_watchers.empty())
		return;

	std::vector<RegisterWatcher *> toRemove;

	for (std::vector<RegisterWatcher *>::const_iterator it = m_watchers.begin(); it != m_watchers.end(); ++it) {
		if (!(*it)->handleRegisterChanged(this, m_id, m_value)) {
			toRemove.push_back(*it);
		}
	}

	for (std::vector<RegisterWatcher *>::const_iterator it = toRemove.begin(); it != toRemove.end(); ++it) {
		removeWatcher(*it);
	}

}

void Register::removeWatcher(RegisterWatcher *watcher) {
	std::vector<RegisterWatcher *>::iterator it = std::find(m_watchers.begin(), m_watchers.end(), watcher);
	if (it != m_watchers.end()) {
		m_watchers.erase(it);
	}
}

}

