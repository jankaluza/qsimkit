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
#include "Peripherals/Peripheral.h"
#include "CPU/Pins/PinManager.h"

namespace MCU {

class Memory;
class RegisterSet;
class InstructionDecoder;
class Instruction;
class InterruptManager;
class DCO;
class MCLK;

}

class Variant;

class PinAddr {
	public:
		PinAddr() : addr(0), bit(0) {}
		PinAddr(uint16_t addr, uint8_t bit) : addr(addr), bit(bit) {}
		
		uint16_t addr;
		uint8_t bit;
};

class MSP430 : public Peripheral, public MCU::PinWatcher
{
	public:
		MSP430(Variant *variant);

		Variant *getVariant() { return m_variant; }

		bool loadA43(const std::string &data);

		void internalTransition();

		void externalEvent(double t, const SimulationEventList &);

		void output(SimulationEventList &output);

		double timeAdvance();

		void reset();

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

		MCU::RegisterSet *getRegisterSet() {
			return m_reg;
		}

		MCU::Memory *getMemory() {
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

		void loadPackage(const QString &file);

	private:
		void setPinType(const QString &name, MCU::PinType &type, int &subtype);

	private:
		std::map<int, QChar> m_sides;
		PinList m_pins;
		std::vector<PinAddr> m_pin2addr;

		double m_time;
		double m_instructionCycles;

		MCU::Memory *m_mem;
		MCU::RegisterSet *m_reg;
		MCU::InstructionDecoder *m_decoder;
		MCU::Instruction *m_instruction;
		Variant *m_variant;
		MCU::PinManager *m_pinManager;
		MCU::InterruptManager *m_intManager;
		MCU::DCO *m_dco;
		MCU::MCLK *m_mclk;
		std::string m_code;
		SimulationEventList m_output;
		QStringList m_options;
		bool m_ignoreNextStep;
		QByteArray m_elf;
		
};

