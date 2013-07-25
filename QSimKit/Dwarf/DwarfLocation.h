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

class DwarfExpression;

class DwarfLocation {
	public:
		DwarfLocation(DwarfExpression *expr, uint16_t pcLow, uint16_t pcHigh);
		virtual ~DwarfLocation();

		DwarfExpression *getExpression() {
			return m_expr;
		}

		bool contains(uint16_t pc);

	private:
		DwarfExpression *m_expr;
		uint16_t m_pcLow;
		uint16_t m_pcHigh;
};

