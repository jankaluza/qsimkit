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
#include "PinMultiplexer.h"
#include "GPPinHandler.h"
#include "CPU/Variants/Variant.h"
#include "CPU/Memory/Memory.h"
#include "CPU/Interrupts/InterruptManager.h"
#include <iostream>

namespace MSP430 {

PinManager::PinManager(Memory *mem, InterruptManager *intManager, Variant *variant) :
m_mem(mem), m_intManager(intManager), m_variant(variant) {

}

PinManager::~PinManager() {

}

#define CREATE_MPX_AND_HANDLER_WITH_INT(TYPE, INDEX, VEC) {\
	mpx = new PinMultiplexer(this, m_multiplexers.size(), m_mem, m_variant, m_variant->get##TYPE##DIR(), \
							m_variant->get##TYPE##SEL(), subtype); \
	handler = new GPPinHandler(m_mem, mpx, m_intManager, m_variant->get##TYPE##DIR(), \
				m_variant->get##TYPE##IN(), m_variant->get##TYPE##OUT(), \
				m_variant->get##TYPE##IE(), m_variant->get##TYPE##IES(), \
				m_variant->get##TYPE##IFG(), VEC, subtype);\
	mpx->addPinHandler("GP", handler); \
}

#define CREATE_MPX_AND_HANDLER(TYPE, INDEX) {\
	mpx = new PinMultiplexer(this, m_multiplexers.size(), m_mem, m_variant, m_variant->get##TYPE##DIR(), \
							m_variant->get##TYPE##SEL(), subtype); \
	handler = new GPPinHandler(m_mem, mpx, m_intManager, m_variant->get##TYPE##DIR(), \
				m_variant->get##TYPE##IN(), m_variant->get##TYPE##OUT(), \
				0, 0, 0, 0, subtype);\
	mpx->addPinHandler("GP", handler); \
}

PinMultiplexer *PinManager::addPin(PinType type, int subtype) {
	PinMultiplexer *mpx = 0;
	GPPinHandler *handler = 0;
	switch (type) {
		case P1:
			CREATE_MPX_AND_HANDLER_WITH_INT(P1, subtype, m_variant->getPORT1_VECTOR());
			break;
		case P2:
			CREATE_MPX_AND_HANDLER_WITH_INT(P2, subtype, m_variant->getPORT2_VECTOR());
			break;
		case P3:
			CREATE_MPX_AND_HANDLER(P3, subtype);
			break;
		case P4:
			CREATE_MPX_AND_HANDLER(P4, subtype);
			break;
		case P5:
			CREATE_MPX_AND_HANDLER(P5, subtype);
			break;
		case P6:
			CREATE_MPX_AND_HANDLER(P6, subtype);
			break;
		case P7:
			CREATE_MPX_AND_HANDLER(P7, subtype);
			break;
		case P8:
			CREATE_MPX_AND_HANDLER(P8, subtype);
			break;
		default:
			break;
	}

	m_multiplexers.push_back(mpx);
	return mpx;
}

std::vector<PinMultiplexer *> PinManager::addPinHandler(const std::string &name, PinHandler *handler) {
	std::vector<PinMultiplexer *> multiplexers;
	for (std::vector<PinMultiplexer *>::iterator it = m_multiplexers.begin(); it != m_multiplexers.end(); ++it) {
		if ((*it) && (*it)->hasMultiplexing(name)) {
			(*it)->addPinHandler(name, handler);
			multiplexers.push_back(*it);
		}
	}

	return multiplexers;
}

bool PinManager::handlePinInput(int id, double value) {
	if (m_multiplexers[id])
		m_multiplexers[id]->handleInput(value);
	return true;
}

void PinManager::generateOutput(int id, double value) {
	m_watcher->handlePinChanged(id, value);
}

void PinManager::reset() {
	m_multiplexers.clear();
}

}
