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

#include "SimulationObject.h"

#include <QDebug>


void SimulationObjectWrapper::delta_int() {
	m_obj->internalTransition();
}

void SimulationObjectWrapper::delta_ext(double e, const SimulationEventList& xb) {
	m_obj->externalEvent(e, xb);
}

void SimulationObjectWrapper::delta_conf(const SimulationEventList& xb) {
	delta_int();
	delta_ext(0.0, xb);
}

void SimulationObjectWrapper::output_func(SimulationEventList& yb) {
	m_obj->output(yb);
}

double SimulationObjectWrapper::ta() {
	return m_obj->timeAdvance();
}

void SimulationObjectWrapper::gc_output(SimulationEventList& g) {

}
