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

#pragma once

#include <QWidget>
#include <QString>
#include <QChar>
#include <QRect>
#include <QList>
#include <map>

#include "Peripherals/SimulationObject.h"
#include "CPU/BasicClock/Timer.h"

class Timer : public SimulationObject, public MSP430::Timer {
	public:
		Timer(Timer::Type &type, MSP430::PinManager *pinManager, MSP430::InterruptManager *intManager, MSP430::Memory *mem, Variant *variant,
			  MSP430::ACLK *aclk, MSP430::SMCLK *smclk, uint16_t tactl, uint16_t tar,
			  uint16_t taiv, uint16_t intvec0, uint16_t intvec1);
		~Timer();

		void internalTransition();

		void externalEvent(double t, const SimulationEventList &);

		void output(SimulationEventList &output);

		double timeAdvance();
};
