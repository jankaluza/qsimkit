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


void SimulationObjectWrapper::delta_int() {
	m_obj->internalTransition();
}

void SimulationObjectWrapper::delta_ext(double e, const adevs::Bag<adevs::PortValue<SimulationEvent *> >& xb) {
	std::vector<SimulationEvent *> events;
	for (adevs::Bag<adevs::PortValue<SimulationEvent *> >::iterator iter = xb.begin(); iter != xb.end(); iter++) {
		(*iter).value->port = (*iter).port;
		events.push_back((*iter).value);
	}
	m_obj->externalEvent(events);
}

void SimulationObjectWrapper::delta_conf(const adevs::Bag<adevs::PortValue<SimulationEvent *> >& xb) {
	delta_int();
	delta_ext(0.0, xb);
}

void SimulationObjectWrapper::output_func(adevs::Bag<adevs::PortValue<SimulationEvent *> >& yb) {
	std::vector<SimulationEvent *> events;
	m_obj->output(events);

	for (std::vector<SimulationEvent *>::iterator it = events.begin(); it != events.end(); ++it) {
		adevs::PortValue<SimulationEvent *> p((*it)->port, *it);
		yb.insert(p);
	}
}

double SimulationObjectWrapper::ta() {
	return m_obj->timeAdvance();
}

void SimulationObjectWrapper::gc_output(adevs::Bag<adevs::PortValue<SimulationEvent *> >& g) {

}
