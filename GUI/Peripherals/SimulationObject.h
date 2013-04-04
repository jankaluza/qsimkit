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
#include <map>

#include "adevs.h"

class SimulationEvent {
	public:
		SimulationEvent(int port, bool high) : port(port), high(high) {}
		virtual ~SimulationEvent() {}

		int port;
		bool high;
};

class SimulationObject {
	public:
		SimulationObject() {}
		virtual ~SimulationObject() {}

		virtual void internalTransition() = 0;

		virtual void externalEvent(double t, const std::vector<SimulationEvent *> &) = 0;

		virtual void output(std::vector<SimulationEvent *> &output) = 0;

		virtual double timeAdvance() = 0;

};

class SimulationObjectWrapper : public adevs::Atomic<adevs::PortValue<SimulationEvent *> > {
	public:
		SimulationObjectWrapper(SimulationObject *obj) : m_obj(obj) {}
		~SimulationObjectWrapper() {}

		/// Internal transition function.
		void delta_int();

		/// Handles external changes (change on PINs or interrupts)
		void delta_ext(double e, const adevs::Bag<adevs::PortValue<SimulationEvent *> >& xb);

		/// Confluent transition function.
		void delta_conf(const adevs::Bag<adevs::PortValue<SimulationEvent *> >& xb);

		/// Output function.
		void output_func(adevs::Bag<adevs::PortValue<SimulationEvent *> >& yb);

		/// Time advance function.
		double ta();

		/// Output value garbage collection.
		void gc_output(adevs::Bag<adevs::PortValue<SimulationEvent *> >& g);

	private:
		SimulationObject *m_obj;
};

