#include <QtCore/QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QDomDocument>

#include "MCU/MCUManager.h"
#include "Peripherals/PeripheralManager.h"
#include "Peripherals/SimulationModel.h"
#include "Peripherals/Peripheral.h"
#include "Project/ProjectLoader.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	if (argc != 3) {
		qDebug() << "Usage:" << argv[0] << "<input.qsp>" << "<simulation_time>";
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

	// Create simulation model and object wrappers
	SimulationModel *model = new SimulationModel();

	// Create Simulation object and prepare the simulation
	adevs::Simulator<SimulationEvent> *simulator = p.prepareSimulation(document, model);

	// Run simulation events until 'until'
	double until = QString(argv[2]).toDouble();
	qDebug() << "Starting simulation until" << until << simulator->nextEventTime();
	int eventCount = 0;
	while (simulator->nextEventTime() <= until) {
		simulator->execNextEvent();
		if (++eventCount > 65000) {
			qDebug() << "Time:" << simulator->nextEventTime();
			eventCount = 0;
		}
	}

// 	return a.exec();
}
