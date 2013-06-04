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

#include "Package.h"
#include "MSP430.h"

#include "CPU/Variants/Variant.h"
#include "CPU/Pins/PinMultiplexer.h"

#include <QWidget>
#include <QApplication>
#include <QString>
#include <QFile>
#include <QPen>
#include <QPainter>
#include <QDebug>
#include <QDomDocument>

namespace Package {

void setPinType(const QString &n, MCU::PinType &type, int &subtype) {
#define SET_GP_PIN(PREFIX, T) { \
		if (n.startsWith(PREFIX)) { \
			type = T;\
			subtype = n.right(1).toInt();\
		} \
	}

	SET_GP_PIN("P1.", MCU::P1)
	SET_GP_PIN("P2.", MCU::P2)
	SET_GP_PIN("P3.", MCU::P3)
	SET_GP_PIN("P4.", MCU::P4)
	SET_GP_PIN("P5.", MCU::P5)
	SET_GP_PIN("P6.", MCU::P6)
	SET_GP_PIN("P7.", MCU::P7)
	SET_GP_PIN("P8.", MCU::P8)
}

bool loadPackage(MSP430 *cpu, MCU::PinManager *pinManager, const QString &file, PinList &pins, std::map<int, QChar> &sides) {
	pins.clear();
	int pin_size = 10;
	int width = 48;
	int height = 48;
	int errorLine, errorColumn;
	QString errorMsg;

	QFile modelFile(file);
	QDomDocument document;
	if (!document.setContent(&modelFile, &errorMsg, &errorLine, &errorColumn))
	{
			QString error("Syntax error line %1, column %2:\n%3");
			error = error
					.arg(errorLine)
					.arg(errorColumn)
					.arg(errorMsg);
			qDebug() << error;
			return false;
	}

	QDomElement rootElement = document.firstChild().toElement();
	QDomNode package = rootElement.firstChild();
	if (package.nodeName() != "package") {
		return false;
	}

	// Compute width and height according to number of pins on each side
	for(QDomNode node = package.firstChild(); !node.isNull(); node = node.nextSibling()) {
		QDomElement element = node.toElement();
		QChar side = element.nodeName()[0];
		for(QDomNode pin = element.firstChild(); !pin.isNull(); pin = pin.nextSibling()) {
			if (side == 'd') {
				width += pin_size + 2;
			}
			else if (side == 'l') {
				height += pin_size + 2;
			}
		}
	}

	for(QDomNode node = package.firstChild(); !node.isNull(); node = node.nextSibling()) {
		QDomElement element = node.toElement();
		QChar side = element.nodeName()[0];

		int x = 0;
		int y = 0;
		if (side == 'l') {
			x = 0;
			y = 25;
		}
		else if (side == 'r') {
			x = width - pin_size;
			y = height - pin_size - 24;
		}
		else if (side == 'd') {
			x = 25;
			y = height - pin_size;
		}
		else if (side == 'u') {
			x = width - pin_size - 24;
			y = 0;
		}

		for(QDomNode pin = element.firstChild(); !pin.isNull(); pin = pin.nextSibling()) {
			int id = pin.toElement().attribute("id").toInt() - 1;
			sides[id] = side;
			QString n;
			MCU::PinType type = MCU::UNKNOWN;
			int subtype = -1;

			for(QDomNode name = pin.firstChild(); !name.isNull(); name = name.nextSibling()) {
				QString n_ = name.toElement().text();
				n += n_ + "/";
				setPinType(n_, type, subtype);
			}

			pins.push_back(Pin(QRect(x, y, pin_size, pin_size), n, 0));
			MCU::PinMultiplexer *mpx = pinManager->addPin(type, subtype);
			if (mpx) {
				for(QDomNode name = pin.firstChild(); !name.isNull(); name = name.nextSibling()) {
					MCU::PinMultiplexer::Condition c;
					if (name.toElement().hasAttribute("sel")) {
						c["sel"] = name.toElement().attribute("sel").toInt();
					}
					if (name.toElement().hasAttribute("dir")) {
						c["dir"] = name.toElement().attribute("dir").toInt();
					}
					QString n_ = name.toElement().text();
					if (n_.startsWith("P")) {
						mpx->addMultiplexing(c, "GP");
					}
					else {
						mpx->addMultiplexing(c, n_.toStdString());
					}
				}
			}

			if (side == 'd') {
				x += pin_size + 2;
			}
			else if (side == 'l') {
				y += pin_size + 2;
			}
			else if (side == 'r') {
				y -= pin_size + 2;
			}
			else if (side == 'u') {
				x -= pin_size + 2;
			}
		}
	}

	cpu->resize(width, height);
	return true;
}

}
