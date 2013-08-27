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

#pragma once

#include <QDialog>
#include <QString>
#include <QTimer>
#include <DockWidgets/DockWidget.h>
#include <QTreeWidgetItem>
#include "MCU/MCU.h"

#include <stdint.h>

#define MemoryItemType (QTreeWidgetItem::UserType + 1)

class Memory;

class MemoryItem : public QTreeWidgetItem
{
	public:
		MemoryItem(QTreeWidgetItem *parent, const QString &name, uint16_t addr, const VariableType &type);

		static void format(Memory *mem, uint16_t address, VariableType *type, QString &out, QString &tooltip);

		void refresh(Memory *mem);

	private:
		static void formatBase(Memory *mem, uint16_t address, VariableType *type, QString &out, QString &tooltip);
		static void formatArray(Memory *mem, uint16_t address, VariableType *type, QString &out, QString &tooltip);

	private:
		VariableType m_type;

};

