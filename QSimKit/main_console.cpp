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
	// TODO: Move that into ProjectLoader...?
	SimulationModel *model = new SimulationModel();

	// Stores object -> wrapper mapping
	std::map<ScreenObject *, SimulationObjectWrapper *> wrappers;

	// Iterate over all peripherals to create wrappers
	for (int i = 0; i < p.getObjects().size(); ++i) {
		Peripheral *per = dynamic_cast<Peripheral *>(p.getObjects()[i]);
		if (per) {
			// reset peripheral
			per->reset();

			// Create wrapper object for the adevs simulation
			SimulationObjectWrapper *wrapper = new SimulationObjectWrapper(per);
			model->add(wrapper);
			per->setWrapper(wrapper);

			// Store the wrapper
			wrappers[p.getObjects()[i]] = wrapper;

			// Some peripherals have extra internal objects which have to be
			// simulated, so add them into the simulation too.
			std::vector<SimulationObject *> internalObjects;
			per->getInternalSimulationObjects(internalObjects);
			for (int x = 0; x < internalObjects.size(); ++x) {
				SimulationObjectWrapper *wrapper = new SimulationObjectWrapper(internalObjects[x]);
				model->add(wrapper);
			}
		}
	}

	// Load connections from XML file and couple the objects
	// TODO: Move that into ProjectLoader...?
	QDomElement root = document.firstChild().toElement();
	QDomElement connections = root.firstChildElement("connections");
	for(QDomNode node = connections.firstChild(); !node.isNull(); node = node.nextSibling()) {
		QDomElement c = node.toElement();

		ScreenObject *from = p.getObjects()[c.attribute("from").toInt()];
		ScreenObject *to = p.getObjects()[c.attribute("to").toInt()];
		int fpin = c.attribute("fpin").toInt();
		int tpin = c.attribute("tpin").toInt();

		// Connect pins between these two objects
		wrappers[from]->couple(fpin, wrappers[to], tpin);
		wrappers[to]->couple(tpin, wrappers[from], fpin);
	}

	// Create Simulation object
	adevs::Simulator<SimulationEvent> *simulator = new adevs::Simulator<SimulationEvent>(model);

	// Pair simulator with wrapper objects
	for (std::map<ScreenObject *, SimulationObjectWrapper *>::iterator it = wrappers.begin(); it != wrappers.end(); ++it) {
		it->second->setSimulator(simulator);
	}

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
