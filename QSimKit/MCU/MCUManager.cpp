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

#include "MCUManager.h"
#include "MCUInterface.h"
#include "MCU.h"
#include "Script/ScriptEngine.h"
#include <QPluginLoader>
#include <QApplication>
#include <QDebug>
#include <QDomDocument>

MCU *MCUInfo::create(const QString &variant) {
	MCU *p = m_mcu->create(variant);
	p->m_type = m_library;
	p->m_interface = "mcu";
	return p;
}

MCUManager::MCUManager() {
}

MCUManager::~MCUManager() {
}

bool MCUManager::loadXML(QString file) {
	int errorLine, errorColumn;
	QString errorMsg;

	QFile modelFile(file + "/mcu.xml");
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
	if (peripheral.nodeName() != "mcu") {
		return false;
	}

	QString type = peripheral.attribute("type");

	QString library;
	MCUInfo info;

	for(QDomNode node = peripheral.firstChild(); !node.isNull(); node = node.nextSibling()) {
		QDomElement element = node.toElement();

		if (element.nodeName() == "library") {
			library = element.text();
			info.m_library = library;
			if (type == "binary") {
#if defined(Q_OS_WIN)
				info.m_mcu = loadBinaryMCU(file + "/" + element.text());
#else
				info.m_mcu = loadBinaryMCU(file + "/lib" + element.text());
#endif
				
			}
		}
		else if (element.nodeName() == "name") {
			info.m_name = element.text();
		}
	}

	if (info.m_mcu) {
		qDebug() << "adding MCU" << library;
		m_mcu[library] = info;
	}
}

MCUInterface *MCUManager::loadBinaryMCU(QString f) {
	QStringList extensions = QStringList() << ".so" << ".dll";

	foreach(const QString& ext, extensions) {
		QPluginLoader pluginLoader(f + ext);
		QObject *plugin = pluginLoader.instance();
		if (plugin) {
			MCUInterface *p = qobject_cast<MCUInterface *>(plugin);
			if (p) {
				qDebug() << "Loaded peripheral:" << (f + ext);
				return p;
			}
		}
		else {
			qDebug() << "Error loading" << (f + ext) << pluginLoader.errorString();
		}
	}

	return 0;
}

void MCUManager::loadMCUs() {
	QDir pluginsDir(QApplication::applicationDirPath());
#if defined(Q_OS_WIN)
	pluginsDir.cd("qsimkit");
	pluginsDir.cd("mcu");
#elif defined(Q_OS_MAC)
	if (pluginsDir.dirName() == "MacOS") {
		pluginsDir.cdUp();
		pluginsDir.cdUp();
		pluginsDir.cdUp();
	}
#else
	if (pluginsDir.exists("MCU")) {
		pluginsDir.cd("MCU");
	}
	else {
		pluginsDir = QString(MODULES_DIR) + "/mcu";
	}
#endif

	qDebug() << "Trying to load MCUs from " << pluginsDir;
	foreach (QString fileName, pluginsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
		loadXML(pluginsDir.absoluteFilePath(fileName));
	}
}
