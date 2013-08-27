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

#include "Disassembler.h"
#include "DisassemblerItem.h"
#include "QSimKit/DockWidgets/Peripherals/Peripherals.h"

#include "ui/QSimKit.h"
#include "MCU/MCU.h"
#include "MCU/RegisterSet.h"
#include "MCU/Register.h"

#include "Breakpoints/BreakpointManager.h"

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
#include <QTextDocument>

#define SRC_ITEM 0
#define INST_ITEM 1

Disassembler::Disassembler(QSimKit *simkit) :
DockWidget(simkit), m_mcu(0), m_simkit(simkit), m_showSource(true), m_showAssembler(true), m_pc(0),
m_dd(0), m_showingAssembler(0) {
	setupUi(this);

	connect(view, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(handleContextMenu(const QPoint &)) );
	connect(file, SIGNAL(currentIndexChanged(int)), this, SLOT(handleFileChanged(int)));
	connect(showMode, SIGNAL(clicked()), this, SLOT(handleShowModeClicked()));

	m_simkit->getPeripheralsWidget()->addPeripheralItem(new DisassemblerItem(this));
}

void Disassembler::handleShowModeClicked() {
	showAssembler(! m_showAssembler);
}

void Disassembler::handleFileChanged(int id) {
	m_currentFile = file->itemData(id).toString();
	reloadFile();
}

void Disassembler::showAssembler(bool show) {
	m_showAssembler = show;

	if (m_showAssembler) {
		showMode->setText("Show C");
	}
	else {
		showMode->setText("Show ASM");
	}

	uint16_t currentPc = m_pc;

	reloadFile();

	if (currentPc) {
		pointToInstruction(currentPc);
	}
}

void Disassembler::showSourceCode(bool show) {
	for (int i = 0; i < view->topLevelItemCount(); ++i) {
		QTreeWidgetItem *item = view->topLevelItem(i);
		if (item->data(0, Qt::UserRole) == SRC_ITEM) {
			item->setHidden(!show);
		}
	}

	m_showSource = show;
}

void Disassembler::handleContextMenu(const QPoint &pos) {
	QList<QAction *> actions;

	QTreeWidgetItem *item = view->currentItem();

	QAction *add = 0;
	QAction *remove = 0;
	if (!m_breakpoints.contains(item)) {
		add = new QAction("Add breakpoint", 0);
		actions.append(add);
	}
	else {
		remove = new QAction("Remove breakpoint", 0);
		actions.append(remove);
	}

	QAction *showSource = new QAction("Show inline source code", 0);
	showSource->setCheckable(true);
	showSource->setChecked(m_showSource);
	actions.append(showSource);

	QAction *action = QMenu::exec(actions, view->mapToGlobal(pos), 0, 0);
	if (!action) {
		return;
	}

	if (action == add) {
		addBreakpoint();
	}
	else if (action == remove) {
		removeBreakpoint();
	}
	else if (action == showSource) {
		showSourceCode(action->isChecked());
	}
}

void Disassembler::addBreakpoint() {
	QTreeWidgetItem *item = view->currentItem();

	QList<QTreeWidgetItem *> items = view->findItems(item->text(0), Qt::MatchExactly);
	foreach(QTreeWidgetItem *it, items) {
		it->setBackground(0, QBrush(Qt::red));
		m_breakpoints.append(it);
	}

	BreakpointManager *m = m_simkit->getBreakpointManager();
	m->addRegisterBreak(0, item->text(0).toInt(0, 16));
	qDebug() << "break when PC is" << item->text(0).toInt(0, 16);
}

void Disassembler::removeBreakpoint() {
	QTreeWidgetItem *item = view->currentItem();
	QList<QTreeWidgetItem *> items = view->findItems(item->text(0), Qt::MatchExactly);
	foreach(QTreeWidgetItem *it, items) {
		it->setBackground(0, view->palette().window());
		m_breakpoints.removeAll(it);
	}

	BreakpointManager *m = m_simkit->getBreakpointManager();
	m->removeRegisterBreak(0, item->text(0).toInt(0, 16));
	qDebug() << "removing break when PC is" << item->text(0).toInt(0, 16);
}

