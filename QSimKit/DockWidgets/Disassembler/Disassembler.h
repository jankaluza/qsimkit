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
#include <QHash>
#include <stdint.h>

#include "ui_Disassembler.h"

#include "MCU/MCU.h"

class MCU;
class QSimKit;

class Disassembler : public DockWidget, public Ui::Disassembler
{
	Q_OBJECT

	public:
		Disassembler(QSimKit *simkit);

		void setMCU(MCU *mcu);

		MCU *getMCU() {
			return m_mcu;
		}

		void refresh();

		bool isDifferentCLine(uint16_t pc);

		void showSourceCode(bool show);
		void showAssembler(bool show);
		void pointToInstruction(uint16_t pc);

		Subprogram *getCurrentSubprogram();


	public slots:
		void handleContextMenu(const QPoint &point);
		void reloadCode();
		void reloadFile();

	private slots:
		void handleFileChanged(int id);
		void handleShowModeClicked();

	private:
		void addBreakpoint();
		void removeBreakpoint();
		void addSourceLine(uint16_t addr, const QString &line);
		void addInstructionLine(uint16_t addr, const QString &line, const QString &tooltip);
		void addSectionLine(uint16_t addr, const QString &line);
		QString findFileWithAddr(uint16_t addr);
		void loadFileLines(QStringList &lines);
		void loadPairedInstructions(QHash<int, uint16_t> &line2addr);
		void reloadFileAssembler(QStringList &lines);
		void reloadFileSource(QStringList &lines);

	private:
		MCU *m_mcu;
		QSimKit *m_simkit;
		QList<QTreeWidgetItem *> m_currentItems;
		QList<QTreeWidgetItem *> m_breakpoints;
		bool m_showSource;
		bool m_showAssembler;
		DisassembledFiles m_files;
		QString m_currentFile;
		QHash<uint16_t, uint16_t> m_pairedInstructions;
		uint16_t m_pc;
		DebugData *m_dd;
		bool m_showingAssembler;
};

