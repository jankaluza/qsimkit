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
#include <QStringList>
#include <QChar>
#include <QRect>
#include <map>
#include "MCU/MCU.h"
#include "CPU/Pins/PinManager.h"

namespace MSP430 {

class Memory;
class RegisterSet;
class InstructionDecoder;
class Instruction;
class InterruptManager;
class BasicClock;
}

class Timer;
class AdevsTimerFactory;

class Variant;

class PinAddr {
	public:
		PinAddr() : addr(0), bit(0) {}
		PinAddr(uint16_t addr, uint8_t bit) : addr(addr), bit(bit) {}
		
		uint16_t addr;
		uint8_t bit;
};

class MCU_MSP430 : public MCU, public MSP430::PinWatcher
{
	public:
		MCU_MSP430(const QString &variant = "msp430x241x");

		QString getVariant();
		Variant *getVariantPtr() { return m_variant; }

		QStringList getVariants();

		bool loadA43(const std::string &data);

		void getInternalSimulationObjects(std::vector<SimulationObject *> &objects);

		void internalTransition();

		void externalEvent(double t, const SimulationEventList &);

		void output(SimulationEventList &output);

		double timeAdvance();

		void reset();

		PeripheralItem *getPeripheralItem() {
			return m_peripheralItem;
		}

		void paint(QWidget *screen);

		PinList &getPins() {
			return m_pins;
		}

		void refreshPins();

		void refreshGP(const QString &prefix, uint16_t dir, uint16_t out);

		void handlePinChanged(int id, double value);

		const QStringList &getOptions() {
			return m_options;
		}

		void executeOption(int option);

		virtual void save(QTextStream &stream);
		virtual void load(QDomElement &object);

		RegisterSet *getRegisterSet();

		Memory *getMemory() {
			return m_mem;
		}

		const std::string &getCode() {
			return m_code;
		}

		void setELF(const QByteArray &elf) {
			m_elf = elf;
		}

		const QByteArray &getELF() {
			return m_elf;
		}

	private:
		void setPinType(const QString &name, MSP430::PinType &type, int &subtype);

	private:
		std::map<int, QChar> m_sides;
		PinList m_pins;
		std::vector<PinAddr> m_pin2addr;

		double m_time;
		double m_instructionCycles;

		MSP430::Memory *m_mem;
		MSP430::RegisterSet *m_reg;
		MSP430::InstructionDecoder *m_decoder;
		MSP430::Instruction *m_instruction;
		Variant *m_variant;
		MSP430::PinManager *m_pinManager;
		MSP430::InterruptManager *m_intManager;
		MSP430::BasicClock *m_basicClock;
		AdevsTimerFactory *m_timerFactory;
		std::string m_code;
		SimulationEventList m_output;
		QStringList m_options;
		bool m_ignoreNextStep;
		QByteArray m_elf;
		PeripheralItem *m_peripheralItem;
		
};

