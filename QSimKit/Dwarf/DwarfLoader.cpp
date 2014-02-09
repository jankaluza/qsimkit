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

#include "QSimKit/MCU/MCU.h"

DwarfLoader::DwarfLoader(const QString &objdump) : m_objdump(objdump) {
	
}

DwarfLoader::~DwarfLoader() {
	
}

bool DwarfLoader::loadVariableTypes(const QString &out, DwarfDebugData *dd, QMap<uint16_t, VariableType *> &types, QString &error) {
	QStringList lines = out.split("\n", QString::SkipEmptyParts);
	bool notFound = true;
	int maxTries = 10;
	QStringList notFoundList;

	while (notFound && maxTries-- > 0) {
		notFound = false;
		notFoundList.clear();
		for (int i = 0; i < lines.size(); ++i) {
			QString &line = lines[i];

			if (line.contains("DW_TAG_base_type")) {
				QString name;
				uint16_t addr = 0;
				uint8_t byteSize = 0;
				VariableType::Encoding encoding;

				addr = line.mid(line.lastIndexOf("<") + 1, line.lastIndexOf(">") - line.lastIndexOf("<") - 1).trimmed().toUInt(0, 16);
				if (types.contains(addr)) {
					continue;
				}

				for (i++; i < lines.size(); ++i) { 
					QString &l = lines[i];

					if (l.contains("DW_AT_name")) {
						name = l.mid(l.lastIndexOf(":") + 2).trimmed();
					}
					else if (l.contains("DW_AT_byte_size")) {
						byteSize = l.mid(l.lastIndexOf(":") + 2).trimmed().toUInt(0, 16);
					}
					else if (l.contains("DW_AT_encoding")) {
						encoding = (VariableType::Encoding) l.mid(l.lastIndexOf(":") + 2, 2).trimmed().toUInt(0, 16);
					}
					else if (l.size() > 3 && l[1] == '<' && l[3] == '>') {
						break;
					}
				}

				i--; // Otherwise we would skip next header

				if (byteSize != 0) {
					VariableType *type = new VariableType(name, byteSize, encoding, VariableType::Base);
					types[addr] = type;
					dd->addVariableType(type);
				}
			}
			else if (line.contains("DW_TAG_subroutine_type")) {
				uint16_t addr = line.mid(line.lastIndexOf("<") + 1, line.lastIndexOf(">") - line.lastIndexOf("<") - 1).trimmed().toUInt(0, 16);
				if (types.contains(addr)) {
					continue;
				}

				VariableType *type = new VariableType("", 0, VariableType::Address, VariableType::Subroutine);
				types[addr] = type;
				dd->addVariableType(type);
			}
			else {
				VariableType::Type type;
				if (line.contains("DW_TAG_volatile_type")) {
					type = VariableType::Volatile;
				}
				else if (line.contains("DW_TAG_const_type")) {
					type = VariableType::Const;
				}
				else if (line.contains("DW_TAG_array_type")) {
					type = VariableType::Array;
				}
				else if (line.contains("DW_TAG_pointer_type")) {
					type = VariableType::Pointer;
				}
				else {
					continue;
				}

				uint16_t addr = line.mid(line.lastIndexOf("<") + 1, line.lastIndexOf(">") - line.lastIndexOf("<") - 1).trimmed().toUInt(0, 16);
				if (types.contains(addr)) {
					continue;
				}

				VariableType *t = 0;
				int16_t subrange = 0;
				uint8_t byteSize = 0;
				bool typeNotFound = false;
				for (i++; i < lines.size(); ++i) { 
					QString &l = lines[i];
					if (l.contains("DW_AT_type") && subrange == 0) {
						uint16_t address = l.mid(l.lastIndexOf("<") + 1, l.lastIndexOf(">") - l.lastIndexOf("<") - 1).trimmed().toUInt(0, 16);
						if (types.contains(address) && types[address] != NULL) {
							t = types[address];
						}
						else {
							typeNotFound = true;
							notFoundList << l;
						}
					}
					else if (l.contains("DW_AT_byte_size")) {
						byteSize = l.mid(l.lastIndexOf(":") + 2).trimmed().toUInt(0, 16);
					}
					else if (l.contains("DW_TAG_subrange_type")) {
						subrange = -1;
					}
					else if (l.contains("DW_AT_upper_bound")) {
						subrange = l.mid(l.lastIndexOf(":") + 2).trimmed().toUInt(0, 10);
					}
					else if (l.size() > 3 && l[1] == '<' && l[3] == '>') {
						i--;
						break;
					}
				}

				if (typeNotFound) {
					notFound = true;
					continue;
				}

				VariableType *typeCopy = new VariableType("", byteSize, VariableType::Address, type, subrange, t);
				types[addr] = typeCopy;
				dd->addVariableType(typeCopy);
			}
		}
	}

	if (!notFoundList.isEmpty()) {
		qDebug() << "Didn't find following DWARF types:" << notFoundList;
	}
	return !notFound;
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
		QString exprString = t.mid(t.indexOf("(") + 1, t.lastIndexOf(")") - t.indexOf("("));
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

void DwarfLoader::parseLocation(const QString &l, QMap<uint16_t, DwarfLocationList *> &locations, DwarfLocationList **ll, DwarfExpression **expr) {
	if (l.contains("location list")) {
		QString keyStr = l.mid(l.lastIndexOf(":") + 2, l.lastIndexOf("(") - l.lastIndexOf(":") - 2).trimmed();
		uint16_t key = keyStr.toUInt(0, 16);
		*ll = locations[key];
		// Every DWARF location list is used only once, so we can remove it from
		// locations variable now.
		locations.remove(key);
	}
	else {
		QString exprString = l.mid(l.indexOf("(") + 1, l.lastIndexOf(")") - l.indexOf("("));
		*expr = new DwarfExpression(exprString);
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

bool DwarfLoader::loadSubprograms(const QString &out, DwarfDebugData *dd,
								  QMap<uint16_t, DwarfLocationList *> &locations,
								  QMap<uint16_t, VariableType *> &types,
								  QString &error) {
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
					pcHigh = l.mid(l.lastIndexOf(":") + 2).trimmed().toUInt(0, 16);
				}
				else if (l.contains("DW_AT_frame_base")) {
					parseLocation(l, locations, &ll, &expr);
				}
				else if (l.size() > 3 && l[1] == '<' && l[3] == '>') {
					break;
				}
			}

			i--; // Otherwise we would skip next header

			if (ll || expr) {
				currentSubprogram = new DwarfSubprogram(name, pcLow, pcHigh, ll, expr);
				dd->addSubprogram(currentFile, currentSubprogram);
			}
		}
		else if (line.contains("DW_TAG_variable")) {
			if (line[2] == '2') {
				// Variable related to 'currentSubprogram'
				QString name;
				DwarfLocationList *ll = 0;
				DwarfExpression *expr = 0;
				VariableType *type = 0;

				for (i++; i < lines.size(); ++i) { 
					QString &l = lines[i];

					if (l.contains("DW_AT_name")) {
						name = l.mid(l.lastIndexOf(":") + 2).trimmed();
					}
					else if (l.contains("DW_AT_location")) {
						parseLocation(l, locations, &ll, &expr);
					}
					else if (l.contains("DW_AT_type")) {
						uint16_t addr = l.mid(l.lastIndexOf("<") + 1, l.lastIndexOf(">") - l.lastIndexOf("<") - 1).trimmed().toUInt(0, 16);
						type = types[addr];
						if (type == 0) {
							qDebug() << "Unknown type" << l;
						}
					}
					else if (l.size() > 3 && l[1] == '<' && l[3] == '>') {
						break;
					}
				}

				i--; // Otherwise we would skip next header

				if (ll || expr) {
					DwarfVariable *v = new DwarfVariable(type, name, ll, expr);
					currentSubprogram->addVariable(v);
				}
			}
			else {
				// Variable related to 'currentFile'
			}
		}
		else if (line.contains("DW_TAG_formal_parameter")) {
			QString name;
			DwarfLocationList *ll = 0;
			DwarfExpression *expr = 0;
			VariableType *type = 0;

			for (i++; i < lines.size(); ++i) { 
				QString &l = lines[i];

				if (l.contains("DW_AT_name")) {
					name = l.mid(l.lastIndexOf(":") + 2).trimmed();
				}
				else if (l.contains("DW_AT_location")) {
					parseLocation(l, locations, &ll, &expr);
				}
				else if (l.contains("DW_AT_type")) {
					uint16_t addr = l.mid(l.lastIndexOf("<") + 1, l.lastIndexOf(">") - l.lastIndexOf("<") - 1).trimmed().toUInt(0, 16);
					type = types[addr];
					if (type == 0) {
						qDebug() << "Unknown type" << l;
					}
				}
				else if (l.size() > 3 && l[1] == '<' && l[3] == '>') {
					break;
				}
			}

			i--; // Otherwise we would skip next header

			if (ll || expr) {
				DwarfVariable *v = new DwarfVariable(type, name, ll, expr);
				currentSubprogram->addArg(v);
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

	QMap<uint16_t, VariableType *> types;
	if (!loadVariableTypes(result, dd, types, error)) {
		delete dd;
		return 0;
	}

	QMap<uint16_t, DwarfLocationList *> locations;
	loadLocations(file, locations, error);

	if (!loadSubprograms(result, dd, locations, types, error)) {
		delete dd;
		dd = 0;
	}

	QMap<uint16_t, DwarfLocationList *>::iterator i = locations.begin();
	while (i != locations.end()) {
		delete i.value();
		++i;
	}


	return dd;
}
