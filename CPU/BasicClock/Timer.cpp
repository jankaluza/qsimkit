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

#include "Timer.h"
#include "CPU/Variants/Variant.h"
#include "CPU/Memory/Memory.h"
#include "CPU/Interrupts/InterruptManager.h"
#include <iostream>

#include "ACLK.h"

#define TIMER_STOPPED 0
#define TIMER_UP 1
#define TIMER_CONTINUOUS 2
#define TIMER_UPDOWN 3

namespace MCU {

Timer::Timer(InterruptManager *intManager, Memory *mem, Variant *variant,
			 ACLK *aclk, SMCLK *smclk) :
m_intManager(intManager), m_mem(mem), m_variant(variant), m_source(0),
m_aclk(aclk), m_smclk(smclk), m_up1(true), m_up2(true) {
#define ADD_WATCHER(METHOD) \
	if (METHOD != 0) { m_mem->addWatcher(METHOD, this); }
	ADD_WATCHER(m_variant->getBCSCTL2());

	reset();
}

Timer::~Timer() {

}

void Timer::changeTAR(uint16_t address, uint8_t mode, bool &up, uint16_t taccr_, uint16_t tacctl) {
	uint16_t tar = m_mem->getBigEndian(address);
	uint16_t taccr = m_mem->getBigEndian(taccr_);
	switch (mode) {
		case TIMER_STOPPED:
			break;
		case TIMER_UP:
			if (taccr == 0) {
				break;
			}
			if (tar == taccr) {
				// set TAIFG
				m_mem->setBigEndian(address, 0);
				m_mem->setBit(m_variant->getTA0IV(), 4, true);
				
				m_intManager->queueInterrupt(m_variant->getTIMERA1_VECTOR());
			}
			else {
				tar += 1;
				if (tar == taccr) {
					// set TACCR0 CCIFG
					m_mem->setBit(tacctl, 1, true);
					m_intManager->queueInterrupt(m_variant->getTIMERA0_VECTOR());
				}
				m_mem->setBigEndian(address, tar);
			}
			break;
		case TIMER_CONTINUOUS:
			m_mem->setBigEndian(address, m_mem->getBigEndian(address) + 1);
			break;
		case TIMER_UPDOWN:
			if (up) {
				m_mem->setBigEndian(address, m_mem->getBigEndian(address) + 1);
			}
			else {
				m_mem->setBigEndian(address, m_mem->getBigEndian(address) - 1);
			}
			break;
		default:
			break;
	}
}

void Timer::tick() {
	uint8_t mode = (m_mem->getByte(m_variant->getTA0CTL()) >> 4) & 3;
	uint16_t address = m_variant->getTA0R();
	uint16_t taccr = m_variant->getTA0CCR0();
	uint16_t tacctl = m_variant->getTA0CCTL0();

	changeTAR(address, mode, m_up1, taccr, tacctl);

// 	mode = (m_mem->getByte(m_variant->getTA1CTL()) >> 4) & 3;
// 	if (mode) {
// 		taccr = m_variant->getTA1CCR0_();
// 		address = m_variant->getTA1R();
// 		changeTAR(address, mode, m_up2);
// 	}
}

unsigned long Timer::getFrequency() {
	return m_source->getFrequency();
}

double Timer::getStep() {
	return m_source->getStep();
}

void Timer::reset() {
	m_source = m_aclk;
}


void Timer::handleMemoryChanged(Memory *memory, uint16_t address) {
	// Set divider and source
}

}
