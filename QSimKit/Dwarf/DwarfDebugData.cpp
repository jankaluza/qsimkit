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

#include "DwarfDebugData.h"

#include <QDebug>

DwarfDebugData::DwarfDebugData() {}

DwarfDebugData::~DwarfDebugData() {
	QMap<QString, Subprograms>::iterator i = m_subprograms.begin();
	while (i != m_subprograms.end()) {
		Subprograms &subprograms = i.value();
		foreach(Subprogram *s, subprograms) {
			delete s;
		}
		++i;
	}

	foreach(VariableType *type, m_types) {
		delete type;
	}
}

void DwarfDebugData::addSubprogram(const QString &file, Subprogram *subprogram) {
	m_subprograms[file].append(subprogram);
}

const Subprograms &DwarfDebugData::getSubprograms(const QString &file) {
	return m_subprograms[file];
}

Subprogram *DwarfDebugData::getSubprogram(const QString &file, uint16_t pc) {
	Subprograms &ss = m_subprograms[file];
	foreach(Subprogram *s, ss) {
		if (pc >= s->getPCLow() && pc < s->getPCHigh()) {
			return s;
		}
	}

	return 0;
}

Subprogram *DwarfDebugData::getSubprogram(uint16_t pc) {
	QMap<QString, Subprograms>::iterator i = m_subprograms.begin();
	while (i != m_subprograms.end()) {
		Subprograms &subprograms = i.value();
		foreach(Subprogram *s, subprograms) {
			if (pc >= s->getPCLow() && pc < s->getPCHigh()) {
				return s;
			}
		}
		++i;
	}

	return 0;
}
