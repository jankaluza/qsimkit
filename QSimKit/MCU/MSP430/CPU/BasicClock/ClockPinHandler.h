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

#include <stdint.h>
#include <string>
#include <vector>
#include "Clock.h"
#include "CPU/Pins/PinHandler.h"

namespace MSP430 {

class PinManager;
class PinMultiplexer;
class Clock;

/// Outputs Clock's signal to output pin.
class ClockPinHandler : public PinHandler, public ClockHandler {
	public:
		/// Creates new ClockPinHandler.
		/// \param pinManager PinManager to which output pin of this handler is
		/// connected to.
		/// \param clock Clock connected to output pin.
		/// \param name Name of the output to be used to register PinMultiplexer.
		ClockPinHandler(PinManager *pinManager, Clock *clock, const std::string &name);
		virtual ~ClockPinHandler();

		/// Called by Clock on every tick. Generates output signal on pin.
		void tickRising();
		void tickFalling();

		/// Called by PinMultiplexer in case of input signal. Any input is
		/// ignored.
		void handlePinInput(const std::string &name, double value);

		/// Called by PinMultiplexer when this PinHandler becomes active.
		void handlePinActivated(const std::string &name);

		/// Called by PinMultiplexer when this PinHandler becomes inactive.
		void handlePinDeactivated(const std::string &name);

	private:
		PinMultiplexer *m_mpx;
		Clock *m_clock;
};

}
