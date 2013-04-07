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

#include <QMainWindow>
#include <QString>
#include <QTimer>

#include "ui_QSimKit.h"

#include "Peripherals/Peripheral.h"
#include "adevs.h"

class Variant;
class CPU;
class PeripheralManager;
class Disassembler;
class Registers;
class BreakpointManager;

class QSimKit : public QMainWindow, public Ui::QSimKit
{
	Q_OBJECT

	public:
		QSimKit(QWidget *parent = 0);

		void setVariant(const QString &variant);
		bool loadA43File(const QString &file);
		bool loadELFFile(const QString &file);
		bool loadProject(const QString &file);

		BreakpointManager *getBreakpointManager() {
			return m_breakpointManager;
		}

	public slots:
		void loadA43();
		void loadELF();
		void newProject();
		void saveProject();
		void loadProject();
		void projectOptions();

		void simulationStep();
		void singleStep();

		void startSimulation();
		void stopSimulation();
		void pauseSimulation(bool pause);
		void resetSimulation();

	private:
		void refreshDockWidgets();

	private:
		Variant *m_variant;
		adevs::Digraph<double> *m_dig;
		adevs::Simulator<SimulationEvent> *m_sim;
		QTimer *m_timer;
		PeripheralManager *m_peripherals;
		QString m_filename;
		Disassembler *m_disassembler;
		Registers *m_registers;
		QAction *m_pauseAction;
		BreakpointManager *m_breakpointManager;
};

