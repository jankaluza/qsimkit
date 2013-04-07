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

#include "ui/QSimKit.h"
#include "Peripherals/MSP430/MSP430.h"
#include "CPU/Memory/RegisterSet.h"
#include "CPU/Memory/Register.h"

#include "Breakpoints/BreakpointManager.h"
#include "Breakpoints/RegisterBreakpoint.h"

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

Disassembler::Disassembler(QSimKit *simkit) :
QDockWidget(simkit), m_cpu(0), m_simkit(simkit), m_currentItem(0) {
	setupUi(this);

	connect(view, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(handleContextMenu(const QPoint &)) );
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
}

void Disassembler::addBreakpoint() {
	QTreeWidgetItem *item = view->currentItem();
	item->setBackground(0, QBrush(Qt::red));
	m_breakpoints.append(item);

	BreakpointManager *m = m_simkit->getBreakpointManager();
	m->addRegisterBreak(0, item->text(0).toInt(0, 16));
	qDebug() << "break when PC is" << item->text(0).toInt(0, 16);
}

void Disassembler::removeBreakpoint() {
	QTreeWidgetItem *item = view->currentItem();
	item->setBackground(0, view->palette().window());
	m_breakpoints.removeAll(item);

	BreakpointManager *m = m_simkit->getBreakpointManager();
	m->removeRegisterBreak(0, item->text(0).toInt(0, 16));
	qDebug() << "removing break when PC is" << item->text(0).toInt(0, 16);
}

void Disassembler::parseCode(const QString &code) {
	QStringList lines = code.split("\n", QString::SkipEmptyParts);
	for (int i = 0; i < lines.size(); ++i) {
		QString &line = lines[i];
		if (line[0] == ' ') {
			if (line.startsWith("   ") && line[8] == ':') {
				QString l = line.simplified();

				QString addr = l.left(l.indexOf(':'));
				int x;
				int c = 0;
				for (x = l.indexOf(':') + 1; x < l.size(); ++x) {
					if (!l[x].isNumber() && !l[x].isSpace() || (l[x].isSpace() && c > 2)) {
						if (l[x] >= 'a' && l[x] <= 'f') {
							c++;
							continue;
						}
						if (!l[x - 1].isSpace() && !l[x - 1].isNumber()) {
							x -= c;
						}
						break;
					}
					c = 0;
				}
				QString inst = l.mid(x);

				if (inst.isEmpty()) {
					continue;
				}

				QTreeWidgetItem *item = new QTreeWidgetItem(view);
				item->setText(0, addr);
				item->setText(1, "  " + inst);
				item->setBackground(0, view->palette().window());
			}
			else {
				QTreeWidgetItem *item = new QTreeWidgetItem(view);
				item->setText(0, "");
				item->setText(1, line);
				item->setBackground(0, QBrush(QColor(200, 255, 200)));
				item->setBackground(1, QBrush(QColor(200, 255, 200)));
				QFont f = item->font(1);
				f.setPointSize(f.pointSize() - 2);
				item->setFont(0, f);
				item->setFont(1, f);
				qDebug() << line;
			}
		}
		else if (line[0] == '0') {
			QTreeWidgetItem *item = new QTreeWidgetItem(view);
			QString addr = line.left(8).right(4);
			item->setText(0, addr);
			item->setText(1, line.mid(9));
			QFont f = item->font(1);
			f.setBold(true);
			item->setFont(0, f);
			item->setFont(1, f);
			item->setBackground(0, view->palette().window());
			item->setBackground(1, view->palette().window());
		}
		else {

		}
	}

	view->resizeColumnToContents(0);
}

void Disassembler::reloadCode() {
	if (!m_cpu) {
		return;
	}

	bool elf = true;
	QByteArray code = m_cpu->getELF();
	if (code.isEmpty()) {
		elf = false;
		code = QString::fromStdString(m_cpu->getCode()).toAscii();
	}

	QFile file("test.dump");
	if (!file.open(QFile::WriteOnly | QFile::Truncate | QIODevice::Text))
		return;

	file.write(code);
	file.close();

	QProcess objdump;
	if (elf) {
		objdump.start("msp430-objdump", QStringList() << "-dS" << "test.dump");
	}
	else {
		objdump.start("msp430-objdump", QStringList() << "-D" << "-m" << "msp:43" << "test.dump");
	}
	
	if (!objdump.waitForStarted()) {
		return;
	}

	if (!objdump.waitForFinished())
		return;

	QString result = QString(objdump.readAll());
	parseCode(result);
}

QString Disassembler::ELFToA43(const QByteArray &elf) {
	QProcess objdump;
	objdump.start("msp430-objcopy", QStringList() << "-O" << "ihex" << "test.a43");
	if (!objdump.waitForStarted()) {
		return "";
	}

	if (!objdump.waitForFinished())
		return "";

	QFile file("test.a43");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return "";

	return file.readAll();
}

void Disassembler::updatePC() {
	QString addr = QString("%1").arg(m_cpu->getRegisterSet()->get(0)->getBigEndian(), 0, 16);
	QList<QTreeWidgetItem *> item = view->findItems(addr, Qt::MatchExactly);
	if (item.empty()) {
		return;
	}

	if (m_currentItem) {
		m_currentItem->setBackground(1, view->palette().base());
	}

	m_currentItem = item[item.size() - 1];
	m_currentItem->setBackground(1, QBrush(Qt::green));
	view->scrollToItem(m_currentItem);
}

void Disassembler::setCPU(MSP430 *cpu) {
	m_cpu = cpu;
	reloadCode();
	updatePC();
}
