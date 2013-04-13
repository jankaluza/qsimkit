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

#include "Stack.h"

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

Stack::Stack(QSimKit *simkit) :
QDockWidget(simkit), m_cpu(0), m_simkit(simkit), m_defaultSP(0) {
	setupUi(this);

}

void Stack::refresh() {
	if (!m_cpu) {
		return;
	}

	view->clear();


	uint16_t sp = m_cpu->getRegisterSet()->get(1)->getBigEndian();
	qDebug() << sp << m_defaultSP;

	for (; sp < m_defaultSP; sp += 2) {
		int16_t n = m_cpu->getMemory()->getBigEndian(sp);
		QString dec;
		QString hex;
		QString bin;
// 		if (r == 0) {
// 			dec = QString::number((uint16_t) n);
// 			hex = QString("0x%1").arg((uint16_t) n, 0, 16);
// 			bin = QString("%1").arg((uint16_t) n, 0, 2);
// 		}
// 		else {
			dec = QString::number(n);
			hex = QString("0x%1").arg(n, 0, 16);
			bin = QString("%1").arg(n, 0, 2);
// 		}

		QTreeWidgetItem *it = new QTreeWidgetItem(view);
		it->setText(0, QString::number(m_defaultSP - sp));
		it->setText(1, dec);
		it->setText(2, hex);
		it->setText(3, bin);
	}

	view->resizeColumnToContents(0);
	view->resizeColumnToContents(1);
	view->resizeColumnToContents(2);
	view->resizeColumnToContents(3);
}

bool Stack::handleRegisterChanged(Register *reg, int id, uint16_t value) {
	m_defaultSP = reg->getBigEndian();
	return false;
}

void Stack::setCPU(MSP430 *cpu) {
	m_cpu = cpu;
	if (m_cpu) {
		m_cpu->getRegisterSet()->get(1)->addWatcher(this);
		refresh();
	}
}
