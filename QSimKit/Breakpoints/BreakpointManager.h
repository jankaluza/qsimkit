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

#include <stdint.h>
#include <QList>
#include <QHash>
#include <QTextStream>
#include <QDomDocument>
#include "MCU/Register.h"
#include "MCU/Memory.h"

class MCU;

class BreakpointManager : public QObject, public RegisterWatcher, public MemoryWatcher {
	Q_OBJECT

	public:
		typedef struct {
			uint16_t val;
			bool any;
		} MemoryBreak;

		BreakpointManager();
		~BreakpointManager();

		void setMCU(MCU *cpu);

		void addRegisterBreak(int reg, uint16_t value);
		void removeRegisterBreak(int reg, uint16_t value);
		const QList<uint16_t> &getRegisterBreaks(int reg) {
			return m_breaks[reg];
		}

		void addMemoryBreak(uint16_t addr, uint16_t value);
		void addMemoryBreak(uint16_t addr);
		void removeMemoryBreak(uint16_t addr);
		const QHash<uint16_t, MemoryBreak> &getMemoryBreaks() {
			return m_membreaks;
		}

		bool shouldBreak();

		bool handleRegisterChanged(Register *reg, int id, uint16_t value);
		void handleMemoryChanged(Memory *memory, uint16_t address);

		void save(QTextStream &stream);
		bool load(QDomDocument &doc);

	signals:
		void onRegisterBreakAdded(int reg, uint16_t value);
		void onRegisterBreakRemoved(int reg, uint16_t value);
		void onMemoryBreakAdded(uint16_t addr);
		void onMemoryBreakRemoved(uint16_t addr);

	private:
		MCU *m_mcu;
		QList<QList<uint16_t> > m_breaks;
		QHash<uint16_t, MemoryBreak> m_membreaks;
		bool m_break;

};

