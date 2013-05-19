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
			 ACLK *aclk, SMCLK *smclk, uint16_t tactl, uint16_t tar,
			 uint16_t taiv) :
m_intManager(intManager), m_mem(mem), m_variant(variant), m_source(0),
m_aclk(aclk), m_smclk(smclk), m_up(true), m_tactl(tactl), m_tar(tar),
m_taiv(taiv) {
#define ADD_WATCHER(METHOD) \
	if (METHOD != 0) { m_mem->addWatcher(METHOD, this); }
	ADD_WATCHER(m_variant->getBCSCTL2());

	reset();
}

Timer::~Timer() {

}

void Timer::addCCR(uint16_t tacctl, uint16_t taccr) {
	CCR ccr;
	ccr.tacctl = tacctl;
	ccr.taccr = taccr;
	m_ccr.push_back(ccr);
}

void Timer::checkCCRInterrupts(uint16_t tar) {
	for (int i = 1; i < m_ccr.size(); ++i) {
		uint16_t ccr = m_mem->getBigEndian(m_ccr[i].taccr);
		bool interrupt_enabled = m_mem->isBitSet(m_ccr[i].tacctl, 16);
		if (interrupt_enabled && ccr == tar) {
			m_mem->setBigEndian(m_taiv, 2 * i);
			m_mem->setBit(m_ccr[i].tacctl, 1, true);
			m_intManager->queueInterrupt(m_variant->getTIMERA1_VECTOR());
		}
	}
}

void Timer::changeTAR(uint8_t mode) {
	uint16_t tar = m_mem->getBigEndian(m_tar);
	uint16_t ccr0 = m_mem->getBigEndian(m_ccr[0].taccr);
	bool interrupt_enabled = m_mem->isBitSet(m_ccr[0].tacctl, 16);

	switch (mode) {
		case TIMER_STOPPED:
			break;
		case TIMER_UP:
			if (ccr0 == 0) {
				break;
			}
			if (tar == ccr0) {
				// set TAIFG
				m_mem->setBigEndian(m_tar, 0);
				if (interrupt_enabled) {
					m_mem->setBit(m_taiv, 4, true);
					m_intManager->queueInterrupt(m_variant->getTIMERA1_VECTOR());
				}
			}
			else {
				tar += 1;
				if (interrupt_enabled && tar == ccr0) {
					// set TACCR0 CCIFG
					m_mem->setBit(m_ccr[0].tacctl, 1, true);
					m_intManager->queueInterrupt(m_variant->getTIMERA0_VECTOR());
				}
				m_mem->setBigEndian(m_tar, tar);
			}
			checkCCRInterrupts(tar);
			break;
		case TIMER_CONTINUOUS:
			m_mem->setBigEndian(m_tar, tar + 1);
			break;
		case TIMER_UPDOWN:
			if (m_up) {
				m_mem->setBigEndian(m_tar, tar + 1);
			}
			else {
				m_mem->setBigEndian(m_tar, tar - 1);
			}
			break;
		default:
			break;
	}
}

void Timer::tick() {
	uint8_t mode = (m_mem->getByte(m_tactl) >> 4) & 3;
	changeTAR(mode);

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
