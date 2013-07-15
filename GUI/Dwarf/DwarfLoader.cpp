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

#include "DwarfLoader.h"
#include "DwarfDebugData.h"

#include <QFile>
#include <QProcess>
#include <QDir>
#include <QDebug>

#include "GUI/MCU/MCU.h"

DwarfLoader::DwarfLoader(const QString &objdump) : m_objdump(objdump) {
	
}

DwarfLoader::~DwarfLoader() {
	
}

bool DwarfLoader::loadVariableTypes(const QString &out, DebugData *dd, QString &error) {
	QStringList lines = code.split("\n", QString::SkipEmptyParts);
	for (int i = 0; i < lines.size(); ++i) {
		QString &line = lines[i];
		
	}

	return true;
}

bool DwarfLoader::loadSubprograms(const QString &out, DebugData *dd, QString &error) {
	QString currentFile;

	QStringList lines = code.split("\n", QString::SkipEmptyParts);
	for (int i = 0; i < lines.size(); ++i) {
		QString &line = lines[i];

		if (line.contains("DW_TAG_compile_unit")) {
			
		}
	}

	return true;
}

DebugData *DwarfLoader::load(QString &file, QString &error) {
	QString out;

	QProcess objdump;
	objdump.start("msp430-objdump", QStringList() << file << "--dwarf=info");
	
	if (!objdump.waitForStarted()) {
		error = QString("'msp430-objdump' cannot be started. Is msp430-gcc installed and is msp430-objdump in PATH?");
		return 0;
	}

	if (!objdump.waitForFinished()) {
		error = QString("'msp430-objdump' did not finish properly.");
		return 0;
	}

	DebugData *dd = new DwarfDebugData();

	QString result = QString(objdump.readAll());
	if (!loadVariableTypes(result, dd, error)) {
		delete dd;
		return 0;
	}

	if (!loadSubprograms(result, dd, error)) {
		delete dd;
		return 0;
	}

	return dd;
}
