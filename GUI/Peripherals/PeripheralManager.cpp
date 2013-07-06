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

#include "PeripheralManager.h"
#include "PeripheralInterface.h"
#include "PythonPeripheralInterface.h"
#include "Peripheral.h"
#include "Script/ScriptEngine.h"
#include <QPluginLoader>
#include <QApplication>
#include <QDebug>
#include <QDomDocument>

Peripheral *PeripheralInfo::create() {
	Peripheral *p = m_peripheral->create();
	p->m_type = m_library;
	p->m_interface = "peripheral";
	return p;
}

PeripheralManager::PeripheralManager() {
	m_scriptEngine = new ScriptEngine();
}

PeripheralManager::~PeripheralManager() {
	delete m_scriptEngine;
}

bool PeripheralManager::loadXML(QString file) {
	int errorLine, errorColumn;
	QString errorMsg;

	QFile modelFile(file + "/peripheral.xml");
	QDomDocument document;
	if (!document.setContent(&modelFile, &errorMsg, &errorLine, &errorColumn))
	{
// 			QString error("Syntax error line %1, column %2:\n%3");
// 			error = error
// 					.arg(errorLine)
// 					.arg(errorColumn)
// 					.arg(errorMsg);
// 			qDebug() << error;
			return false;
	}

	QDomElement peripheral = document.firstChild().toElement();
	if (peripheral.nodeName() != "peripheral") {
		return false;
	}

	QString type = peripheral.attribute("type");

	QString library;
	PeripheralInfo info;

	for(QDomNode node = peripheral.firstChild(); !node.isNull(); node = node.nextSibling()) {
		QDomElement element = node.toElement();

		if (element.nodeName() == "library") {
			library = element.text();
			info.m_library = library;
			if (type == "binary") {
				info.m_peripheral = loadBinaryPeripheral(file + "/" + element.text());
			}
			else {
				info.m_peripheral = loadPythonPeripheral(file + "/" + element.text(), element.text());
			}
		}
		else if (element.nodeName() == "name") {
			info.m_name = element.text();
		}
	}

	if (info.m_peripheral) {
		qDebug() << "adding peripheral" << library;
		m_peripherals[library] = info;
	}
}

PeripheralInterface *PeripheralManager::loadBinaryPeripheral(QString f) {
	QStringList extensions = QStringList() << ".dll" << ".so";

	foreach(const QString& ext, extensions) {
		QPluginLoader pluginLoader(f + ext);
		QObject *plugin = pluginLoader.instance();
		if (plugin) {
			PeripheralInterface *p = qobject_cast<PeripheralInterface *>(plugin);
			if (p) {
				qDebug() << "Loaded peripheral:" << (f + ext);
				return p;
			}
		}
	}

	return 0;
}

PeripheralInterface *PeripheralManager::loadPythonPeripheral(QString f, QString name) {
	Script *script = 0;
	QString moduleName = QString("Plugin") + name;

	if((script = m_scriptEngine->load(f + ".py", moduleName)) == 0) {
		return 0;
	}

	qDebug() << "Loaded peripheral:" << (f + ".py");
	PythonPeripheralInterface *p = new PythonPeripheralInterface(script);
	return p;
}

void PeripheralManager::loadPeripherals() {
	QDir pluginsDir(QApplication::applicationDirPath());
#if defined(Q_OS_WIN)
	if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
		pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
	if (pluginsDir.dirName() == "MacOS") {
		pluginsDir.cdUp();
		pluginsDir.cdUp();
		pluginsDir.cdUp();
	}
#endif
	
	if (pluginsDir.exists("Peripherals")) {
		pluginsDir.cd("Peripherals");
	}
	else {
		pluginsDir = QString(MODULES_DIR) + "/peripheral";
	}
	foreach (QString fileName, pluginsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
		loadXML(pluginsDir.absoluteFilePath(fileName));
	}
}
