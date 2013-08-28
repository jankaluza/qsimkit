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

#include "DwarfVariable.h"
#include "DwarfLocationList.h"
#include "DwarfExpression.h"
#include "DwarfSubprogram.h"

#include "QSimKit/MCU/RegisterSet.h"
#include "QSimKit/MCU/Register.h"
#include "QSimKit/MCU/Memory.h"
#include "QSimKit/DockWidgets/Peripherals/MemoryItem.h"

#include <QDebug>

DwarfVariable::DwarfVariable(VariableType *type, const QString &name, DwarfLocationList *ll, DwarfExpression *expr) :
	Variable(name, type), m_ll(ll), m_expr(expr) {
	
}

DwarfVariable::~DwarfVariable() {
	delete m_ll;
	delete m_expr;
}

QString DwarfVariable::getValue(RegisterSet *r, Memory *m, Subprogram *p, uint16_t pc) {
	QString v = "??";
	QList<DwarfExpression::Value> data;
	bool isAddress;
	if (m_ll) {
		data = m_ll->getValue(r, m, static_cast<DwarfSubprogram *>(p), pc, isAddress);
	}
	else {
		data = m_expr->getValue(r, m, static_cast<DwarfSubprogram *>(p), pc, isAddress);
	}

	uint64_t ret = 0;
	uint8_t lastPiece = 0;
	// TODO: We only support DW_OP_piece split between more registers.
	// It would be great to support combination of register and memory
	foreach(const DwarfExpression::Value &value, data) {
		if (value.isAddress) {
			QString tooltip;
			MemoryItem::format(m, value.data, getType(), v, tooltip);
			return v;
		}
		else {
			ret |= value.data;
			ret = ret << value.piece;
			lastPiece = value.piece;
		}
	}
	v = QString("0x%1").arg(ret >> lastPiece, 0, 16);
	return v;
}
