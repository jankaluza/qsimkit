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

#include "CodeUtil.h"

#include <QFile>
#include <QProcess>
#include <QDir>
#include <QDebug>

#include "GUI/Dwarf/DwarfLoader.h"

namespace CodeUtil {
	
static void parseCode(DisassembledFiles &df, QString &code, QString &error) {
	QString file;
	DisassembledLine pendingSection;
	int num = 0;

	QStringList lines = code.split("\n", QString::SkipEmptyParts);
	for (int i = 0; i < lines.size(); ++i) {
		QString &line = lines[i];
		if (line[0] == ' ') {
			if (line.startsWith("   ") && line[8] == ':') {
				QString l = line.simplified();

				QString addr = l.left(l.indexOf(':'));
				int x;
				int c = 0;
				for (x = l.indexOf(':') + 1; x < l.size(); ++x) {
					if (!l[x].isNumber() && !l[x].isSpace() || (l[x].isSpace() && c > 2)) {
						if (l[x] >= 'a' && l[x] <= 'f') {
							c++;
							continue;
						}
						if (!l[x - 1].isSpace() && !l[x - 1].isNumber()) {
							x -= c;
						}
						break;
					}
					c = 0;
				}
				QString inst = l.mid(x);

				if (inst.isEmpty()) {
					continue;
				}

				df[file].append(DisassembledLine(addr.toInt(0, 16), num, DisassembledLine::Instruction, inst));
			}
			else {
				df[file].append(DisassembledLine(0, num, DisassembledLine::Code, line));
			}
		}
		else if (line[0] == '0') {
			QString addr = line.left(8).right(4);
			pendingSection = DisassembledLine(addr.toInt(0, 16), num, DisassembledLine::Section, line.mid(9));
		}
#ifdef Q_OS_LINUX
		else if (line.startsWith("+<")) {
			file = line.mid(6, line.indexOf(':') - 6);
			num = line.mid(line.indexOf(':') + 1).toInt();
			if (pendingSection.getAddr()) {
				df[file].append(pendingSection);
				pendingSection = DisassembledLine();
			}
		}
#else
		else if (line[1] == ':' && line[2] == '\\') {
			file = line.left(line.lastIndexOf(':'));
			num = line.mid(line.lastIndexOf(':') + 1).toInt();
			if (pendingSection.getAddr()) {
				df[file].append(pendingSection);
				pendingSection = DisassembledLine();
			}
		}
#endif
// 		else if (i > 2) {
// 			df[file].append(DisassembledLine(0, 0, DisassembledLine::Code, line));
// 		}
	}
}

DisassembledFiles disassemble(const QByteArray &elf, const QString &a43, QString &error) {
	DisassembledFiles df;

	bool hasELF = true;
	QByteArray code = elf;
	if (code.isEmpty()) {
		hasELF = false;
		code = a43.toAscii();
	}

	QString f_in = QDir::tempPath() + "/test.dump";
	QFile file(f_in);
	if (!file.open(QFile::WriteOnly | QFile::Truncate | QIODevice::Text)) {
		error = QString("Cannot open '%1' for writing.").arg(f_in);
		return df;
	}

	file.write(code);
	file.close();

	QProcess objdump;
	if (hasELF) {
#ifdef Q_OS_LINUX
		objdump.start("msp430-objdump", QStringList() << "-dSl" << "--prefix=+<FILE" << f_in);
#else
		objdump.start("msp430-objdump", QStringList() << "-dSl" << f_in);
#endif
	}
	else {
		objdump.start("msp430-objdump", QStringList() << "-D" << "-m" << "msp430:430" << f_in);
	}
	
	if (!objdump.waitForStarted()) {
		error = QString("'msp430-objdump' cannot be started. Is msp430-gcc installed and is msp430-objdump in PATH?");
		return df;
	}

	if (!objdump.waitForFinished()) {
		error = QString("'msp430-objdump' did not finish properly.");
		return df;
	}

	QString result = QString(objdump.readAll());
	parseCode(df, result, error);

	DwarfLoader dl("msp430-objdump");
	dl.load(f_in, error);

	return df;
}

QString ELFToA43(const QByteArray &elf, QString &error) {
// 	error = "4";
	QString f_in = QDir::tempPath() + "/test.dump";
	QString f_out = QDir::tempPath() + "/test.a43";
	QFile file(f_in);
	if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
		error = QString("Cannot open '%1' for writting.").arg(f_in);
		return "";
	}

	file.write(elf);
	file.close();

	QProcess objdump;
	objdump.start("msp430-objcopy", QStringList() << "-O" << "ihex" << f_in << f_out);
	if (!objdump.waitForStarted()) {
		error = QString("'msp430-objcopy' cannot be started. Is msp430-gcc installed and is msp430-objcopy in PATH?");
		return "";
 	}
 
	if (!objdump.waitForFinished()) {
		error = QString("'msp430-objcopy' did not finish properly.");
		return "";
	}

	QFile file2(f_out);
	if (!file2.open(QIODevice::ReadOnly | QIODevice::Text)) {
		error = QString("Cannot open '%1' for reading.").arg(f_out);
		return "";
	}

	return file2.readAll();
}

}
