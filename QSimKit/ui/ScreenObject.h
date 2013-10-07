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

#pragma once

#include <QPainter>
#include <map>
#include <QTextStream>
#include <QDomElement>
// #include "Peripherals/SimulationObject.h"


class Pin {
	public:
		Pin() {}
		Pin(const QRect &rect, const QString &name, double value) :
			rect(rect), name(name), value(value) { }

		QRect rect;
		QString name;
		double value;
};

typedef std::vector<Pin> PinList;

class ScreenObject : public QObject
{
	Q_OBJECT

	public:
		ScreenObject();
		virtual ~ScreenObject() {}

		int x() { return m_x; }
		int y() { return m_y; }
		int width() { return m_width; }
		int height() { return m_height; }
		const QString &type() { return m_type; }
		const QString &interface() { return m_interface; }
		const QString &name() { return m_name; }

		void setX(int x);
		void setY(int y);
		void setWidth(int width) { m_width = width; }
		void setHeight(int height) { m_height = height; }
		void setName(const QString &name) { m_name = name; }

		void resize(int w, int h) { m_width = w; m_height = h; }

		virtual void save(QTextStream &stream);
		virtual void load(QDomElement &object, QString &error) {}

		virtual void paint(QWidget *screen) = 0;

		virtual void objectMoved(int x, int y) {}

		virtual PinList &getPins() = 0;

		virtual const QStringList &getOptions() = 0;

		virtual void executeOption(int option) = 0;

		virtual bool clicked(const QPoint &p) { return false; }

		void update();

	signals:
		void onUpdated();

	private:
		void movePins(int x, int y);

	protected:
		int m_x;
		int m_y;
		int m_width;
		int m_height;
		int m_id;
		QString m_name;
		QString m_type;
		QString m_interface;
		bool m_updated;
	
		friend class Screen;
		friend class MCUInfo;
		friend class PeripheralInfo;

};

