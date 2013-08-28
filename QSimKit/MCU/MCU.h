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
		typedef enum {
			Address       = 0x1,
			Boolean       = 0x2,
			ComplexFloat  = 0x3,
			Float         = 0x4,
			Signed        = 0x5,
			SignedChar    = 0x6,
			Unsigned      = 0x7,
			UnsignedChar    = 0x8,
			ImaginaryFloat  = 0x9,
			PackedDecimal   = 0xa,
			NumericString   = 0xb,
			Edited          = 0xc,
			SignedFixed     = 0xd,
			UnsignedFixed   = 0xe,
			DecimalFloat    = 0xf,
		} Encoding;

		typedef enum {
			Base,
			Volatile,
			Const,
			Array,
			Pointer,
		} Type;

		VariableType(const QString &name, uint8_t byteSize, Encoding encoding, Type type, uint16_t upperBound = 0) :
		m_name(name), m_byteSize(byteSize), m_encoding(encoding), m_type(type), m_upperBound(upperBound) {
		}

		const QString &getName() { return m_name; }

		uint8_t getByteSize() { return m_byteSize; }

		Encoding getEncoding() { return m_encoding; }

		Type getType() { return m_type; }

		uint16_t getUpperBound() { return m_upperBound; }

	private:
		QString m_name;
		uint8_t m_byteSize;
		Encoding m_encoding;
		Type m_type;
		uint16_t m_upperBound;
};

class Variable {
	public:

		Variable(const QString &name, VariableType *type) :
			m_name(name), m_type(type) { }

		VariableType *getType() { return m_type; }

		const QString &getName() {
			return m_name;
		}

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

		bool contains(uint16_t pc) {
			return pc >= m_pcLow && pc <= m_pcHigh;
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

		virtual Subprogram *getSubprogram(const QString &file, uint16_t pc) = 0;
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

