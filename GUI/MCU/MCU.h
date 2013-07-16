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
#include <QHash>
#include <QString>
#include <QStringList>
#include <QChar>
#include <QRect>
#include <QList>
#include <stdint.h>

#include "ui/ScreenObject.h"
#include "Peripherals/Peripheral.h"
#include "Peripherals/SimulationObject.h"

class Memory;
class RegisterSet;
class Subprogram;

class DisassembledLine {
	public:
		typedef enum {Instruction, Code, Section} Type;

		DisassembledLine(uint16_t addr = 0, int lineNumber = 0, Type type = Instruction, const QString &data = "") :
			m_addr(addr), m_type(type), m_data(data), m_line(lineNumber) {}

		uint16_t getAddr() const {
			return m_addr;
		}

		const Type &getType() const {
			return m_type;
		}

		const QString &getData() const {
			return m_data;
		}

		int getLineNumber() const {
			return m_line;
		}

	private:
		uint16_t m_addr;
		Type m_type;
		QString m_data;
		int m_line;
};

typedef QList<DisassembledLine> DisassembledCode;

typedef QHash<QString, DisassembledCode> DisassembledFiles;

class VariableType {
	public:
		VariableType(const QString &name) : m_name(name) {
		}

		const QString &getName() { return m_name; }

	private:
		QString m_name;
};

class Variable {
	public:

		Variable(const QString &name, VariableType *type) :
			m_name(name), m_type(type) { }

		VariableType *getType() { return m_type; }

		virtual QString getValue(RegisterSet *r, Memory *m, Subprogram *p, uint16_t pc) = 0;

	private:
		QString m_name;
		VariableType *m_type;
};

typedef QList<Variable *> Variables;

class Subprogram {
	public:
		Subprogram(const QString &name, uint16_t pcLow, uint16_t pcHigh) :
			m_name(name), m_pcLow(pcLow), m_pcHigh(pcHigh) {}

		uint16_t getPCLow() const {
			return m_pcLow;
		}

		uint16_t getPCHigh() const {
			return m_pcHigh;
		}

		const QString &getName() const {
			return m_name;
		}

		virtual Variables &getVariables() = 0;

		virtual Variables &getArgs() = 0;

	private:
		QString m_name;
		uint16_t m_pcLow;
		uint16_t m_pcHigh;
};

typedef QList<Subprogram *> Subprograms;

class DebugData {
	public:
		DebugData() {}
		virtual ~DebugData() {}

		virtual const Subprograms &getSubprograms(const QString &file) = 0;
};

class MCU : public Peripheral {
	Q_OBJECT

	public:
		MCU() {}

		virtual QString getVariant() = 0;

		virtual QStringList getVariants() = 0;

		virtual RegisterSet *getRegisterSet() = 0;

		virtual Memory *getMemory() = 0;

		virtual DisassembledFiles getDisassembledCode() = 0;

		virtual DebugData *getDebugData() = 0;


	signals:
		void onCodeLoaded();

};

