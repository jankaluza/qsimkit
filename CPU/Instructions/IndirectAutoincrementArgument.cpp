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

#include "CPU/Instructions/IndirectAutoincrementArgument.h"
#include "CPU/Memory/Register.h"
#include "CPU/Memory/Memory.h"

#include <iostream>
#include <sstream>

IndirectAutoincrementArgument::IndirectAutoincrementArgument(Memory *mem, Register *reg, bool bw) :
	m_reg(reg), m_bw(bw), m_mem(mem) { }

IndirectAutoincrementArgument::~IndirectAutoincrementArgument() {

}

uint16_t IndirectAutoincrementArgument::get() {
	uint16_t r = m_mem->get(m_reg->getBigEndian());
	m_reg->setBigEndian(m_reg->getBigEndian() + (m_bw ? 1 : 2));
	return r;
}

uint16_t IndirectAutoincrementArgument::getBigEndian() {
	uint16_t r = m_mem->getBigEndian(m_reg->getBigEndian());
	m_reg->setBigEndian(m_reg->getBigEndian() + (m_bw ? 1 : 2));
	return r;
}

void IndirectAutoincrementArgument::set(uint16_t value) {

}

void IndirectAutoincrementArgument::setBigEndian(uint16_t value) {

}

uint8_t IndirectAutoincrementArgument::getByte() {
	uint8_t r = m_mem->getByte(m_reg->getBigEndian());
	m_reg->setBigEndian(m_reg->getBigEndian() + (m_bw ? 1 : 2));
	return r;
}

void IndirectAutoincrementArgument::setByte(uint8_t value) {

}

