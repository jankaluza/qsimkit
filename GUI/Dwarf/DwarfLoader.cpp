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
#include "DwarfSubprogram.h"
#include "DwarfLocation.h"
#include "DwarfLocationList.h"
#include "DwarfExpression.h"
#include "DwarfVariable.h"

#include <QFile>
#include <QProcess>
#include <QDir>
#include <QDebug>

#include "GUI/MCU/MCU.h"

DwarfLoader::DwarfLoader(const QString &objdump) : m_objdump(objdump) {
	
}

DwarfLoader::~DwarfLoader() {
	
}

bool DwarfLoader::loadVariableTypes(const QString &out, DwarfDebugData *dd, QString &error) {
	QStringList lines = out.split("\n", QString::SkipEmptyParts);
	for (int i = 0; i < lines.size(); ++i) {
		QString &line = lines[i];
		
	}

	return true;
}

bool DwarfLoader::loadLocations(QString &file, QMap<uint16_t, DwarfLocationList *> &locations, QString &error) {
	QProcess objdump;
	objdump.start("msp430-objdump", QStringList() << file << "--dwarf=loc");
	
	if (!objdump.waitForStarted()) {
		error = QString("'msp430-objdump' cannot be started. Is msp430-gcc installed and is msp430-objdump in PATH?");
		return 0;
	}

	if (!objdump.waitForFinished()) {
		error = QString("'msp430-objdump' did not finish properly.");
		return 0;
	}

	QString out = QString(objdump.readAll());

	uint16_t currentAddr = 0;
	QStringList lines = out.split("\n", QString::SkipEmptyParts);
	foreach(const QString &line, lines) {
		QString t = line.trimmed();

		QStringList words = t.split(" ", QString::SkipEmptyParts);
		if (words.size() == 0) {
			continue;
		}

		if (words[0][0] != '0') {
			continue;
		}

		uint16_t addr = words[0].toUInt(0, 16);
		uint16_t pcLow = words[1].toUInt(0, 16);
		uint16_t pcHigh = words[2].toUInt(0, 16);
		QString exprString = t.mid(t.indexOf("("), t.lastIndexOf(")") - t.indexOf("("));

		if (!exprString.isEmpty()) {
			DwarfExpression *expr = new DwarfExpression(exprString);
			DwarfLocation *loc = new DwarfLocation(expr, pcLow, pcHigh);
			if (!locations.contains(addr)) {
				locations[addr] = new DwarfLocationList();
			}
			locations[addr]->append(loc);
		}
	}
}

#define SCROLL_TO(X, TO) \
	for (X = i + 1; X < lines.size(); ++X) { \
		QString &l = lines[X];\
		if (l.contains(TO)) { \
			break; \
		} \
		else if (l.size() > 3 && l[1] == '<' && l[3] == '>') { \
			X = -1; \
			break; \
		} \
	}

bool DwarfLoader::loadSubprograms(const QString &out, DwarfDebugData *dd, QMap<uint16_t, DwarfLocationList *> &locations, QString &error) {
	int x;
	QString currentFile;
	DwarfSubprogram *currentSubprogram = 0;

	QStringList lines = out.split("\n", QString::SkipEmptyParts);
	for (int i = 0; i < lines.size(); ++i) {
		QString &line = lines[i];

		if (line.contains("DW_TAG_compile_unit")) {
			SCROLL_TO(x, "DW_AT_name");
			if (x > 0) {
				currentFile = lines[x];
				if (currentFile.contains("/")) {
					currentFile = currentFile.mid(currentFile.lastIndexOf("/") + 1);
				}
				else {
					currentFile = currentFile.mid(currentFile.lastIndexOf(":") + 2);
				}
				currentFile = currentFile.trimmed();
			}
		}
		else if (line.contains("DW_TAG_subprogram")) {
			QString name;
			uint16_t pcLow;
			uint16_t pcHigh;
			DwarfLocationList *ll = 0;
			DwarfExpression *expr = 0;

			for (i++; i < lines.size(); ++i) { 
				QString &l = lines[i];

				if (l.contains("DW_AT_name")) {
					name = l.mid(l.lastIndexOf(":") + 2).trimmed();
				}
				else if (l.contains("DW_AT_low_pc")) {
					pcLow = l.mid(l.lastIndexOf(":") + 2).trimmed().toUInt(0, 16);
				}
				else if (l.contains("DW_AT_high_pc")) {
					pcLow = l.mid(l.lastIndexOf(":") + 2).trimmed().toUInt(0, 16);
				}
				else if (l.contains("DW_AT_frame_base")) {
					if (l.contains("location list")) {
						uint16_t key = l.mid(l.lastIndexOf(":") + 2, l.lastIndexOf("(") - l.lastIndexOf(":") + 2).trimmed().toUInt(0, 16);
						ll = locations[key];
					}
					else {
						QString exprString = l.mid(l.indexOf("("), l.lastIndexOf(")") - l.indexOf("("));
						expr = new DwarfExpression(exprString);
					}
				}
				else if (l.size() > 3 && l[1] == '<' && l[3] == '>') {
					break;
				}
			}

			i--; // Otherwise we would skip next header

			currentSubprogram = new DwarfSubprogram(name, pcLow, pcHigh, ll, expr);
			dd->addSubprogram(currentFile, currentSubprogram);
		}
		else if (line.contains("DW_TAG_variable")) {
			if (line[2] == '2') {
				// Variable related to 'currentSubprogram'
				QString name;
				DwarfLocationList *ll = 0;
				DwarfExpression *expr = 0;

				for (i++; i < lines.size(); ++i) { 
					QString &l = lines[i];

					if (l.contains("DW_AT_name")) {
						name = l.mid(l.lastIndexOf(":") + 2).trimmed();
					}
					else if (l.contains("DW_AT_location")) {
						if (l.contains("location list")) {
							uint16_t key = l.mid(l.lastIndexOf(":") + 2, l.lastIndexOf("(") - l.lastIndexOf(":") + 2).trimmed().toUInt(0, 16);
							ll = locations[key];
						}
						else {
							QString exprString = l.mid(l.indexOf("("), l.lastIndexOf(")") - l.indexOf("("));
							expr = new DwarfExpression(exprString);
						}
					}
					else if (l.size() > 3 && l[1] == '<' && l[3] == '>') {
						break;
					}
				}

				i--; // Otherwise we would skip next header

				qDebug() << name << ll << expr;
				DwarfVariable *v = new DwarfVariable(name, ll, expr);
				currentSubprogram->addVariable(v);
			}
			else {
				// Variable related to 'currentFile'
			}
		}
	}

	return true;
}

DebugData *DwarfLoader::load(QString &file, QString &error) {
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

	DwarfDebugData *dd = new DwarfDebugData();

	QString result = QString(objdump.readAll());
	if (!loadVariableTypes(result, dd, error)) {
		delete dd;
		return 0;
	}

	QMap<uint16_t, DwarfLocationList *> locations;
	loadLocations(file, locations, error);

	if (!loadSubprograms(result, dd, locations, error)) {
		delete dd;
		return 0;
	}

	return dd;
}
