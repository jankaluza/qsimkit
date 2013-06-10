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

#include "LFXT1.h"
#include <QDebug>

LFXT1::LFXT1(MSP430::Memory *mem, Variant *variant) : MSP430::LFXT1(mem, variant) {
	
}

LFXT1::~LFXT1() {

}

void LFXT1::internalTransition() {
	tick();
}

void LFXT1::externalEvent(double t, const SimulationEventList &) {

}

void LFXT1::output(SimulationEventList &output) {

}

double LFXT1::timeAdvance() {
	return getStep();
}
