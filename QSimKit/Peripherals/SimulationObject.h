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
#include <QHash>
#include <map>

#include "adevs.h"
#include <stdint.h>

class PinHistory;

typedef adevs::PortValue<double> SimulationEvent;

typedef adevs::Bag<SimulationEvent> SimulationEventList;

class SimulationObjectWrapper;

class SimulationObject {
	public:
		SimulationObject() : m_wrapper(0) {}
		virtual ~SimulationObject() {}

		virtual void internalTransition() = 0;

		virtual void externalEvent(double t, const SimulationEventList &) = 0;

		virtual void output(SimulationEventList &output) = 0;

		virtual double timeAdvance() = 0;

		virtual void getInternalSimulationObjects(std::vector<SimulationObject *> &) {}

		void setWrapper(SimulationObjectWrapper *wrapper) {
			m_wrapper = wrapper;
		}

	protected:
		SimulationObjectWrapper *m_wrapper;
};

class SimulationObjectWrapper : public adevs::Atomic<SimulationEvent> {
	public:
		SimulationObjectWrapper(SimulationObject *obj, const QList<int> &monitoredPins = QList<int>());
		~SimulationObjectWrapper();

		/// Internal transition function.
		void delta_int();

		/// Handles external changes (change on PINs or interrupts)
		void delta_ext(double e, const SimulationEventList &xb);

		/// Confluent transition function.
		void delta_conf(const SimulationEventList &xb);

		/// Output function.
		void output_func(SimulationEventList &yb);

		/// Time advance function.
		double ta();

		/// Output value garbage collection.
		void gc_output(SimulationEventList& g);

		void setSimulator(adevs::Simulator<SimulationEvent> *sim) {
			m_sim = sim;
		}

		void reschedule() {
			m_sim->addModel(this);
		}

		double getTime() {
			return m_sim->nextEventTime();
		}

		QVector<PinHistory *> &getPinHistory() {
			return m_history;
		}

		void setContext(uint16_t context) {
			m_context = context;
		}

		adevs::Devs<SimulationEvent, double> *getTarget(int in, int &out);

		void couple(int out, adevs::Devs<SimulationEvent, double> *c, int in);

	private:
		void addChangeToHistory(int pin, double value);

	private:
		adevs::Simulator<SimulationEvent> *m_sim;
		SimulationObject *m_obj;
		QVector<int> m_monitoredPins;
		QVector<PinHistory *> m_history;
		uint16_t m_context;

		class node {
			public:
			node() : c(0), port(0) {}
			adevs::Devs<SimulationEvent, double> *c;
			int port;
		};
		std::vector<node> m_conns;
};

