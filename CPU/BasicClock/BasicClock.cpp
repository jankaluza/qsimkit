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

#include "BasicClock.h"
#include "CPU/Variants/Variant.h"
#include "CPU/Memory/Memory.h"
#include "CPU/Interrupts/InterruptManager.h"
#include <iostream>

#include "VLO.h"
#include "LFXT1.h"
#include "ACLK.h"
#include "SMCLK.h"
#include "DCO.h"
#include "MCLK.h"
#include "TimerFactory.h"
#include "Timer.h"

namespace MCU {

BasicClock::BasicClock(Memory *mem, Variant *variant,
					   InterruptManager *intManager, TimerFactory *factory) :
m_mem(mem), m_variant(variant), m_intManager(intManager), m_factory(factory) {

	m_vlo = new VLO();
	m_lfxt1 = new LFXT1(m_mem, m_variant);
	m_aclk = new ACLK(m_mem, m_variant, m_vlo, m_lfxt1);
	m_smclk = new SMCLK(m_mem, m_variant, m_dco);
	m_dco = new DCO(m_mem, m_variant);
	m_mclk = new MCLK(m_mem, m_variant, m_dco, m_vlo, m_lfxt1);
	m_timerA = m_factory->createTimer(m_intManager, m_mem, m_variant, m_aclk, m_smclk,
									  m_variant->getTA0CTL(), m_variant->getTA0R(),
									  m_variant->getTA0IV());

	m_timerA->addCCR(m_variant->getTA0CCTL0(), m_variant->getTA0CCR0());
	m_timerA->addCCR(m_variant->getTA0CCTL1(), m_variant->getTA0CCR1());
}

BasicClock::~BasicClock() {
	delete m_vlo;
	delete m_lfxt1;
	delete m_aclk;
	delete m_smclk;
	delete m_dco;
	delete m_mclk;
	delete m_timerA;
}

void BasicClock::reset() {
	m_dco->reset();
	m_mclk->reset();
}

}
