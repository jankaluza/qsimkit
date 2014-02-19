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

#include "DwarfSubprogram.h"
#include "DwarfLocationList.h"
#include "DwarfExpression.h"
#include "DwarfVariable.h"

#include <QDebug>

DwarfSubprogram::DwarfSubprogram(const QString &name, uint16_t pcLow, uint16_t pcHigh,
	DwarfLocationList *ll, DwarfExpression *expr) :
	Subprogram(name, pcLow, pcHigh), m_ll(ll), m_expr(expr) {
	
}

DwarfSubprogram::~DwarfSubprogram() {
	delete m_ll;
	delete m_expr;

	foreach(Variable *v, m_vars) {
		delete v;
	}

	foreach(Variable *v, m_args) {
		delete v;
	}
}

void DwarfSubprogram::addVariable(DwarfVariable *v) {
	m_vars.append(v);
}

void DwarfSubprogram::addArg(DwarfVariable *v) {
	m_args.append(v);
	m_vars.append(v);
}

uint16_t DwarfSubprogram::getFrameBase(RegisterSet *r, Memory *m, uint16_t pc) {
	uint16_t base;
	bool isAddress;
	if (m_ll) {
		VariableValue vv = m_ll->getValue(r, m, this, pc, isAddress);
		if (!vv.empty()) {
			base = vv[0].getData();
		}
		else {
			qDebug() << "DwarfSubprogram::getFrameBase(): no VariableValue!" << getName() << getPCLow() << getPCHigh() <<  pc;
		}
	}
	else {
		base = m_expr->getValue(r, m, this, pc, isAddress)[0].getData();
	}

// 	qDebug() << getName() << "base is" << base;
	return base;
}

Variables &DwarfSubprogram::getVariables() {
	return m_vars;
}

Variables &DwarfSubprogram::getArgs() {
	return m_args;
}
