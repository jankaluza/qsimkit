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

namespace MSP430 {

class OscillatorHandler {
	public:
		virtual void tickRising() = 0;
		virtual void tickFalling() = 0;
};

class Oscillator {
	public:
		Oscillator(const std::string &name = "Unnamed oscillator");
		virtual ~Oscillator();

		void addHandler(OscillatorHandler *handler);
		void removeHandler(OscillatorHandler *handler);

		virtual unsigned long getFrequency() = 0;

		virtual double getStep() = 0;

		void tick();

		virtual void pause() {}
		virtual void start() {}

		const std::string &getName() {
			return m_name;
		}

	private:
		std::string m_name;
		std::vector<OscillatorHandler *> m_handlers;
		bool m_rising;
		bool m_inTick;
		bool m_willAddRemove;
		std::vector<OscillatorHandler *> m_toRemove;
		std::vector<OscillatorHandler *> m_toAdd;
};

}
