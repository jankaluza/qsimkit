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

Register::Register(const std::string &name, uint16_t value, const std::string &desc) :
	m_value(value), m_name(name), m_desc(desc) { }

Register::~Register() {

}

void Register::set(uint16_t value) {
	m_value = value;
}

Register *Register::operator=(uint16_t value) {
	m_value = value;
	return this;
}

uint16_t Register::get() {
	return m_value;
}
