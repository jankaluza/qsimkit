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
#include <QList>
#include <QTimer>
#include <QTime>
#include <QLineEdit>
#include <QComboBox>

#include "ui_QSimKit.h"

#include "Peripherals/Peripheral.h"
#include "adevs.h"

class MCU;
class PeripheralManager;
class MCUManager;
class Disassembler;
class BreakpointManager;
class DockWidget;
class Peripherals;
class TrackedPins;

typedef enum {
	SimulationStep,
	AssemblerStep,
	CStep,
} StepMode;

class QSimKit : public QMainWindow, public Ui::QSimKit
{
	Q_OBJECT

	public:
		QSimKit(QWidget *parent = 0);

		bool loadProject(const QString &file);

		void addDockWidget(DockWidget *widget, Qt::DockWidgetArea area);

		void setStepMode(StepMode mode);
		StepMode getStepMode();

		BreakpointManager *getBreakpointManager() {
			return m_breakpointManager;
		}

		Peripherals *getPeripheralsWidget() {
			return m_peripheralsWidget;
		}

		Screen *getScreen();

	public slots:
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

		void showTrackedPins(bool value);
		void pointToInstruction(int pc);

	signals:
		void onSimulationStarted(bool wasPaused);
		void onSimulationPaused();
		void onSimulationStopped();
		void onSimulationStep(double t);

	protected:
		void closeEvent(QCloseEvent *event);

	private:
		void refreshDockWidgets();
		void setDockWidgetsEnabled(bool enabled);
		void setDockWidgetsMCU(MCU *cpu);
		void populateToolBar();
		void readSettings();
		void doSingleAssemblerStep();
		void doSingleCStep();

	private:
		Digraph *m_dig;
		adevs::Simulator<SimulationEvent> *m_sim;
		QTimer *m_timer;
		PeripheralManager *m_peripherals;
		QString m_filename;
		Disassembler *m_disassembler;
		QAction *m_pauseAction;
		BreakpointManager *m_breakpointManager;
		QList<DockWidget *> m_dockWidgets;
		Peripherals *m_peripheralsWidget;
		MCUManager *m_mcuManager;
		TrackedPins *m_trackedPins;
		int m_logicalSteps;
		int m_instPerCycle;
		bool m_stopped;
		QLineEdit *m_runUntil;
		QComboBox *m_stepModeCombo;
		QTime m_simStart;
};

