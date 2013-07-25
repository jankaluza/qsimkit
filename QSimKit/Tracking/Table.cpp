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

#include "Table.h"

#include "ui/QSimKit.h"
#include "ui/Screen.h"
#include "ui/ScreenObject.h"
#include "Peripherals/SimulationObject.h"
#include "PinHistory.h"

#include <QWidget>
#include <QTime>
#include <QMainWindow>
#include <QString>
#include <QFileDialog>
#include <QInputDialog>
#include <QFile>
#include <QIcon>
#include <QTreeWidgetItem>
#include <QDebug>

Table::Table(QWidget *parent) : QDialog(parent) {
	setupUi(this);
}

void Table::addPinHistory(PinHistory *pin) {
	if (!pin)
		return;

	m_pins.append(pin);
}

void Table::populate() {
	QVector<QLinkedList<PinEvent>::iterator> indexes;
	QVector<QLinkedList<PinEvent>::iterator> ends;

	for (int i = 0; i < m_pins.size(); ++i) {
		indexes << m_pins[i]->getEvents().begin();
		ends << m_pins[i]->getEvents().end();
	}

	table->setColumnCount(m_pins.size());

	int c = 0;
	while (true) {
		double lowest = DBL_MAX;
		QLinkedList<PinEvent>::iterator *it;
		for (int i = 0; i < m_pins.size(); ++i) {
			if (indexes[i] != ends[i] && indexes[i]->t < lowest) {
				lowest = indexes[i]->t;
				it = &indexes[i];
			}
		}

		if (lowest == DBL_MAX) {
			break;
		}

		QTableWidgetItem *item = new QTableWidgetItem(QString::number(lowest));
		table->setVerticalHeaderItem(c, item);
		for (int i = 0; i < m_pins.size(); ++i) {
			item = new QTableWidgetItem(QString::number(indexes[i]->v));
			table->setItem(c, i, item);
		}
		c++;
		

		(*it)++;
		table->setRowCount(c + 1);
	}
}


