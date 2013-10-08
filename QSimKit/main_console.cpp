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

#include <QtCore/QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QDomDocument>

#include "MCU/MCU.h"
#include "MCU/Memory.h"
#include "MCU/RegisterSet.h"
#include "MCU/Register.h"
#include "MCU/MCUManager.h"
#include "Peripherals/PeripheralManager.h"
#include "Peripherals/SimulationModel.h"
#include "Peripherals/Peripheral.h"
#include "Project/ProjectLoader.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	if (argc != 3) {
		qDebug() << "Usage:" << argv[0] << "<input.qsp>" << "<max_simulation_time_in_seconds>";
		qDebug() << "Example:" << argv[0] << "mmc.qsp" << "0.05";
		return -2;
	}

	// Load MCU plugins
	MCUManager *mcuManager = new MCUManager();
	mcuManager->loadMCUs();

	// Load Peripheral plugins
	PeripheralManager *perManager = new PeripheralManager();
	perManager->loadPeripherals();

	// Load XML file with saved project
	int errorLine, errorColumn;
	QString errorMsg;
	QString f(argv[1]);
	QFile modelFile(f);
	QDomDocument document;
	if (!document.setContent(&modelFile, &errorMsg, &errorLine, &errorColumn)) {
			QString error("Syntax error line %1, column %2:\n%3");
			error = error.arg(errorLine).arg(errorColumn).arg(errorMsg);
			qDebug() << error;
			return -1;
	}

	// Load project (all objects) using ProjectLoader
	ProjectLoader p(mcuManager, perManager);
	if (!p.load(document, errorMsg)) {
		qDebug() << errorMsg;
		return -3;
	}

	// Create simulation model
	SimulationModel *model = new SimulationModel();

	// Create Simulation object and prepare the simulation
	adevs::Simulator<SimulationEvent> *simulator = p.prepareSimulation(document, model);

	// get debugging data from ELF binary
	DebugData *dd = p.getMCU()->getDebugData();

	// Run simulation events until 'until' seconds
	double until = QString(argv[2]).toDouble();
	qDebug() << "Starting simulation until" << until;
	long eventCount = 0;
	while (simulator->nextEventTime() <= until) {
		simulator->execNextEvent();
		if (++eventCount > 65000) {
			// Print some useful info... just to show how to access MCU internals
			qDebug() << "Time:" << simulator->nextEventTime();

			uint16_t pc = p.getMCU()->getRegisterSet()->get(0)->getBigEndian();
			qDebug() << "Small register dump:"
				<< "PC:" << pc
				<< "SP:" << p.getMCU()->getRegisterSet()->get(1)->getBigEndian();

			if (dd->getSubprogram(pc)) {
				qDebug() << "Current subprogram:" << dd->getSubprogram(pc)->getName();
			}

			qDebug() << "P1OUT:" << p.getMCU()->getMemory()->getByte(0x0021);
			eventCount = 0;
		}
	}

// 	return a.exec();

	delete simulator;
	delete model;
	delete dd;
	delete mcuManager;
	delete perManager;
}
