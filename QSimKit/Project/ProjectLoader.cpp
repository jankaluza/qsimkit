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
