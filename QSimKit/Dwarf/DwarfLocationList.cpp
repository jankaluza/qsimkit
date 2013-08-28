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

#include "DwarfLocationList.h"
#include <QDebug>

#include "DwarfLocation.h"
#include "DwarfExpression.h"
#include "DwarfSubprogram.h"

DwarfLocationList::DwarfLocationList() {
	
}

DwarfLocationList::~DwarfLocationList() {
	foreach(DwarfLocation *loc, m_locations) {
		delete loc;
	}
}

QList<DwarfExpression::Value> DwarfLocationList::getValue(RegisterSet *r, Memory *m, DwarfSubprogram *p, uint16_t pc, bool &isAddress) {
	foreach(DwarfLocation *loc, m_locations) {
		if (loc->contains(pc)) {
			return loc->getExpression()->getValue(r, m, p, pc, isAddress);
		}
	}

	return QList<DwarfExpression::Value>();
}
