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
#include "CPU/Pins/PinManager.h"
#include "CPU/Pins/PinMultiplexer.h"
#include <iostream>

#include "ACLK.h"
#include "SMCLK.h"

#define TIMER_STOPPED 0
#define TIMER_UP 1
#define TIMER_CONTINUOUS 2
#define TIMER_UPDOWN 3

namespace MSP430 {

Timer::Timer(Type type, PinManager *pinManager, InterruptManager *intManager, Memory *mem, Variant *variant,
			 ACLK *aclk, SMCLK *smclk, uint16_t tactl, uint16_t tar,
			 uint16_t taiv, uint16_t intvect0, uint16_t intvect1) :
m_pinManager(pinManager), m_intManager(intManager), m_mem(mem), m_variant(variant), m_source(0),
m_divider(1), m_aclk(aclk), m_smclk(smclk), m_up(true), m_tactl(tactl),
m_tar(tar), m_taiv(taiv), m_intvect0(intvect0), m_intvect1(intvect1),
m_type(type), m_counterMax(0xffff), m_counter(0) {

	m_mem->addWatcher(tactl, this);
	m_mem->addWatcher(taiv, this, MemoryWatcher::Read);

	m_intManager->addWatcher(m_intvect0, this);
	m_intManager->addWatcher(m_intvect1, this);

	reset();
}

Timer::~Timer() {

}

void Timer::addCCR(const std::string &taName, const std::string &cciaName, const std::string &ccibName, uint16_t tacctl, uint16_t taccr) {
	m_cciNames[cciaName] = m_ccr.size();
	m_cciNames[ccibName] = m_ccr.size();

	std::vector<PinMultiplexer *> mpxs;
	mpxs = m_pinManager->addPinHandler(taName, this);

	CCR ccr;
	ccr.tacctl = tacctl;
	ccr.taccr = taccr;
	ccr.outputMpxs = mpxs;
	ccr.ccia = cciaName;
	ccr.ccib = ccibName;
	ccr.taName = taName;
	ccr.capturePending = false;
	ccr.ccrRead = true;
	ccr.ccis = 0;

	mpxs = m_pinManager->addPinHandler(cciaName, this);
	ccr.cciaMpx = mpxs.empty() ? 0 : mpxs[0];
	mpxs = m_pinManager->addPinHandler(ccibName, this);
	ccr.ccibMpx = mpxs.empty() ? 0 : mpxs[0];

	m_ccr.push_back(ccr);

	// Setup memory watcher for ccr so we can generate Capture overflow (COV).
	m_mem->addWatcher(taccr, this, MemoryWatcher::ReadWrite);
	// Setup memory watcher for cctl so we can change output according to OUT
	m_mem->addWatcher(tacctl, this, MemoryWatcher::Write);
}

void Timer::doOutput(CCR &ccr, uint16_t tacctl, bool ccr0_interrupt) {
	// generate output bit according to OUTMODx
	switch((tacctl >> 5) & 7) {
		case 0:
			// Mode = Output
			break;
		case 1:
			// Mode = Set
			if (ccr0_interrupt) {
				break;
			}
			m_mem->setBitWatcher(ccr.tacctl, 4, true);
			break;
		case 2:
			// Mode = Toggle/Reset
			if (ccr0_interrupt) {
				m_mem->setBitWatcher(ccr.tacctl, 4, false);
			}
			else {
				m_mem->setBitWatcher(ccr.tacctl, 4, !(tacctl & 4));
			}
			break;
		case 3:
			// Mode = Set/Reset
			if (ccr0_interrupt) {
				m_mem->setBitWatcher(ccr.tacctl, 4, false);
			}
			else {
				m_mem->setBitWatcher(ccr.tacctl, 4, true);
			}
			break;
		case 4:
			// Mode = Toggle
			if (ccr0_interrupt) {
				break;
			}
			m_mem->setBitWatcher(ccr.tacctl, 4, !(tacctl & 4));
			break;
		case 5:
			// Mode = Reset
			if (ccr0_interrupt) {
				break;
			}
			m_mem->setBitWatcher(ccr.tacctl, 4, false);
			break;
		case 6:
			// Mode = Toggle/Set
			if (ccr0_interrupt) {
				m_mem->setBitWatcher(ccr.tacctl, 4, true);
			}
			else {
				m_mem->setBitWatcher(ccr.tacctl, 4, !(tacctl & 4));
			}
			break;
		case 7:
			// Mode = Reset/Set
			if (ccr0_interrupt) {
				m_mem->setBitWatcher(ccr.tacctl, 4, true);
			}
			else {
				m_mem->setBitWatcher(ccr.tacctl, 4, false);
			}
			break;
	}
}

void Timer::checkCCRInterrupts(uint16_t tar) {
	// If timer changes its value to CCR0, fire CCR0 interrupt if
	// enabled.
	uint16_t tacctl = m_mem->getBigEndian(m_ccr[0].tacctl);
	bool ccr0_interrupt_enabled = tacctl & 16;
	bool ccr0_interrupt = false;
	uint16_t ccr0 = m_ccr[0].tbcl;
	if (tar == ccr0) {
		if (ccr0_interrupt_enabled) {
			// "Interrupt flag CCIFG is set"
			m_mem->setBit(m_ccr[0].tacctl, 1, true);
			m_intManager->queueInterrupt(m_intvect0);
		}
		// "Internal signal EQUx=1 and EQUx affects current output"
		doOutput(m_ccr[0], tacctl, false);
		if (m_type == TimerA) {
			// CCI is latched to SCCI
			m_mem->setBit(m_ccr[0].tacctl, 1 << 10, tacctl & 8);
		}
		ccr0_interrupt = true;
	}

	// Set CCIFG if TAR == CCR and CCIE is enabled
	for (int i = 1; i < m_ccr.size(); ++i) {
		tacctl = m_mem->getBigEndian(m_ccr[i].tacctl);
		uint16_t ccr = m_ccr[i].tbcl;
		bool interrupt_enabled = tacctl & 16;
		if (ccr == tar) {
			if (interrupt_enabled) {
				// "Interrupt flag CCIFG is set"
				m_mem->setBit(m_ccr[i].tacctl, 1, true);
				m_intManager->queueInterrupt(m_intvect1);
			}
			// "Internal signal EQUx=1 and EQUx affects current output"
			doOutput(m_ccr[i], tacctl, false);
			if (m_type == TimerA) {
				// CCI is latched to SCCI
				m_mem->setBit(m_ccr[i].tacctl, 1 << 10, tacctl & 8);
			}
		}

		if (ccr0_interrupt) {
			doOutput(m_ccr[i], tacctl, true);
		}
	}
}

void Timer::finishPendingCaptures(uint16_t tar) {
	for (int i = 0; i < m_ccr.size(); ++i) {
		CCR &ccr = m_ccr[i];
		if (ccr.capturePending) {
			if (m_mem->isBitSet(ccr.tacctl, 16)) {
				// interrupts enabled
				if (ccr.ccrRead) {
					m_mem->setBigEndian(ccr.taccr, tar, false);
					m_mem->setBit(m_ccr[i].tacctl, 1, true);
					if (i == 0) {
						m_intManager->queueInterrupt(m_intvect0);
					}
					else {
						m_intManager->queueInterrupt(m_intvect1);
					}
					ccr.ccrRead = false;
				}
				else {
					// Set COV, because CCR has not been read yet
					m_mem->setBit(ccr.tacctl, 2, true);	
				}
			}
			ccr.capturePending = false;
		}
	}
}

void Timer::latchTBCL(uint16_t tar, bool direction_changed) {
	for (int i = 0; i < m_ccr.size(); ++i) {
		CCR &ccr = m_ccr[i];
		uint16_t tacctl = m_mem->getBigEndian(ccr.tacctl, false);
		switch((tacctl >> 9) & 3) {
			case 0:
				break;
			case 1:
				if (tar == 0) {
					ccr.tbcl = m_mem->getBigEndian(ccr.taccr, false);
				}
				break;
			case 2:
				if (tar == 0 || direction_changed) {
					ccr.tbcl = m_mem->getBigEndian(ccr.taccr, false);
				}
				break;
			case 3:
				if (tar == ccr.tbcl) {
					ccr.tbcl = m_mem->getBigEndian(ccr.taccr, false);
				}
				break;
		}
	}
}

void Timer::changeTAR(uint8_t mode) {
	uint16_t ccr0;
	uint16_t tar = m_mem->getBigEndian(m_tar, false);
	bool taifg_interrupt_enabled = m_mem->isBitSet(m_tactl, 2);
	bool direction_changed = false;

	switch (mode) {
		case TIMER_STOPPED:
			break;
		case TIMER_UP:
			ccr0 = m_ccr[0].tbcl;

			// CCR0 is 0, so timer is stopped
			if (ccr0 == 0) {
				break;
			}

			if (tar == ccr0) {
				// Timer overflows, fire TAIFG interrupt if it's enabled
				m_mem->setBigEndian(m_tar, 0);
				if (taifg_interrupt_enabled) {
					m_mem->setBit(m_tactl, 1, true);
					m_intManager->queueInterrupt(m_intvect1);
				}
				tar = 0;
			}
			else {
				tar += 1;
				m_mem->setBigEndian(m_tar, tar);
			}

			finishPendingCaptures(tar);

			// Generate CCRx interrupts
			checkCCRInterrupts(tar);

			if (m_type == TimerB) {
				latchTBCL(tar, false);
			}
			break;
		case TIMER_CONTINUOUS:
			if (tar == m_counterMax) {
				// Timer overflows, fire TAIFG interrupt if it's enabled
				m_mem->setBigEndian(m_tar, 0);
				if (taifg_interrupt_enabled) {
					m_mem->setBit(m_tactl, 1, true);
					m_intManager->queueInterrupt(m_intvect1);
				}
				tar = 0;
			}
			else {
				tar += 1;
				m_mem->setBigEndian(m_tar, tar);
			}

			finishPendingCaptures(tar);

			// Generate CCRx interrupts
			checkCCRInterrupts(tar);

			if (m_type == TimerB) {
				latchTBCL(tar, false);
			}
			break;
		case TIMER_UPDOWN:
			ccr0 = m_ccr[0].tbcl;

			// CCR0 is 0, so timer is stopped
			if (ccr0 == 0) {
				break;
			}

			if (tar == 1 && !m_up) {
				// we are counting from 1 -> 0, so fire TAIFG interrupt
				m_mem->setBigEndian(m_tar, 0);
				if (taifg_interrupt_enabled) {
					m_mem->setBit(m_tactl, 1, true);
					m_intManager->queueInterrupt(m_intvect1);
				}
				tar = 0;
				m_up = true;
				direction_changed = true;
			}
			else {
				if (m_up) {
					tar += 1;
					// Generate CCRx interrupts only when changing from
					// CCR - 1 to CCR (it means in UP mode)
					checkCCRInterrupts(tar);
				}
				else {
					tar -= 1;
				}

				if (tar == ccr0) {
					m_up = false;
					direction_changed = true;
				}
				m_mem->setBigEndian(m_tar, tar);
			}

			finishPendingCaptures(tar);

			if (m_type == TimerB) {
				latchTBCL(tar, direction_changed);
			}
			break;
		default:
			break;
	}
}

void Timer::tickRising() {
	if (++m_counter >= m_divider) {
		m_counter = 0;
		uint8_t mode = (m_mem->getByte(m_tactl) >> 4) & 3;
		changeTAR(mode);
	}
}

void Timer::reset() {
	if (m_source) {
		m_source->removeHandler(this);
	}
	m_source = m_aclk;
	m_source->addHandler(this, Clock::Rising);
}

void Timer::generateOutput(CCR &ccr, bool value) {
	for (std::vector<PinMultiplexer *>::iterator it = ccr.outputMpxs.begin(); it != ccr.outputMpxs.end(); ++it) {
		(*it)->generateOutput(this, value ? 3.0 : 0.0);
	}

	m_pinManager->generateSignal(ccr.taName, value ? 3.0 : 0.0);
}

void Timer::handleMemoryChanged(::Memory *memory, uint16_t address) {
	uint16_t val = memory->getBigEndian(address, false);

	if (address == m_tactl) {
		// TACLR
		if (val & 4) {
			memory->setBit(m_tactl, 4, false);
			memory->setBigEndian(m_tar, 0);
			m_up = true;
			m_divider = 1;
		}

		// Choose divider
		switch((val >> 6) & 3) {
			case 0: m_divider = 1; break;
			case 1: m_divider = 2; break;
			case 2: m_divider = 4; break;
			case 3: m_divider = 8; break;
			default: break;
		}

		m_counter = m_divider;

		// Choose source
		switch((val >> 8) & 3) {
			case 0: break;
			case 1:
				if (m_source) {
					m_source->removeHandler(this);
				}
				m_source = m_aclk;
				m_source->addHandler(this, Clock::Rising);
				break;
			case 2:
				if (m_source) {
					m_source->removeHandler(this);
				}
				m_source = m_smclk;
				m_source->addHandler(this, Clock::Rising);
				break;
			case 3: break;
			default: break;
		}

		if (m_type == TimerB) {
			// Choose source
			switch((val >> 11) & 3) {
				case 0: m_counterMax = 0xffff; break;
				case 1: m_counterMax = 0x0fff; break;
				case 2: m_counterMax = 0x03ff; break;
				case 3: m_counterMax = 0x00ff; break;
				default: break;
			}
		}
	}
	else {
		for (int i = 0; i < m_ccr.size(); ++i) {
			CCR &ccr = m_ccr[i];
			if (ccr.tacctl == address) {
				generateOutput(ccr, (val & 4) == 4);

				// Check if CCIS really changed
				uint8_t ccis = (val >> 12) & 3;
				if (ccr.ccis == ccis) {
					break;
				}
				ccr.ccis = ccis;

				bool isInput = false;
				double value;
				// Check input select
				switch(ccis) {
					case 0:
						// CCIA
						if (!ccr.cciaMpx) {
							break;
						}
						value = ccr.cciaMpx->getValue(isInput) ? 3.0 : 0.0;
						if (isInput) {
							handlePinInput(ccr, i, ccr.ccia, value);
						}
						break;
					case 1:
						// CCIB
						if (!ccr.ccibMpx) {
							break;
						}
						value = ccr.ccibMpx->getValue(isInput) ? 3.0 : 0.0;;
						if (isInput) {
							handlePinInput(ccr, i, ccr.ccib, value);
						}
						break;
					case 2:
						// GND
						handlePinInput(ccr, i, "GND", 0.0);
						break;
					case 3:
						// VCC
						handlePinInput(ccr, i, "VCC", 3.0);
						break;
				}

				break;
			}
			else if (ccr.taccr == address) {
				if (m_type == TimerA) {
					ccr.tbcl = val;
				}
				else {
					uint16_t tacctl = memory->getBigEndian(ccr.tacctl, false);
					switch((tacctl >> 9) & 3) {
						case 0:
							// TBCTL loads on write
							ccr.tbcl = val;
							break;
						default:
							// Otherwise it gets refreshed in changeTar()
							break;
					}
				}
				break;
			}
		}
	}
}

void Timer::handleInterruptFinished(InterruptManager *intManager, int vector) {
	if (vector == m_intvect0) {
		// We have to reset CCR0 CCIFG after interrupt routine
		m_mem->setBit(m_ccr[0].tacctl, 1, false);
	}
	else {
		// Check if we have more interrupts queued and if any, generate the
		// interrupt.
		for (int i = 1; i < m_ccr.size(); ++i) {
			bool interrupt_enabled = m_mem->isBitSet(m_ccr[i].tacctl, 16);
			if (m_mem->isBitSet(m_ccr[i].tacctl, 1)) {
				m_intManager->queueInterrupt(m_intvect1);
				return;
			}
		}

		bool taifg_interrupt_enabled = m_mem->isBitSet(m_tactl, 2);
		if (taifg_interrupt_enabled && m_mem->isBitSet(m_tactl, 1)) {
			m_intManager->queueInterrupt(m_intvect1);
		}
	}
}

void Timer::handleMemoryRead(::Memory *memory, uint16_t address, uint16_t &value) {
	if (address == m_taiv) {
		// Check what interrupts we have queued and set 'value' to the one
		// with highest priority. TAIV will remain 0.
		for (int i = 1; i < m_ccr.size(); ++i) {
			bool interrupt_enabled = m_mem->isBitSet(m_ccr[i].tacctl, 16);
			if (m_mem->isBitSet(m_ccr[i].tacctl, 1)) {
				value = 2 * i;
				m_mem->setBit(m_ccr[i].tacctl, 1, false);
				return;
			}
		}

		bool taifg_interrupt_enabled = m_mem->isBitSet(m_tactl, 2);
		if (taifg_interrupt_enabled && m_mem->isBitSet(m_tactl, 1)) {
			m_mem->setBit(m_tactl, 1, false);
			value = 10;
		}
	}
	else {
		// CCR is read, so we have to set ccr.ccrRead flag to not generate
		// COV later.
		for (int i = 0; i < m_ccr.size(); ++i) {
			CCR &ccr = m_ccr[i];
			if (ccr.taccr == address) {
				ccr.ccrRead = true;
				break;
			}
		}
	}
}

void Timer::doCapture(CCR &ccr, int ccrIndex, uint16_t tacctl) {
	if (tacctl & (1 << 11)) {
		// SCS is 1, so we are in sync mode, therefore just set
		// the capturePending flag.
		ccr.capturePending = true;
	}
	else if (tacctl & 16) {
		// Interrupts enabled and we are in async mode, so fire
		// the interrupt.
		if (ccr.ccrRead) {
			m_mem->setBit(ccr.tacctl, 1, true);
			m_mem->setBigEndian(ccr.taccr, m_mem->getBigEndian(m_tar, false), false);
			if (ccrIndex == 0) {
				m_intManager->queueInterrupt(m_intvect0);
			}
			else {
				m_intManager->queueInterrupt(m_intvect1);
			}
			ccr.ccrRead = false;
		}
		else {
			// Set COV, because previous capture has not been
			// read yet.
			m_mem->setBit(ccr.tacctl, 2, true);
		}
	}
}

void Timer::handlePinInput(CCR &ccr, int ccrIndex, const std::string &name, double value) {
	if (value == HIGH_IMPEDANCE) {
		return;
	}

	uint16_t tacctl = m_mem->getBigEndian(ccr.tacctl, false);
	bool old_value = tacctl & 8;

	// Set CCI bit
	m_mem->setBit(ccr.tacctl, 8, value >= 1.5);

	// Running in compare mode, so return
	if ((tacctl & (1 << 8)) == 0) {
		return;
	}

	// Check input select
	switch((tacctl >> 12) & 3) {
		case 0:
			// Input is set to ccia, but we have different input
			if (ccr.ccia != name) {
				return;
			}
			break;
		case 1:
			// Input is set to ccib, but we have different input
			if (ccr.ccib != name) {
				return;
			}
			break;
		case 2:
			if (name != "GND") {
				return;
			}
			break;
		case 3:
			if (name != "VCC") {
				return;
			}
			break;
	}

	// Check capture mode
	switch((tacctl >> 14) & 3) {
		case 0:
			// No capture
			return;
		case 1:
			// Capture on rising edge
			if (old_value == 0 && value >= 1.5) {
				doCapture(ccr, ccrIndex, tacctl);
			}
			break;
		case 2:
			// Capture on failing edge
			if (old_value == 1 && value <= 1.3) {
				doCapture(ccr, ccrIndex, tacctl);
			}
			break;
		case 3:
			// Capture on failing and rising edge
			if (old_value != (value <= 1.3)) {
				doCapture(ccr, ccrIndex, tacctl);
			}
			break;
		default:
			break;
	}
}

void Timer::handlePinInput(const std::string &name, double value) {
	std::map<std::string, int>::iterator it = m_cciNames.find(name);
	if (it == m_cciNames.end()) {
		return;
	}

	CCR &ccr = m_ccr[it->second];
	handlePinInput(ccr, it->second, name, value);
}

void Timer::handlePinActivated(const std::string &name) {
	
}

void Timer::handlePinDeactivated(const std::string &name) {
	
}

}
