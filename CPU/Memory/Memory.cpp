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

static unsigned int hexToInt(const std::string &str) {
	unsigned int x;
	std::stringstream ss;
	ss << std::hex << str;
	ss >> x;
	return x;
}

Memory::Memory(unsigned int size) {
	m_memory.resize(size);
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
				reg->get(0)->set(default_ip);
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
	return w;
}

uint16_t Memory::getBigEndian(uint16_t address) {
	uint16_t w;
	uint8_t *ptr = (uint8_t *) &w;
	*ptr++ = m_memory[address];
	*ptr++ = m_memory[address + 1];
	return w;
}

void Memory::set(uint16_t address, uint16_t value) {
	uint8_t *ptr2 = (uint8_t *) &value;
	m_memory[address] = *(ptr2 + 1);
	m_memory[address + 1] = *ptr2;
}

void Memory::setBigEndian(uint16_t address, uint16_t value) {
	uint8_t *ptr2 = (uint8_t *) &value;
	m_memory[address] = *(ptr2);
	m_memory[address + 1] = *(ptr2 + 1);
}

uint8_t Memory::getByte(uint16_t address) {
	return m_memory[address];
}

void Memory::setByte(uint16_t address, uint8_t value) {
	m_memory[address] = value;
}

