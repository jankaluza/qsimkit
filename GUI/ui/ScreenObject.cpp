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

#include "ScreenObject.h"

ScreenObject::ScreenObject() : QObject(0), m_x(0), m_y(0), m_width(0), m_height(0) {
}

void ScreenObject::save(QTextStream &stream) {
	stream << "    <position x='" << m_x << "' y='" << m_y << "'/>\n";
}

void ScreenObject::setX(int x) {
	movePins(x, m_y);
	m_x = x;
}

void ScreenObject::setY(int y) {
	movePins(m_x, y);
	m_y = y;
}

void ScreenObject::movePins(int x, int y) {
	std::map<int, Pin> &pins = getPins();
	int mx = x - m_x;
	int my = y - m_y;

	for (std::map<int, Pin>::iterator it = pins.begin(); it != pins.end(); ++it) {
		it->second.rect.adjust(mx, my, mx, my);
	}

	objectMoved(x, y);
}
