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

#include "USARTModules.h"
#include "USART.h"
#include "CPU/Variants/Variant.h"
#include "CPU/Memory/Memory.h"
#include "CPU/Interrupts/InterruptManager.h"
#include "CPU/Pins/PinManager.h"
#include "CPU/Pins/PinMultiplexer.h"
#include "CPU/BasicClock/ACLK.h"
#include "CPU/BasicClock/SMCLK.h"
#include <iostream>

namespace MSP430 {

USARTModules::USARTModules(PinManager *pinManager, InterruptManager *intManager, Memory *mem, Variant *variant,
						 ACLK *aclk, SMCLK *smclk) {

	if (variant->getU0CTL() != 0) {
		m_usart.push_back(new USART(pinManager, intManager, mem, variant, 0, aclk, smclk));
	}

	if (variant->getU1CTL() != 0) {
		m_usart.push_back(new USART(pinManager, intManager, mem, variant, 1, aclk, smclk));
	}
}

USARTModules::~USARTModules() {
	for (std::vector<USART *>::iterator it = m_usart.begin(); it != m_usart.end(); ++it) {
		delete *it;
	}
}

}
