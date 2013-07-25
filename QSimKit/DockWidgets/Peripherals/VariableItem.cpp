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

#include "VariableItem.h"

#include "MCU/Memory.h"
#include "MCU/RegisterSet.h"
#include "MCU/Register.h"
#include "MCU/MCU.h"

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

VariableItem::VariableItem(QTreeWidgetItem *parent, Variable *v) : QTreeWidgetItem(parent, VariableItemType), m_v(v) {
	setText(0, v->getName());
	setText(1, "??");
	setBackground(0, QApplication::palette().window());
}

void VariableItem::refresh(RegisterSet *r, Memory *m, Subprogram *s, uint16_t pc) {
	setText(1, m_v->getValue(r, m, s, pc));
}

