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
#include "Timer.h"
#include <iostream>

#include "CPU/BasicClock/ACLK.h"

AdevsTimerFactory::AdevsTimerFactory() {}

MCU::Timer *AdevsTimerFactory::createTimer(MCU::InterruptManager *intManager, MCU::Memory *mem,
						Variant *variant, MCU::ACLK *aclk,
						MCU::SMCLK *smclk, uint16_t tactl, uint16_t tar,
						uint16_t taiv) {
	Timer *timer = new Timer(intManager, mem, variant, aclk, smclk, tactl, tar, taiv);
	return timer;
}
