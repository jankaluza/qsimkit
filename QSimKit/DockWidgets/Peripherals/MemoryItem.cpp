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

#include "MemoryItem.h"

#include "MCU/Memory.h"

#include <QWidget>
#include <QTime>
#include <QMainWindow>
#include <QString>
#include <QFileDialog>
#include <QInputDialog>
#include <QFile>
#include <QProcess>
#include <QTreeWidgetItem>
#include <QDebug>
#include <QApplication>

MemoryItem::MemoryItem(QTreeWidgetItem *parent, const QString &name,
					   uint16_t addr, const VariableType &type) :
					   QTreeWidgetItem(parent, MemoryItemType), m_type(type) {
	setData(0, Qt::UserRole, addr);
	setText(0, name);
	setText(1, 0x00);
	setBackground(0, QApplication::palette().window());
}

void MemoryItem::format(Memory *mem, uint16_t address, VariableType *type, QString &out, QString &tooltip) {
	QString dec;
	QString hex;
	QString bin;
	int16_t int16;

#define SET_STRINGS(N) 	dec = QString::number(N); \
	hex = QString("0x%1").arg(N, 0, 16); \
	bin = QString("%1").arg(N, 0, 2); \

	switch(type->getEncoding()) {
		default:
		case VariableType::Unsigned:
		case VariableType::UnsignedChar:
			switch(type->getByteSize()) {
				case 1:
					int16 = mem->getByte(address, false);
					SET_STRINGS((uint8_t) int16);
					break;
				default:
				case 2:
					int16 = mem->getBigEndian(address, false);
					SET_STRINGS((uint16_t) int16);
					break;
			};
			out = hex;
			break;
		case VariableType::Signed:
		case VariableType::SignedChar:
			switch(type->getByteSize()) {
				case 1:
					int16 = mem->getByte(address, false);
					SET_STRINGS((int8_t) int16);
					break;
				default:
				case 2:
					int16 = mem->getBigEndian(address, false);
					SET_STRINGS((int16_t) int16);
					break;
			};
			out = hex;
			break;
	};

	tooltip = "Dec: " + dec + "<br/>";
	tooltip += "Hex: " + hex + "<br/>";
	tooltip += "Bin: " + bin;
}

void MemoryItem::refresh(Memory *mem) {
	int address = data(0, Qt::UserRole).toInt();

	QString tooltip;
	QString out;
	MemoryItem::format(mem, address, &m_type, out, tooltip);

	setText(1, out);
	setToolTip(1, tooltip);
}

