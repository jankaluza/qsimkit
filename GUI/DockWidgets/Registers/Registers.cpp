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

#include "Registers.h"

#include "ui/QSimKit.h"
#include "Peripherals/MSP430/MSP430.h"
#include "CPU/Memory/RegisterSet.h"
#include "CPU/Memory/Register.h"

#include <QWidget>
#include <QTime>
#include <QMainWindow>
#include <QString>
#include <QFileDialog>
#include <QInputDialog>
#include <QFile>
#include <QProcess>
#include <QTreeWidgetItem>
#include <QDebug>

Registers::Registers(QSimKit *simkit) :
QDockWidget(simkit), m_cpu(0), m_simkit(simkit) {
	setupUi(this);

	for (int r = 0; r < 16; r++) {
		QTreeWidgetItem *item = new QTreeWidgetItem(view);
		if (r == 0)
			item->setText(0, QString("PC"));
		else if (r == 1)
			item->setText(0, QString("SP"));
		else if (r == 2)
			item->setText(0, QString("SR"));
		else
			item->setText(0, QString("R") + QString::number(r));
		item->setText(1, "0");
		item->setText(2, "0");
		item->setText(3, "0");
		item->setBackground(0, view->palette().window());
	}

	view->resizeColumnToContents(0);
}

void Registers::refresh() {
	if (!m_cpu) {
		return;
	}

	for (int r = 0; r < 16; r++) {
		int16_t n = m_cpu->getRegisterSet()->get(r)->getBigEndian();
		QString dec;
		QString hex;
		QString bin;
		if (r == 0) {
			dec = QString::number((uint16_t) n);
			hex = QString("0x%1").arg((uint16_t) n, 0, 16);
			bin = QString("%1").arg((uint16_t) n, 0, 2);
		}
		else {
			dec = QString::number(n);
			hex = QString("0x%1").arg(n, 0, 16);
			bin = QString("%1").arg(n, 0, 2);
		}

		QTreeWidgetItem *it = view->topLevelItem(r);
		it->setText(1, dec);
		it->setText(2, hex);
		it->setText(3, bin);
	}

	view->resizeColumnToContents(1);
	view->resizeColumnToContents(2);
	view->resizeColumnToContents(3);
}

void Registers::setCPU(MSP430 *cpu) {
	m_cpu = cpu;
	refresh();
}
