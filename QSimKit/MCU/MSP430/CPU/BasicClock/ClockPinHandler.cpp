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

#include "ClockPinHandler.h"
#include "Clock.h"
#include "CPU/Pins/PinManager.h"
#include "CPU/Pins/PinMultiplexer.h"
#include <iostream>

namespace MSP430 {
	
ClockPinHandler::ClockPinHandler(PinManager *pinManager, Clock *clock,
								 const std::string &name) : m_mpx(0),
								 m_clock(clock) {

	// Register PinMultiplexer
	std::vector<PinMultiplexer *> mpxs;
	mpxs = pinManager->addPinHandler(name, this);
	m_mpx = mpxs.empty() ? 0 : mpxs[0];
}

ClockPinHandler::~ClockPinHandler() {
	
}

void ClockPinHandler::tickRising() {
	m_mpx->generateOutput(this, 3.0);
}

void ClockPinHandler::tickFalling() {
	m_mpx->generateOutput(this, 0.0);
}

void ClockPinHandler::handlePinInput(const std::string &name, double value) {

}

void ClockPinHandler::handlePinActivated(const std::string &name) {
	m_clock->addHandler(this);
}

void ClockPinHandler::handlePinDeactivated(const std::string &name) {
	m_clock->removeHandler(this);
}

}
