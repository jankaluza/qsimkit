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

#include "GUI/MCU/RegisterSet.h"
#include "GUI/MCU/Register.h"
#include "GUI/MCU/Memory.h"

#include <QDebug>

DwarfVariable::DwarfVariable(const QString &name, DwarfLocationList *ll, DwarfExpression *expr) :
	Variable(name, 0), m_ll(ll), m_expr(expr) {
	
}

DwarfVariable::~DwarfVariable() {
	
}

QString DwarfVariable::getValue(RegisterSet *r, Memory *m, Subprogram *p, uint16_t pc) {
	QString v = "??";
	uint16_t data;
	if (m_ll) {
		data = m_ll->getValue(r, m, static_cast<DwarfSubprogram *>(p), pc);
	}
	else {
		data = m_expr->getValue(r, m, static_cast<DwarfSubprogram *>(p), pc);
	}

	v = QString("0x%1").arg(m->getBigEndian(data, false), 0, 16);
	return v;
}
