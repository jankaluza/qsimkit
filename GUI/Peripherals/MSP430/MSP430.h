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
#include "CPU/Memory/Memory.h"

class Memory;
class RegisterSet;
class InstructionDecoder;
class Instruction;
class Variant;

struct PinState {
	bool high;
	bool in;
};

class MSP430 : public Peripheral, public MemoryWatcher
{
	public:
		MSP430(Variant *variant, unsigned long freq = 1000000);

		bool loadXML(const QString &file);

		bool loadA43(const std::string &data);

		void internalTransition();

		void externalEvent(const std::vector<SimulationEvent *> &);

		void output(std::vector<SimulationEvent *> &output);

		double timeAdvance();

		void reset();

		void paint(QPainter &p);

		std::map<int, Pin> &getPins() {
			return m_pins;
		}

		void refreshPins();

		void refreshGP(const QString &prefix, uint16_t dir, uint16_t out);

		void handleMemoryChanged(Memory *memory, uint16_t address);

	private:
		void addMemoryWatchers();

	private:
		std::map<int, QChar> m_sides;
		std::map<int, Pin> m_pins;
		std::map<int, PinState> m_states;
		std::map<int, QString> m_names;
		std::map<QString, int> m_map;

		double m_time;
		double m_instructionCycles;

		Memory *m_mem;
		RegisterSet *m_reg;
		InstructionDecoder *m_decoder;
		Instruction *m_instruction;
		Variant *m_variant;
		double m_step;
		std::string m_code;
		std::vector<SimulationEvent *> m_output;
		
};

