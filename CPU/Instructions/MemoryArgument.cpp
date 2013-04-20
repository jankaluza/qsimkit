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

#include "CPU/Instructions/MemoryArgument.h"
#include "CPU/Memory/Memory.h"

#include <iostream>
#include <sstream>

namespace MCU {

MemoryArgument::MemoryArgument(Memory *mem, uint16_t address) :
	m_mem(mem), m_address(address) { }

MemoryArgument::~MemoryArgument() {

}

uint16_t MemoryArgument::get() {
	return m_mem->get(m_address);
}

uint16_t MemoryArgument::getBigEndian() {
	return m_mem->getBigEndian(m_address);
}

void MemoryArgument::set(uint16_t value) {
	m_mem->set(m_address, value);
}

void MemoryArgument::setBigEndian(uint16_t value) {
	m_mem->setBigEndian(m_address, value);
}

uint8_t MemoryArgument::getByte() {
	m_mem->getByte(m_address);
}

void MemoryArgument::setByte(uint8_t value) {
	m_mem->setByte(m_address, value);
}

}
