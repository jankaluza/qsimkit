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

#include "ProjectLoader.h"
#include <QDebug>
#include "Peripherals/PeripheralManager.h"
#include "Peripherals/SimulationModel.h"
#include "MCU/MCUManager.h"
#include "MCU/MCU.h"
#include "ui/ConnectionNode.h"

ProjectLoader::ProjectLoader(MCUManager *mcuManager, PeripheralManager *perManager)
: m_mcu(0), m_mcuManager(mcuManager), m_perManager(perManager) {
}

ProjectLoader::~ProjectLoader() {

}

bool ProjectLoader::load(QDomDocument &doc, QString &error) {
	QDomElement root = doc.firstChild().toElement();
	QDomElement objects = root.firstChildElement("objects");

	for(QDomNode node = objects.firstChild(); !node.isNull(); node = node.nextSibling()) {
		
		QDomElement object = node.toElement();
		QString type = object.attribute("type");

		ScreenObject *obj = 0;
		if (object.attribute("interface") == "mcu") {
			if (!m_mcuManager->hasMCU(type)) {
				error = tr("File you want to load uses MCU module '%1'. This module is not installed or cannot be loaded.").arg(type);
				return false;
			}

			QString variant = object.firstChildElement("variant").text();
			obj = m_mcuManager->getMCU(type).create(variant);
		}
		else if (type == "ConnectionNode") {
			obj = new ConnectionNode();
		}
		else {
			if (!m_perManager->hasPeripheral(type)) {
				error = tr("File you want to load uses Peripheral module '%1'. This module is not installed or cannot be loaded.").arg(type);
				return false;
			}
			obj = m_perManager->getPeripheral(type).create();
		}

		if (!obj) {
			continue;
		}

		obj->load(object, error);
		if (!error.isEmpty()) {
			delete obj;
			return false;
		}

		QDomElement position = object.firstChildElement("position");
		obj->setX(position.attribute("x").toInt());
		obj->setY(position.attribute("y").toInt());

		if (!m_mcu) {
			m_mcu = dynamic_cast<MCU *>(obj);
			m_objects.append(obj);
		}
		else {
			m_objects.append(obj);
		}
	}

	return true;
}

adevs::Simulator<SimulationEvent> *ProjectLoader::prepareSimulation(QDomDocument &doc, SimulationModel *model) {
	// Stores object -> wrapper mapping
	std::map<ScreenObject *, SimulationObjectWrapper *> wrappers;

	// Iterate over all peripherals to create wrappers
	for (int i = 0; i < m_objects.size(); ++i) {
		Peripheral *per = dynamic_cast<Peripheral *>(m_objects[i]);
		if (per) {
			// reset peripheral
			per->reset();

			// Create wrapper object for the adevs simulation
			SimulationObjectWrapper *wrapper = new SimulationObjectWrapper(per);
			model->add(wrapper);
			per->setWrapper(wrapper);

			// Store the wrapper
			wrappers[m_objects[i]] = wrapper;

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
	QDomElement root = doc.firstChild().toElement();
	QDomElement connections = root.firstChildElement("connections");
	for(QDomNode node = connections.firstChild(); !node.isNull(); node = node.nextSibling()) {
		QDomElement c = node.toElement();

		ScreenObject *from = m_objects[c.attribute("from").toInt()];
		ScreenObject *to = m_objects[c.attribute("to").toInt()];
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
	
	return simulator;
}
