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

#include "QSimKit.h"

#include "ProjectConfiguration.h"

#include "CPU/Variants/Variant.h"
#include "CPU/Variants/VariantManager.h"
#include "CPU/Memory/RegisterSet.h"
#include "CPU/Memory/Register.h"
#include "Peripherals/MSP430/MSP430.h"
#include "Peripherals/PeripheralManager.h"

#include "DockWidgets/Disassembler/Disassembler.h"
#include "DockWidgets/Registers/Registers.h"

#include "Breakpoints/BreakpointManager.h"

#include <QWidget>
#include <QTime>
#include <QMainWindow>
#include <QString>
#include <QFileDialog>
#include <QInputDialog>
#include <QFile>
#include <QIcon>
#include <QIODevice>
#include <QDebug>
#include <QDomDocument>

QSimKit::QSimKit(QWidget *parent) : QMainWindow(parent), m_variant(0),
m_dig(0), m_sim(0) {
	setupUi(this);

	m_peripherals = new PeripheralManager();
	m_peripherals->loadPeripherals();

	screen->setPeripheralManager(m_peripherals);

	m_breakpointManager = new BreakpointManager();

	connect(actionLoad_A43, SIGNAL(triggered()), this, SLOT(loadA43()) );
	connect(actionLoad_ELF, SIGNAL(triggered()), this, SLOT(loadELF()) );
	connect(actionNew_project, SIGNAL(triggered()), this, SLOT(newProject()) );
	connect(actionSave_project, SIGNAL(triggered()), this, SLOT(saveProject()) );
	connect(actionLoad_project, SIGNAL(triggered()), this, SLOT(loadProject()) );
	connect(actionProject_options, SIGNAL(triggered()), this, SLOT(projectOptions()) );

	QAction *action = toolbar->addAction(QIcon("./icons/22x22/actions/media-playback-start.png"), tr("Start &simulation"));
	connect(action, SIGNAL(triggered()), this, SLOT(startSimulation()));

	action = toolbar->addAction(QIcon("./icons/22x22/actions/media-playback-pause.png"), tr("P&ause simulation"));
	action->setCheckable(true);
	connect(action, SIGNAL(triggered(bool)), this, SLOT(pauseSimulation(bool)));
	m_pauseAction = action;

	action = toolbar->addAction(QIcon("./icons/22x22/actions/media-playback-stop.png"), tr("Sto&p simulation"));
	connect(action, SIGNAL(triggered()), this, SLOT(stopSimulation()));

	action = toolbar->addAction(QIcon("./icons/22x22/actions/media-skip-forward.png"), tr("Single step"));
	connect(action, SIGNAL(triggered()), this, SLOT(singleStep()));

	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(simulationStep()));

	m_disassembler = new Disassembler(this);
	addDockWidget(Qt::RightDockWidgetArea, m_disassembler);

	m_registers = new Registers(this);
	addDockWidget(Qt::RightDockWidgetArea, m_registers);
}

void QSimKit::setVariant(const QString &variant) {
	m_variant = getVariant(variant.toStdString().c_str());
}

void QSimKit::refreshDockWidgets() {
	m_disassembler->updatePC();
	m_registers->refresh();

	statusbar->showMessage(QString::number(m_sim->nextEventTime()));
}

void QSimKit::singleStep() {
	if (!m_dig) {
		resetSimulation();
	}

	double t = m_sim->nextEventTime();

	do {
		m_sim->execNextEvent();
	}
	while (t == m_sim->nextEventTime());
	refreshDockWidgets();
}

void QSimKit::simulationStep() {
	statusbar->showMessage(QString::number(m_sim->nextEventTime()));
	QTime perf;
	perf.start();
	for (int i = 0; i < 10000; ++i) {
		m_sim->execNextEvent();
		if (m_breakpointManager->shouldBreak()) {
			m_pauseAction->setChecked(true);
			pauseSimulation(true);
			return;
		}
	}
	qDebug() << perf.elapsed();
}

