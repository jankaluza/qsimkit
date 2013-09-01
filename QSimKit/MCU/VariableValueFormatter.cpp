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

#include "VariableValueFormatter.h"
#include <QDebug>

#include "Memory.h"

namespace VariableValueFormatter {

static int64_t getValue(Memory *mem, VariableValue &values, VariableType *type) {
	int64_t ret = 0;
	foreach(const VariableValuePiece &value, values) {
		if (value.isAddress()) {
			// If piece is 0, then this is the only piece in the value and
			// value is then type->getByteSize() long
			int bytes = value.getPieceSize();
			if (!bytes) {
				bytes = type->getByteSize();
			}

			while (bytes > 0) {
				if (bytes >= 2) {
					ret = ret << 2;
					ret |= mem->getBigEndian(value.getData(), false);
					bytes -= 2;
				}
				else {
					ret = ret << 1;
					ret |= mem->getByte(value.getData(), false);
					bytes -= 1;
				}
			}
		}
		else {
			// If piece is 0, then this is the only piece in the value and
			// value is then type->getByteSize() long
			int bytes = value.getPieceSize();
			if (!bytes) {
				bytes = type->getByteSize();
			}

			ret = ret << bytes;
			ret |= value.getData();
		}
	}

	return ret;
}

static void formatBase(Memory *mem, VariableValue &values, VariableType *type, QString &out, QString &tooltip) {
	QString dec;
	QString hex;
	QString bin;
	int16_t int16;

#define SET_STRINGS(N) 	dec = QString::number(N); \
	hex = QString("0x%1").arg(N, 0, 16); \
	bin = QString("%1").arg(N, 0, 2); \

	int64_t ret = getValue(mem, values, type);

	switch(type->getEncoding()) {
		default:
		case VariableType::Unsigned:
		case VariableType::UnsignedChar:
			switch(type->getByteSize()) {
				case 1:
					SET_STRINGS((ret & 0xff));
					break;
				default:
				case 2:
					SET_STRINGS((ret & 0xffff));
					break;
			};
			out = hex;
			break;
		case VariableType::Signed:
		case VariableType::SignedChar:
			switch(type->getByteSize()) {
				case 1:
					SET_STRINGS((ret & 0xff));
					break;
				default:
				case 2:
					SET_STRINGS((ret & 0xffff));
					break;
			};
			out = hex;
			break;
	};

	tooltip = "Dec: " + dec + "<br/>";
	tooltip += "Hex: " + hex + "<br/>";
	tooltip += "Bin: " + bin;
}

static void formatArray(Memory *mem, VariableValue &value, VariableType *type, QString &out, QString &tooltip) {
	
}

void format(Memory *mem, VariableValue &value, VariableType *type, QString &out, QString &tooltip) {
	switch(type->getType()) {
		case VariableType::Array:
			formatArray(mem, value, type, out, tooltip);
			break;
		default:
			formatBase(mem, value, type, out, tooltip);
			break;
	};
}

}
