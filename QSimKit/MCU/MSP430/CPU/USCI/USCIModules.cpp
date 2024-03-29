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

#include "USCIModules.h"
#include "USCI.h"
#include "CPU/Variants/Variant.h"
#include "CPU/Memory/Memory.h"
#include "CPU/Interrupts/InterruptManager.h"
#include "CPU/Pins/PinManager.h"
#include "CPU/Pins/PinMultiplexer.h"
#include "CPU/BasicClock/ACLK.h"
#include "CPU/BasicClock/SMCLK.h"
#include <iostream>

namespace MSP430 {

USCIModules::USCIModules(PinManager *pinManager, InterruptManager *intManager, Memory *mem, Variant *variant,
						 ACLK *aclk, SMCLK *smclk) {

	if (variant->getUCA0CTL0() != 0) {
		m_usci.push_back(new USCI(pinManager, intManager, mem, variant, USCI::USCI_A, 0, aclk, smclk));
	}

	if (variant->getUCA1CTL0() != 0) {
		m_usci.push_back(new USCI(pinManager, intManager, mem, variant, USCI::USCI_A, 1, aclk, smclk));
	}

	if (variant->getUCB0CTL0() != 0) {
		m_usci.push_back(new USCI(pinManager, intManager, mem, variant, USCI::USCI_B, 0, aclk, smclk));
	}

	if (variant->getUCB1CTL0() != 0) {
		m_usci.push_back(new USCI(pinManager, intManager, mem, variant, USCI::USCI_B, 1, aclk, smclk));
	}
}

USCIModules::~USCIModules() {
	for (std::vector<USCI *>::iterator it = m_usci.begin(); it != m_usci.end(); ++it) {
		delete *it;
	}
}

void USCIModules::reset() {
	for (std::vector<USCI *>::iterator it = m_usci.begin(); it != m_usci.end(); ++it) {
		(*it)->reset();
	}
}

}
