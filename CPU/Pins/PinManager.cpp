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

#include "PinManager.h"
#include "CPU/Variants/Variant.h"
#include "CPU/Memory/Memory.h"
#include "CPU/Interrupts/InterruptManager.h"
#include <iostream>

namespace MCU {

PinManager::PinManager(Memory *mem, Variant *variant) : m_mem(mem), m_variant(variant) {
	setMemory(mem);
}

PinManager::~PinManager() {

}

#define REFRESH_GP(TYPE, VAR, ADDRESS) { \
	uint16_t b = m_mem->getBigEndian(ADDRESS);\
	uint8_t dir =  m_mem->getByte(m_variant->get##TYPE##DIR()); \
	for (int i = 0; i < 8; ++i) { \
		if (!(dir & (1 << i))) continue; \
		int pin_id = m_gpCache[((int) TYPE) * 10 + i]; \
		if (b & (1 << i)) { \
			if (m_pins[pin_id].value != 1.0) { \
				m_pins[pin_id].value = 1.0; \
				m_watcher->handlePinChanged(pin_id, 1.0); \
			} \
		} \
		else { \
			if (m_pins[pin_id].value != 0) { \
				m_pins[pin_id].value = 0; \
				m_watcher->handlePinChanged(pin_id, 0.0); \
			} \
		} \
	} \
}

void PinManager::handleMemoryChanged(Memory *memory, uint16_t address) {
	if (address == m_variant->getP1OUT()) {
		REFRESH_GP(P1, in, m_variant->getP1OUT());
	}
	else if (address == m_variant->getP2OUT()) {
		REFRESH_GP(P2, in, m_variant->getP2OUT());
	}
	else if (address == m_variant->getP3OUT()) {
		REFRESH_GP(P3, in, m_variant->getP3OUT());
	}
	else if (address == m_variant->getP4OUT()) {
		REFRESH_GP(P4, in, m_variant->getP4OUT());
	}
	else if (address == m_variant->getP5OUT()) {
		REFRESH_GP(P5, in, m_variant->getP5OUT());
	}
	else if (address == m_variant->getP6OUT()) {
		REFRESH_GP(P6, in, m_variant->getP6OUT());
	}
	else if (address == m_variant->getP7OUT()) {
		REFRESH_GP(P7, in, m_variant->getP7OUT());
	}
	else if (address == m_variant->getP8OUT()) {
		REFRESH_GP(P8, in, m_variant->getP8OUT());
	}
}

void PinManager::setMemory(Memory *mem) {
	m_mem = mem;
	if (!m_mem) {
		return;
	}

#define ADD_WATCHER(METHOD) \
	if (METHOD != 0) { m_mem->addWatcher(METHOD, this); }

	ADD_WATCHER(m_variant->getP1OUT());
	ADD_WATCHER(m_variant->getP2OUT());
	ADD_WATCHER(m_variant->getP3OUT());
	ADD_WATCHER(m_variant->getP4OUT());
	ADD_WATCHER(m_variant->getP5OUT());
	ADD_WATCHER(m_variant->getP6OUT());
	ADD_WATCHER(m_variant->getP7OUT());
	ADD_WATCHER(m_variant->getP8OUT());
	ADD_WATCHER(m_variant->getP9OUT());
	ADD_WATCHER(m_variant->getP10OUT());

#undef ADD_WATCHER
}

void PinManager::addPin(PinType type, int subtype) {
	m_pins.push_back(InternalPin(type, subtype));

	switch (type) {
		case P1: case P2: case P3: case P4: case P5:
		case P6: case P7: case P8:
			m_gpCache[((int) type * 10) + subtype] = m_pins.size() - 1;
			break;
		default:
			break;
	}
}

bool PinManager::handlePinInput(int id, double value) {
	InternalPin &p = m_pins[id];
	bool handled = false;

	// Checks if the pin is input and stores the value in the
	// proper address in memory
#define SET_GP_IN(PIN, I, VALUE) \
	if (!(m_mem->getByte(m_variant->get##PIN##DIR()) & (1 << I))) { \
		m_mem->setBit(m_variant->get##PIN##IN(), 1 << I, VALUE == 1); \
		handled = true; \
	}

	// 1. Check if interrupts are enabled for given pin (PxIE)
	// 2. Check if the condition for interrupt was met (PxIES)
	// 3. Set the PxIFG value according to given pin
	// 4. Inform InterruptManager that we have interrupt here.
#define RUN_GP_INTERRUPT(PIN, PORT, I, VALUE) \
	if (m_mem->getByte(m_variant->get##PIN##IE()) & (1 << I)) { \
		/* Interrupt enabled */ \
		if (m_mem->getByte(m_variant->get##PIN##IES()) & (1 << I)) { \
			/* Low to high */ \
			if (m_pins[id].value == 0 && value == 1.0) { \
				m_mem->setBit(m_variant->get##PIN##IFG(), 1 << I, 1); \
				m_intManager->queueInterrupt(m_variant->get##PORT##_VECTOR()); \
			}\
		} \
		else { \
			/* High to low */ \
			std::cerr << "xxxxx\n"; \
			if (m_pins[id].value == 1.0 && value == 0) { \
				m_mem->setBit(m_variant->get##PIN##IFG(), 1 << I, 1); \
				m_intManager->queueInterrupt(m_variant->get##PORT##_VECTOR()); \
			}\
		} \
	} \
	else { std::cerr << "IE NOT ENABLED " << (int) m_mem->getByte(m_variant->get##PIN##IE()) << "\n"; }

	switch (p.type) {
		case P1:
			SET_GP_IN(P1, p.subtype, value)
			RUN_GP_INTERRUPT(P1, PORT1, p.subtype, value)
			break;
		case P2:
			SET_GP_IN(P2, p.subtype, value)
			RUN_GP_INTERRUPT(P2, PORT2, p.subtype, value);
			break;
		case P3:
			SET_GP_IN(P3, p.subtype, value)
			break;
		case P4:
			SET_GP_IN(P4, p.subtype, value)
			break;
		case P5:
			SET_GP_IN(P5, p.subtype, value)
			break;
		case P6:
			SET_GP_IN(P6, p.subtype, value)
			break;
		case P7:
			SET_GP_IN(P7, p.subtype, value)
			break;
		case P8:
			SET_GP_IN(P8, p.subtype, value)
		default:
			break;
	}

	m_pins[id].value = value;
	return handled;
}

}
