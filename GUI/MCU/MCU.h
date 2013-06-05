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
#include <QList>
#include <stdint.h>

#include "ui/ScreenObject.h"
#include "Peripherals/Peripheral.h"
#include "Peripherals/SimulationObject.h"

class Memory;
class RegisterSet;

class DisassembledLine {
	typedef enum {Instruction, Code, Section} Type;

	DisassembledLine(uint16_t addr, Type type = Instruction, const QString &data = "") :
		m_addr(addr), m_type(type), m_data(data) {}

	const Type &getType() {
		return m_type;
	}

	const QString &getData() {
		return m_data;
	}

	private:
		uint16_t m_addr;
		Type m_type;
		QString m_data;
};

typedef QList<DisassembledLine> DisassembledCode;

class MCU : public Peripheral {
	Q_OBJECT

	public:
		MCU() {}

		virtual QString getVariant() = 0;

		virtual QStringList getVariants() = 0;

		virtual bool loadA43(const QString &data) = 0;

		virtual const QString &getA43() = 0;

		virtual RegisterSet *getRegisterSet() = 0;

		virtual Memory *getMemory() = 0;

		virtual void loadELF(const QByteArray &elf) = 0;

		virtual const QByteArray &getELF() = 0;

	signals:
		void onCodeLoaded();

};

