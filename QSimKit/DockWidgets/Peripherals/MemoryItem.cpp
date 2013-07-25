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

MemoryItem::MemoryItem(QTreeWidgetItem *parent, const QString &name, uint16_t addr) : QTreeWidgetItem(parent, MemoryItemType) {
	setData(0, Qt::UserRole, addr);
	setText(0, name);
	setText(1, 0x00);
	setBackground(0, QApplication::palette().window());
}

void MemoryItem::refresh(Memory *mem) {
	int address = data(0, Qt::UserRole).toInt();

	int16_t n = mem->getBigEndian(address, false);
	QString dec = QString::number(n);
	QString hex = QString("0x%1").arg((uint16_t) n, 0, 16);
	QString bin = QString("%1").arg((uint16_t) n, 0, 2);

	setText(1, hex);

	QString tooltip = "Dec: " + dec + "<br/>";
	tooltip += "Hex: " + hex + "<br/>";
	tooltip += "Bin: " + bin;
	setToolTip(1, tooltip);
}

