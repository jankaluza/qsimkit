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
#include <QList>
#include <stdint.h>

#include "ui_Disassembler.h"

class MCU;
class QSimKit;

class Disassembler : public DockWidget, public Ui::Disassembler
{
	Q_OBJECT

	public:
		Disassembler(QSimKit *simkit);

		void setMCU(MCU *mcu);

		void refresh();

		void showSourceCode(bool show);
		void pointToInstruction(uint16_t pc);

	public slots:
		void handleContextMenu(const QPoint &point);
		void reloadCode();

	private:
		void addBreakpoint();
		void removeBreakpoint();
		void addSourceLine(const QString &line);
		void addInstructionLine(uint16_t addr, const QString &line);
		void addSectionLine(uint16_t addr, const QString &line);

	private:
		MCU *m_mcu;
		QSimKit *m_simkit;
		QTreeWidgetItem *m_currentItem;
		QList<QTreeWidgetItem *> m_breakpoints;
		bool m_showSource;
};

