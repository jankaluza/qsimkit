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

#include "CPU/Instructions/IndexedArgument.h"
#include "CPU/Memory/Register.h"
#include "CPU/Memory/Memory.h"

namespace MSP430 {

IndexedArgument::IndexedArgument(Memory *mem, Register *reg, uint16_t offset) :
	m_reg(reg), m_offset(offset), m_mem(mem) { }

IndexedArgument::~IndexedArgument() {

}

uint16_t IndexedArgument::get() {
	return m_mem->get(m_reg->getBigEndian() + m_offset);
}

uint16_t IndexedArgument::getBigEndian() {
	return m_mem->getBigEndian(m_reg->getBigEndian() + m_offset);
}

void IndexedArgument::set(uint16_t value) {
	m_mem->set(m_reg->getBigEndian() + m_offset, value);
}

void IndexedArgument::setBigEndian(uint16_t value) {
	m_mem->setBigEndian(m_reg->getBigEndian() + m_offset, value);
}

uint8_t IndexedArgument::getByte() {
	return m_mem->getByte(m_reg->getBigEndian() + m_offset);
}

void IndexedArgument::setByte(uint8_t value) {
	m_mem->setByte(m_reg->getBigEndian() + m_offset, value);
}

}