void QSimKit::resetSimulation() {
	m_timer->stop();
	m_pauseAction->setChecked(false);

	delete m_dig;
	delete m_sim;

	m_dig = new adevs::Digraph<double>();
	screen->prepareSimulation(m_dig);
	m_sim = new adevs::Simulator<SimulationEvent>(m_dig);
}

void QSimKit::startSimulation() {
	if (m_pauseAction->isChecked()) {
		m_pauseAction->setChecked(false);
	}
	else {
		resetSimulation();
	}
	m_timer->start(100);
}

void QSimKit::stopSimulation() {
	resetSimulation();
}

void QSimKit::pauseSimulation(bool checked) {
	if (checked) {
		m_timer->stop();
		refreshDockWidgets();
	}
	else {
		m_timer->start(100);
	}
}

void QSimKit::newProject() {
	ProjectConfiguration dialog(this);
	if (dialog.exec() == QDialog::Accepted) {
		m_filename = "";
		screen->clear();
		screen->setCPU(dialog.getMSP430());
		m_disassembler->setCPU(screen->getCPU());
		m_registers->setCPU(screen->getCPU());
		m_breakpointManager->setCPU(screen->getCPU());
	}
}

void QSimKit::saveProject() {
	if (m_filename.isEmpty()) {
		m_filename = QFileDialog::getSaveFileName(this);
		if (m_filename.isEmpty()) {
			return;
		}
	}

	QFile file(m_filename);
	if (!file.open(QFile::WriteOnly | QFile::Truncate | QIODevice::Text))
		return;

	QTextStream stream(&file);
	stream << "<qsimkit_project>\n";
	screen->save(stream);
	stream << "</qsimkit_project>\n";
}

bool QSimKit::loadProject(const QString &file) {
    int errorLine, errorColumn;
    QString errorMsg;

	QFile modelFile(file);
	QDomDocument document;
	if (!document.setContent(&modelFile, &errorMsg, &errorLine, &errorColumn))
	{
			QString error("Syntax error line %1, column %2:\n%3");
			error = error
					.arg(errorLine)
					.arg(errorColumn)
					.arg(errorMsg);
			qDebug() << error;
			return false;
	}

	screen->load(document);
	m_filename = file;
	m_disassembler->setCPU(screen->getCPU());
	m_registers->setCPU(screen->getCPU());
	m_breakpointManager->setCPU(screen->getCPU());

	return true;
}

void QSimKit::loadProject() {
	QString filename = QFileDialog::getOpenFileName(this);
	if (filename.isEmpty()) {
		return;
	}

	loadProject(filename);
}

bool QSimKit::loadA43File(const QString &f) {
	if (!screen->getCPU()) {
		newProject();
		if (!screen->getCPU()) {
			return false;
		}
	}

	QFile file(f);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return false;

	bool ret = screen->getCPU()->loadA43(file.readAll().data());
	m_disassembler->reloadCode();
	return ret;
}

void QSimKit::loadA43() {
	QString filename = QFileDialog::getOpenFileName(this);
	if (filename.isEmpty()) {
		return;
	}

	loadA43File(filename);
}

bool QSimKit::loadELFFile(const QString &f) {
	if (!screen->getCPU()) {
		newProject();
		if (!screen->getCPU()) {
			return false;
		}
	}

	QFile file(f);
	if (!file.open(QIODevice::ReadOnly))
		return false;

	QByteArray elf = file.readAll();
	QString a43 = m_disassembler->ELFToA43(elf);

	screen->getCPU()->setELF(elf);

	bool ret = screen->getCPU()->loadA43(a43.toAscii().data());
	m_disassembler->reloadCode();
	return ret;
}

void QSimKit::loadELF() {
	QString filename = QFileDialog::getOpenFileName(this);
	if (filename.isEmpty()) {
		return;
	}

	loadELFFile(filename);
}

void QSimKit::projectOptions() {
	ProjectConfiguration dialog(this, screen->getCPU());
	if (dialog.exec() == QDialog::Accepted) {
		screen->getCPU()->setFrequency(dialog.getFrequency());
	}
}
