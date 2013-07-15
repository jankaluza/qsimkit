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

#include <QDebug>

DwarfSubprogram::DwarfSubprogram(const QString &name, uint16_t pcLow, uint16_t pcHigh) :
Subprogram(name, pcLow, pcHigh) {
	
}

DwarfSubprogram::~DwarfSubprogram() {
	
}

Variables &DwarfSubprogram::getVariables() {
	return m_vars;
}

Variables &DwarfSubprogram::getArgs() {
	return m_args;
}