void Disassembler::addSourceLine(uint16_t addr, const QString &line) {
	QTreeWidgetItem *item = new QTreeWidgetItem(view);
	item->setText(0, addr ? QString::number(addr, 16) : "");
	item->setData(0, Qt::UserRole, SRC_ITEM);
	item->setText(1, line);
	if (addr) {
		item->setBackground(0, QBrush(QColor(200, 255, 200)));
		item->setBackground(1, QBrush(QColor(200, 255, 200)));
	}
	QFont f = item->font(1);
	f.setPointSize(f.pointSize() - 2);
	item->setFont(0, f);
	item->setFont(1, f);
}

void Disassembler::addInstructionLine(uint16_t addr, const QString &line, const QString &tooltip) {
	QTreeWidgetItem *item = new QTreeWidgetItem(view);
	item->setText(0, QString::number(addr, 16));
	item->setData(0, Qt::UserRole, INST_ITEM);
	item->setText(1, "  " + line);
	item->setToolTip(1, tooltip);
	item->setBackground(0, view->palette().window());
}

void Disassembler::addSectionLine(uint16_t addr, const QString &line) {
	QTreeWidgetItem *item = new QTreeWidgetItem(view);
	item->setText(0, QString::number(addr, 16));
	item->setText(1, line);
	QFont f = item->font(1);
	f.setBold(true);
	item->setFont(0, f);
	item->setFont(1, f);
	item->setBackground(0, view->palette().window());
	item->setBackground(1, view->palette().window());
}

void Disassembler::loadFileLines(QStringList &lines) {
	QFile file(m_currentFile);
	if(file.open(QIODevice::ReadOnly)) {
		QTextStream in(&file);

		while(!in.atEnd()) {
			lines.append(in.readLine());
		}

		file.close();
	}
}

void Disassembler::loadPairedInstructions(QHash<int, uint16_t> &line2addr) {
	DisassembledCode &code = m_files[m_currentFile];

	// Paired instructions are used to show proper line in C
	// even when it's compiled to more ASM instructions
	foreach(const DisassembledLine &l, code) {
		switch(l.getType()) {
			case DisassembledLine::Instruction:
				if (!line2addr.contains(l.getLineNumber())) {
					line2addr[l.getLineNumber()] = l.getAddr();
				}
				else {
					// Every instruction is pair with itself. However, we add that pair
					// only for real multi-instruction C commands to save some space and time.
					if (!m_pairedInstructions.contains(line2addr[l.getLineNumber()])) {
						m_pairedInstructions[line2addr[l.getLineNumber()]] = line2addr[l.getLineNumber()];
					}
					m_pairedInstructions[l.getAddr()] = line2addr[l.getLineNumber()];
				}
				break;
			default:
				break;
		}
	}
}

void Disassembler::reloadFileSource(QStringList &lines) {
	QHash<int, uint16_t> line2addr;
	loadPairedInstructions(line2addr);

	int n = 1;
	foreach(const QString &line, lines) {
		if (line2addr.contains(n)) {
			addSourceLine(line2addr[n], line);
		}
		else {
			addSourceLine(0, line);
		}
		++n;
	}

// 	m_simkit->setStepMode(CStep);
}

void Disassembler::reloadFileAssembler(QStringList &lines) {
	QHash<int, uint16_t> line2addr;
	loadPairedInstructions(line2addr);

	QString tooltip = "";
	int i;
	int previousLine = 0;
	DisassembledCode &code = m_files[m_currentFile];
	foreach(const DisassembledLine &l, code) {
		switch(l.getType()) {
			case DisassembledLine::Instruction:
				if (l.getLineNumber() != previousLine) {
					tooltip = "";
					// As a tooltip, set the 10 source code lines around the
					// line for which this instruction is generated.
					i = qMax(l.getLineNumber() - 5, 0);
					for (; i < l.getLineNumber() + 5 && i < lines.size(); ++i) {
						if (i == l.getLineNumber()) {
							tooltip += "<b>" + Qt::escape(lines[i - 1]) + "</b>";
							addSourceLine(l.getAddr(), lines[i - 1]);
						}
						else {
							tooltip += Qt::escape(lines[i - 1]);
						}
						tooltip += "<br/>";
					}
					previousLine = l.getLineNumber();
				}

				addInstructionLine(l.getAddr(), l.getData(), tooltip);
				break;
			case DisassembledLine::Section:
				addSectionLine(l.getAddr(), l.getData());
				break;
		}
	}

// 	m_simkit->setStepMode(AssemblerStep);
}

