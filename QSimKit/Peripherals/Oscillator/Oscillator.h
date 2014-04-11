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
#include "Peripherals/Peripheral.h"
#include "Peripherals/PeripheralInterface.h"

class Oscillator : public Peripheral
{
	public:
		Oscillator();

		void internalTransition();

		void externalEvent(double e, const SimulationEventList &);

		void output(SimulationEventList &output);

		double timeAdvance();

		void reset();

		void paint(QWidget *screen);

		PinList &getPins() {
			return m_pins;
		}

		const QStringList &getOptions();

		void executeOption(int option);

		void save(QTextStream &stream);
		void load(QDomElement &object, QString &error);

	private:
		PinList m_pins;
		bool m_state;
		unsigned long m_freq;
		double m_step;
		SimulationEventList m_output;
		QStringList m_options;

};

class OscillatorInterface : public QObject, PeripheralInterface {
	Q_OBJECT
	Q_INTERFACES(PeripheralInterface)

	public:
		Peripheral *create();
};

