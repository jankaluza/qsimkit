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

#include "CPU/Memory/RegisterSet.h"
#include "CPU/Memory/Register.h"

RegisterSet::RegisterSet() {

}

RegisterSet::~RegisterSet() {
	for (std::vector<Register *>::iterator it = m_registers.begin(); it != m_registers.end(); ++it) {
		delete (*it);
	}
}

void RegisterSet::addRegister(const std::string &name, uint16_t value, const std::string &desc) {
	Register *reg = new Register(name, value, desc);
	m_registers.push_back(reg);
}

Register *RegisterSet::operator[](unsigned int reg) {
	return m_registers[reg];
}