void Disassembler::reloadFile() {
	view->clear();
	m_currentItems.clear();
	m_pc = 0;
	m_pairedInstructions.clear();

	QStringList lines;
	loadFileLines(lines);

	if (m_showAssembler || lines.isEmpty()) {
		m_showingAssembler = true;
		reloadFileAssembler(lines);
	}
	else {
		m_showingAssembler = false;
		reloadFileSource(lines);
	}

	if (m_dd) {
		func->clear();

		Subprograms subprograms = m_dd->getSubprograms(file->currentText());
		foreach(const Subprogram *s, subprograms) {
			func->addItem(s->getName());
		}
	}
}

Subprogram *Disassembler::getCurrentSubprogram() {
	if (!m_dd) {
		return 0;
	}

	Subprograms subprograms = m_dd->getSubprograms(file->currentText());
	foreach(Subprogram *s, subprograms) {
		if (s->contains(m_pc)) {
			return s;
		}
	}
	return 0;
}

void Disassembler::reloadCode() {
	view->clear();
	file->clear();
	func->clear();
	m_currentItems.clear();
	m_pc = 0;
	m_currentFile = "";

	if (!m_mcu) {
		return;
	}

	// Fill in the files list
	m_files = m_mcu->getDisassembledCode();
	DisassembledFiles::iterator it = m_files.begin();
	while (it != m_files.end()) {
		QString f = it.key();
		f = f.mid(f.lastIndexOf("/") + 1);
		file->addItem(f, it.key());
		++it;
	}

	delete m_dd;
	m_dd = m_mcu->getDebugData();
	if (!m_dd) {
		qDebug() << "Warning: No DWARF debug data available";
	}

	view->resizeColumnToContents(0);
}

QString Disassembler::findFileWithAddr(uint16_t addr) {
	DisassembledFiles::iterator it = m_files.begin();
	while (it != m_files.end()) {
		DisassembledCode &code = it.value();
		foreach(const DisassembledLine &l, code) {
			if (l.getAddr() == addr) {
				return it.key();
			}
		}
		++it;
	}
	return QString();
}

bool Disassembler::isDifferentCLine(uint16_t pc) {
	if (m_pairedInstructions.contains(m_pc)) {
		if (m_pairedInstructions.contains(pc)) {
			return m_pairedInstructions[pc] != m_pairedInstructions[m_pc];
		}
	}
	// If we don't have any information about paired instructions, it's
	// always different C line. This covers also situation where C code
	// can't be loaded or simply does not exist.
	return true;
}

void Disassembler::pointToInstruction(uint16_t pc) {
	m_pc = pc;
	if (!m_showingAssembler && m_pairedInstructions.contains(pc)) {
		pc = m_pairedInstructions[pc];
	}

	QString addr = QString("%1").arg(pc, 0, 16);
	QList<QTreeWidgetItem *> items = view->findItems(addr, Qt::MatchExactly);
	if (items.empty()) {
		// If we don't have any item with this PC, maybe it is defined in
		// different file. Try to find that file, load it and rerun ourself
		QString f = findFileWithAddr(pc);
		if (f.isEmpty()) {
			return;
		}

		// If the file with addr is the same as the currently loaded file,
		// we are probably showing C code and not assembler one (view has
		// been populated using reloadFileSource()) and therefore there's
		// nothing to point to right now.
		if (m_currentFile == f) {
			return;
		}

		m_currentFile = f;
		file->setCurrentIndex(file->findData(f));
		reloadFile();
		pointToInstruction(pc);
		return;
	}

	// If we have previous items, change their background colors to the
	// original one.
	for (int i = 0; i < m_currentItems.size(); ++i) {
		if (m_currentItems[i]->data(0, Qt::UserRole) == SRC_ITEM) {
			m_currentItems[i]->setBackground(1, QBrush(QColor(200, 255, 200)));
		}
		else {
			m_currentItems[i]->setBackground(1, view->palette().base());
		}
	}

	// Change background of current items to Green.
	m_currentItems = items;
	for (int i = 0; i < m_currentItems.size(); ++i) {
		m_currentItems[i]->setBackground(1, QBrush(Qt::green));
	}

	view->scrollToItem(m_currentItems[0]);
}

void Disassembler::refresh() {
	pointToInstruction(m_mcu->getRegisterSet()->get(0)->getBigEndian());
}

void Disassembler::setMCU(MCU *mcu) {
	m_mcu = mcu;
	connect(m_mcu, SIGNAL(onCodeLoaded()), this, SLOT(reloadCode()) );
	reloadCode();
	refresh();
}
