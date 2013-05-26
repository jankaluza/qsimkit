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

#include "CPU/Memory/Memory.h"
#include "CPU/Memory/RegisterSet.h"
#include "CPU/Memory/Register.h"

#include <iostream>
#include <sstream>

namespace MCU {

static unsigned int hexToInt(const std::string &str) {
	unsigned int x;
	std::stringstream ss;
	ss << std::hex << str;
	ss >> x;
	return x;
}

Memory::Memory(unsigned int size) {
	m_memory.resize(size);
	m_watchers.resize(size);
	m_readWatchers.resize(size);

	for (int i = 0; i < size; ++i) {
		m_memory[i] = 0;
	}
}

Memory::~Memory() {
}

#define LOAD_DIGIT \
		if (++it == data.end()) { return false; } \
		if (*it < '0' || (*it > '9' && *it < 'A' && *it > 'F' && *it < 'a' && *it > 'f')) { return false; } \
		byte += *it;

#define LOAD_BYTE(X) \
		byte = "";\
		LOAD_DIGIT; \
		LOAD_DIGIT; \
		X = hexToInt(byte);

#define LOAD_2BYTES(X) \
		byte = "";\
		LOAD_DIGIT; \
		LOAD_DIGIT; \
		LOAD_DIGIT; \
		LOAD_DIGIT; \
		X = hexToInt(byte);

bool Memory::loadA43(const std::string &data, RegisterSet *reg) {
	unsigned int byte_count;
	unsigned int address;
	unsigned int record_type;
	unsigned int checksum;
	unsigned int default_ip;
	std::string byte;
	for (std::string::const_iterator it = data.begin(); it != data.end(); ++it) {
		if (*it == '\n' || *it == '\r') {
			continue;
		}

		if (*it != ':') {
			return false;
		}

		LOAD_BYTE(byte_count);
		LOAD_2BYTES(address);
		LOAD_BYTE(record_type);
		switch (record_type) {
			case 0:
				for (int i = 0; i < byte_count; i++, address++) {
					LOAD_BYTE(m_memory[address]);
				}
				break;
			case 3:
				if (byte_count != 4) {
					return false;
				}
				LOAD_2BYTES(default_ip);
				LOAD_2BYTES(default_ip);

				// Default PC is stored as big endian
				reg->get(0)->setBigEndian(default_ip);
				break;
			case 1:
				// We have reached End of File
				return true;
			default:
				break;
		}
		LOAD_BYTE(checksum);
	}

	return false;
}

uint16_t Memory::get(uint16_t address) {
	uint16_t w;
	uint8_t *ptr = (uint8_t *) &w;
	*ptr++ = m_memory[address + 1];
	*ptr++ = m_memory[address];
	callReadWatcher(address, w);
	return w;
}

uint16_t Memory::getBigEndian(uint16_t address, bool watchers) {
	uint16_t w;
	uint8_t *ptr = (uint8_t *) &w;
	*ptr++ = m_memory[address];
	*ptr++ = m_memory[address + 1];
	if (watchers) {
		callReadWatcher(address, w);
	}
	return w;
}

void Memory::callWatcher(uint16_t address) {
	std::vector<MemoryWatcher *> &watchers = m_watchers[address];
	if (watchers.empty())
		return;

	for (std::vector<MemoryWatcher *>::const_iterator it = watchers.begin(); it != watchers.end(); ++it) {
		(*it)->handleMemoryChanged(this, address);
	}
}

void Memory::callReadWatcher(uint16_t address, uint16_t &value) {
	std::vector<MemoryWatcher *> &watchers = m_readWatchers[address];
	if (watchers.empty())
		return;

	for (std::vector<MemoryWatcher *>::const_iterator it = watchers.begin(); it != watchers.end(); ++it) {
		(*it)->handleMemoryRead(this, address, value);
	}
}

void Memory::callReadWatcher(uint16_t address, uint8_t &value) {
	std::vector<MemoryWatcher *> &watchers = m_readWatchers[address];
	if (watchers.empty())
		return;

	for (std::vector<MemoryWatcher *>::const_iterator it = watchers.begin(); it != watchers.end(); ++it) {
		(*it)->handleMemoryRead(this, address, value);
	}
}

void Memory::set(uint16_t address, uint16_t value) {
	uint8_t *ptr2 = (uint8_t *) &value;
	m_memory[address] = *(ptr2 + 1);
	m_memory[address + 1] = *ptr2;

	callWatcher(address);
	callWatcher(address + 1);
}

void Memory::setBigEndian(uint16_t address, uint16_t value, bool watchers) {
	uint8_t *ptr2 = (uint8_t *) &value;
	m_memory[address] = *(ptr2);
	m_memory[address + 1] = *(ptr2 + 1);

	if (watchers) {
		callWatcher(address);
		callWatcher(address + 1);
	}
}

uint8_t Memory::getByte(uint16_t address) {
	uint8_t r = m_memory[address];
	callReadWatcher(address, r);
	return r;
}

void Memory::setByte(uint16_t address, uint8_t value) {
	m_memory[address] = value;
	callWatcher(address);
}

void Memory::addWatcher(uint16_t address, MemoryWatcher *watcher, Mode mode) {
	switch (mode) {
		case Read:
			m_readWatchers[address].push_back(watcher);
			break;
		case Write:
			m_watchers[address].push_back(watcher);
			break;
		case ReadWrite:
			m_readWatchers[address].push_back(watcher);
			m_watchers[address].push_back(watcher);
			break;
	}
}

bool Memory::isBitSet(uint16_t address, uint16_t bit) {
	return m_memory[address] & bit;
}

void Memory::setBit(uint16_t address, uint16_t bit, bool value) {
	if (value) {
		m_memory[address] = m_memory[address] | bit;
	}
	else {
		m_memory[address] = m_memory[address] & (~bit);
	}
}

void Memory::setBitWatcher(uint16_t address, uint16_t bit, bool value) {
	setBit(address, bit, value);
	callWatcher(address);
}

}
