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

#include <QByteArray>
#include <QString>
#include <QList>
#include <stdint.h>
#include "MCU/MCU.h"

class DwarfDebugData;
class DebugData;
class RegisterSet;
class Memory;
class DwarfSubprogram;

class DwarfExpression {
	public:
		typedef enum {
			Error,
			Absolute,
			Register,
			RegisterRelative,
		} LocationType;

		typedef struct {
			LocationType type;
			uint16_t addr;
			uint16_t offset;
		} Location;

		typedef struct {
			uint32_t data;
			bool isAddress;
			uint8_t piece;
		} Value;

		DwarfExpression(const QString &expression);
		virtual ~DwarfExpression();

		VariableValue getValue(RegisterSet *r, Memory *m, DwarfSubprogram *s, uint16_t pc, bool &isAddress);

		bool parse(const QString &expression);

	private:
		typedef struct {
			unsigned char op;
			uint16_t arg;
			uint16_t arg2;
		} Instruction;
		
		Instruction getInstruction(const QString &expr);

	private:
		QList<Instruction> m_instructions;
		
};

