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
#include <QDebug>

Timer::Timer(MCU::PinManager *pinManager, MCU::InterruptManager *intManager, MCU::Memory *mem,
			 Variant *variant, MCU::ACLK *aclk, MCU::SMCLK *smclk,
			 uint16_t tactl, uint16_t tar, uint16_t taiv, uint16_t intvec0, uint16_t intvec1) :
			 MCU::Timer(pinManager, intManager, mem, variant, aclk, smclk, tactl, tar, taiv, intvec0, intvec1) {
	
}

Timer::~Timer() {

}

void Timer::internalTransition() {
	tick();
}

void Timer::externalEvent(double t, const SimulationEventList &) {

}

void Timer::output(SimulationEventList &output) {

}

double Timer::timeAdvance() {
	return getStep();
}
