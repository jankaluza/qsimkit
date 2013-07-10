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

#include "VLO.h"
#include <QDebug>

VLO::VLO() : m_paused(false) {
	
}

VLO::~VLO() {

}

void VLO::internalTransition() {
	tick();
}

void VLO::externalEvent(double t, const SimulationEventList &) {

}

void VLO::output(SimulationEventList &output) {

}

double VLO::timeAdvance() {
	// Oscillator has to tick 2x faster, because it has to rise up and fall down.
	return getStep() / 2;
}

void VLO::start() {
	m_paused = false;
	if (m_wrapper) {
		m_wrapper->reschedule();
	}
}

void VLO::pause() {
	m_paused = true;
}
