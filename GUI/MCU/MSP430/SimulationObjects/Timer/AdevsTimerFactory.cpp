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

#include "AdevsTimerFactory.h"
#include "CPU/Variants/Variant.h"
#include "CPU/Memory/Memory.h"
#include "CPU/Interrupts/InterruptManager.h"
#include "CPU/Pins/PinManager.h"
#include "Timer.h"
#include <iostream>

#include "CPU/BasicClock/ACLK.h"

AdevsTimerFactory::AdevsTimerFactory() {}

MSP430::Timer *AdevsTimerFactory::createTimer(MSP430::Timer::Type type, MSP430::PinManager *pinManager, MSP430::InterruptManager *intManager, MSP430::Memory *mem,
						Variant *variant, MSP430::ACLK *aclk,
						MSP430::SMCLK *smclk, uint16_t tactl, uint16_t tar,
						uint16_t taiv, uint16_t intvec0, uint16_t intvec1) {
	Timer *timer = new Timer(type, pinManager, intManager, mem, variant, aclk, smclk, tactl, tar, taiv, intvec0, intvec1);
	return timer;
}
