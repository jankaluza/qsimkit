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

namespace MSP430 {

BasicClock::BasicClock(Memory *mem, Variant *variant,
					   InterruptManager *intManager, PinManager *pinManager, TimerFactory *factory) :
m_mem(mem), m_variant(variant), m_intManager(intManager), m_factory(factory),
m_pinManager(pinManager) {

	uint16_t intvec0;
	uint16_t intvec1;
	m_vlo = new VLO();
	m_lfxt1 = new LFXT1(m_mem, m_variant);
	m_aclk = new ACLK(m_mem, m_variant, m_vlo, m_lfxt1);
	m_dco = new DCO(m_mem, m_variant);
	m_smclk = new SMCLK(m_mem, m_variant, m_dco);
	m_mclk = new MCLK(m_mem, m_variant, m_dco, m_vlo, m_lfxt1);

	// add TA0
	intvec0 = m_variant->getTIMERA0_VECTOR();
	if (intvec0 == 0) {
		intvec0 = m_variant->getTIMER0_A0_VECTOR();
	}

	intvec1 = m_variant->getTIMERA1_VECTOR();
	if (intvec1 == 0) {
		intvec1 = m_variant->getTIMER0_A1_VECTOR();
	}

	m_timerA0 = m_factory->createTimer(Timer::TimerA, m_pinManager, m_intManager, m_mem, m_variant, m_aclk, m_smclk,
									  m_variant->getTA0CTL(), m_variant->getTA0R(),
									  m_variant->getTA0IV(), intvec0, intvec1);
	if (m_variant->getTA0CCTL0() != 0) {
		m_timerA0->addCCR("TA0.0", "TA0.CCI0A", "TA0.CCI0B", m_variant->getTA0CCTL0(), m_variant->getTA0CCR0());
	}
	if (m_variant->getTA0CCTL1() != 0) {
		m_timerA0->addCCR("TA0.1", "TA0.CCI1A", "TA0.CCI1B", m_variant->getTA0CCTL1(), m_variant->getTA0CCR1());
	}
	if (m_variant->getTA0CCTL2() != 0) {
		m_timerA0->addCCR("TA0.2", "TA0.CCI2A", "TA0.CCI2B", m_variant->getTA0CCTL2(), m_variant->getTA0CCR2());
	}
	if (m_variant->getTA0CCTL3() != 0) {
		m_timerA0->addCCR("TA0.3", "TA0.CCI3A", "TA0.CCI3B", m_variant->getTA0CCTL3(), m_variant->getTA0CCR3());
	}
	if (m_variant->getTA0CCTL4() != 0) {
		m_timerA0->addCCR("TA0.4", "TA0.CCI4A", "TA0.CCI4B", m_variant->getTA0CCTL4(), m_variant->getTA0CCR4());
	}

	// add TA1
	intvec0 = m_variant->getTIMER1_A0_VECTOR();
	intvec1 = m_variant->getTIMER1_A1_VECTOR();
	if (intvec0 != 0 && intvec1 != 0) {
		m_timerA1 = m_factory->createTimer(Timer::TimerA, m_pinManager, m_intManager, m_mem, m_variant, m_aclk, m_smclk,
										m_variant->getTA1CTL(), m_variant->getTA1R(),
										m_variant->getTA1IV(), intvec0, intvec1);
		if (m_variant->getTA0CCTL0() != 0) {
			m_timerA1->addCCR("TA1.0", "TA1.CCI0A", "TA1.CCI0B", m_variant->getTA1CCTL0(), m_variant->getTA1CCR0());
		}
		if (m_variant->getTA0CCTL1() != 0) {
			m_timerA1->addCCR("TA1.1", "TA1.CCI1A", "TA1.CCI1B", m_variant->getTA1CCTL1(), m_variant->getTA1CCR1());
		}
		if (m_variant->getTA0CCTL2() != 0) {
			m_timerA1->addCCR("TA1.2", "TA1.CCI2A", "TA1.CCI2B", m_variant->getTA1CCTL2(), m_variant->getTA1CCR2());
		}
		if (m_variant->getTA0CCTL3() != 0) {
			m_timerA1->addCCR("TA1.3", "TA1.CCI3A", "TA1.CCI3B", m_variant->getTA1CCTL3(), m_variant->getTA1CCR3());
		}
		if (m_variant->getTA0CCTL4() != 0) {
			m_timerA1->addCCR("TA1.4", "TA1.CCI4A", "TA1.CCI4B", m_variant->getTA1CCTL4(), m_variant->getTA1CCR4());
		}
	}
	else {
		m_timerA1 = 0;
	}

	// add TB
	intvec0 = m_variant->getTIMERB0_VECTOR();
	intvec1 = m_variant->getTIMERB1_VECTOR();
	if (intvec0 != 0 && intvec1 != 0) {
		m_timerB = m_factory->createTimer(Timer::TimerB, m_pinManager, m_intManager, m_mem, m_variant, m_aclk, m_smclk,
										m_variant->getTBCTL(), m_variant->getTBR(),
										m_variant->getTBIV(), intvec0, intvec1);
		if (m_variant->getTBCCTL0() != 0) {
			m_timerB->addCCR("TB0.0", "TB0.CCI0A", "TB0.CCI0B", m_variant->getTBCCTL0(), m_variant->getTBCCR0());
		}
		if (m_variant->getTBCCTL1() != 0) {
			m_timerB->addCCR("TB0.1", "TB0.CCI1A", "TB0.CCI1B", m_variant->getTBCCTL1(), m_variant->getTBCCR1());
		}
		if (m_variant->getTBCCTL2() != 0) {
			m_timerB->addCCR("TB0.2", "TB0.CCI2A", "TB0.CCI2B", m_variant->getTBCCTL2(), m_variant->getTBCCR2());
		}
		if (m_variant->getTBCCTL3() != 0) {
			m_timerB->addCCR("TB0.3", "TB0.CCI3A", "TB0.CCI3B", m_variant->getTBCCTL3(), m_variant->getTBCCR3());
		}
		if (m_variant->getTBCCTL4() != 0) {
			m_timerB->addCCR("TB0.4", "TB0.CCI4A", "TB0.CCI4B", m_variant->getTBCCTL4(), m_variant->getTBCCR4());
		}
		if (m_variant->getTBCCTL5() != 0) {
			m_timerB->addCCR("TB0.5", "TB0.CCI5A", "TB0.CCI5B", m_variant->getTBCCTL5(), m_variant->getTBCCR5());
		}
		if (m_variant->getTBCCTL6() != 0) {
			m_timerB->addCCR("TB0.6", "TB0.CCI6A", "TB0.CCI6B", m_variant->getTBCCTL6(), m_variant->getTBCCR6());
		}
	}
	else {
		m_timerB = 0;
	}
}

BasicClock::~BasicClock() {
	delete m_vlo;
	delete m_lfxt1;
	delete m_aclk;
	delete m_smclk;
	delete m_dco;
	delete m_mclk;
	delete m_timerA0;
	delete m_timerA1;
	delete m_timerB;
}

void BasicClock::reset() {
	m_dco->reset();
	m_mclk->reset();
}

}
