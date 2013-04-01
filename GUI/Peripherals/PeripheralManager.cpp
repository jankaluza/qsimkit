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
#include "Peripheral.h"
#include <QPluginLoader>
#include <QApplication>
#include <QDebug>
#include <QDomDocument>

Peripheral *PeripheralInfo::create() {
	return m_peripheral->create();
}

PeripheralManager::PeripheralManager() {

}

PeripheralManager::~PeripheralManager() {
	
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

	QString library;
	PeripheralInfo info;

	for(QDomNode node = peripheral.firstChild(); !node.isNull(); node = node.nextSibling()) {
		QDomElement element = node.toElement();

		if (element.nodeName() == "library") {
			library = file + "/" + element.text();
			info.m_peripheral = loadBinaryPeripheral(file + "/" + element.text());
		}
	}

	if (info.m_peripheral) {
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
	pluginsDir.cd("Peripherals");
	foreach (QString fileName, pluginsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
		loadXML(pluginsDir.absoluteFilePath(fileName));
	}
}
