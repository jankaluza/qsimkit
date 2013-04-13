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

#include "MemoryTracker.h"

#include "ui/QSimKit.h"
#include "Peripherals/MSP430/MSP430.h"
#include "CPU/Memory/RegisterSet.h"
#include "CPU/Memory/Register.h"
#include "CPU/Variants/Variant.h"

#include <QWidget>
#include <QTime>
#include <QMainWindow>
#include <QString>
#include <QFileDialog>
#include <QInputDialog>
#include <QFile>
#include <QProcess>
#include <QTreeWidgetItem>
#include <QTreeWidgetItemIterator>
#include <QDebug>

MemoryTracker::MemoryTracker(QSimKit *simkit) :
QDockWidget(simkit), m_cpu(0), m_simkit(simkit) {
	setupUi(this);

	connect(track, SIGNAL(clicked(bool)), this, SLOT(handleTrackClicked(bool)));
}

void MemoryTracker::handleTrackClicked(bool clicked) {
	if (comboBox->currentIndex() == -1) {
		return;
	}

	uint16_t addr = (uint16_t) comboBox->itemData(comboBox->currentIndex()).toInt();
	QTreeWidgetItem *it = new QTreeWidgetItem(view);
	it->setData(0, Qt::UserRole, addr);
	it->setText(0, comboBox->currentText());
	it->setText(1, "0");
	it->setText(2, "0x00");
	it->setText(3, "0");

	view->resizeColumnToContents(0);
	view->resizeColumnToContents(1);
	view->resizeColumnToContents(2);
	view->resizeColumnToContents(3);
}

void MemoryTracker::refresh() {
	if (!m_cpu) {
		return;
	}

	QTreeWidgetItemIterator it(view);
	while (*it) {
		QTreeWidgetItem *item = *it;
		uint16_t addr = (uint16_t) item->data(0, Qt::UserRole).toInt();
		qDebug() << addr;
		uint8_t n = m_cpu->getMemory()->getByte(addr);
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

		item->setText(1, dec);
		item->setText(2, hex);
		item->setText(3, bin);
		++it;
	}
/*
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
	}*/

	view->resizeColumnToContents(0);
	view->resizeColumnToContents(1);
	view->resizeColumnToContents(2);
	view->resizeColumnToContents(3);
}

void MemoryTracker::initComboBox() {
	comboBox->clear();

	if (m_cpu->getVariant()->getP1DIR() != 0) {
		comboBox->addItem("P1DIR", QVariant(m_cpu->getVariant()->getP1DIR()));
	}

	if (m_cpu->getVariant()->getP1IE() != 0) {
		comboBox->addItem("P1IE", QVariant(m_cpu->getVariant()->getP1IE()));
	}

	if (m_cpu->getVariant()->getP1IES() != 0) {
		comboBox->addItem("P1IES", QVariant(m_cpu->getVariant()->getP1IES()));
	}

	if (m_cpu->getVariant()->getP1IFG() != 0) {
		comboBox->addItem("P1IFG", QVariant(m_cpu->getVariant()->getP1IFG()));
	}

	if (m_cpu->getVariant()->getP1IN() != 0) {
		comboBox->addItem("P1IN", QVariant(m_cpu->getVariant()->getP1IN()));
	}

	if (m_cpu->getVariant()->getP1OUT() != 0) {
		comboBox->addItem("P1OUT", QVariant(m_cpu->getVariant()->getP1OUT()));
	}

	if (m_cpu->getVariant()->getP1REN() != 0) {
		comboBox->addItem("P1REN", QVariant(m_cpu->getVariant()->getP1REN()));
	}

	if (m_cpu->getVariant()->getP1SEL() != 0) {
		comboBox->addItem("P1SEL", QVariant(m_cpu->getVariant()->getP1SEL()));
	}

	if (m_cpu->getVariant()->getP2DIR() != 0) {
		comboBox->addItem("P2DIR", QVariant(m_cpu->getVariant()->getP2DIR()));
	}

	if (m_cpu->getVariant()->getP2IE() != 0) {
		comboBox->addItem("P2IE", QVariant(m_cpu->getVariant()->getP2IE()));
	}

	if (m_cpu->getVariant()->getP2IES() != 0) {
		comboBox->addItem("P2IES", QVariant(m_cpu->getVariant()->getP2IES()));
	}

	if (m_cpu->getVariant()->getP2IFG() != 0) {
		comboBox->addItem("P2IFG", QVariant(m_cpu->getVariant()->getP2IFG()));
	}

	if (m_cpu->getVariant()->getP2IN() != 0) {
		comboBox->addItem("P2IN", QVariant(m_cpu->getVariant()->getP2IN()));
	}

	if (m_cpu->getVariant()->getP2OUT() != 0) {
		comboBox->addItem("P2OUT", QVariant(m_cpu->getVariant()->getP2OUT()));
	}

	if (m_cpu->getVariant()->getP2REN() != 0) {
		comboBox->addItem("P2REN", QVariant(m_cpu->getVariant()->getP2REN()));
	}

	if (m_cpu->getVariant()->getP2SEL() != 0) {
		comboBox->addItem("P2SEL", QVariant(m_cpu->getVariant()->getP2SEL()));
	}

	if (m_cpu->getVariant()->getP3DIR() != 0) {
		comboBox->addItem("P3DIR", QVariant(m_cpu->getVariant()->getP3DIR()));
	}

	if (m_cpu->getVariant()->getP3IN() != 0) {
		comboBox->addItem("P3IN", QVariant(m_cpu->getVariant()->getP3IN()));
	}

	if (m_cpu->getVariant()->getP3OUT() != 0) {
		comboBox->addItem("P3OUT", QVariant(m_cpu->getVariant()->getP3OUT()));
	}

	if (m_cpu->getVariant()->getP3REN() != 0) {
		comboBox->addItem("P3REN", QVariant(m_cpu->getVariant()->getP3REN()));
	}

	if (m_cpu->getVariant()->getP3SEL() != 0) {
		comboBox->addItem("P3SEL", QVariant(m_cpu->getVariant()->getP3SEL()));
	}

	if (m_cpu->getVariant()->getP4DIR() != 0) {
		comboBox->addItem("P4DIR", QVariant(m_cpu->getVariant()->getP4DIR()));
	}

	if (m_cpu->getVariant()->getP4IN() != 0) {
		comboBox->addItem("P4IN", QVariant(m_cpu->getVariant()->getP4IN()));
	}

	if (m_cpu->getVariant()->getP4OUT() != 0) {
		comboBox->addItem("P4OUT", QVariant(m_cpu->getVariant()->getP4OUT()));
	}

	if (m_cpu->getVariant()->getP4REN() != 0) {
		comboBox->addItem("P4REN", QVariant(m_cpu->getVariant()->getP4REN()));
	}

	if (m_cpu->getVariant()->getP4SEL() != 0) {
		comboBox->addItem("P4SEL", QVariant(m_cpu->getVariant()->getP4SEL()));
	}

	if (m_cpu->getVariant()->getP5DIR() != 0) {
		comboBox->addItem("P5DIR", QVariant(m_cpu->getVariant()->getP5DIR()));
	}

	if (m_cpu->getVariant()->getP5IN() != 0) {
		comboBox->addItem("P5IN", QVariant(m_cpu->getVariant()->getP5IN()));
	}

	if (m_cpu->getVariant()->getP5OUT() != 0) {
		comboBox->addItem("P5OUT", QVariant(m_cpu->getVariant()->getP5OUT()));
	}

	if (m_cpu->getVariant()->getP5REN() != 0) {
		comboBox->addItem("P5REN", QVariant(m_cpu->getVariant()->getP5REN()));
	}

	if (m_cpu->getVariant()->getP5SEL() != 0) {
		comboBox->addItem("P5SEL", QVariant(m_cpu->getVariant()->getP5SEL()));
	}

	if (m_cpu->getVariant()->getP6DIR() != 0) {
		comboBox->addItem("P6DIR", QVariant(m_cpu->getVariant()->getP6DIR()));
	}

	if (m_cpu->getVariant()->getP6IN() != 0) {
		comboBox->addItem("P6IN", QVariant(m_cpu->getVariant()->getP6IN()));
	}

	if (m_cpu->getVariant()->getP6OUT() != 0) {
		comboBox->addItem("P6OUT", QVariant(m_cpu->getVariant()->getP6OUT()));
	}

	if (m_cpu->getVariant()->getP6REN() != 0) {
		comboBox->addItem("P6REN", QVariant(m_cpu->getVariant()->getP6REN()));
	}

	if (m_cpu->getVariant()->getP6SEL() != 0) {
		comboBox->addItem("P6SEL", QVariant(m_cpu->getVariant()->getP6SEL()));
	}

	if (m_cpu->getVariant()->getP7DIR() != 0) {
		comboBox->addItem("P7DIR", QVariant(m_cpu->getVariant()->getP7DIR()));
	}

	if (m_cpu->getVariant()->getP7IN() != 0) {
		comboBox->addItem("P7IN", QVariant(m_cpu->getVariant()->getP7IN()));
	}

	if (m_cpu->getVariant()->getP7OUT() != 0) {
		comboBox->addItem("P7OUT", QVariant(m_cpu->getVariant()->getP7OUT()));
	}

	if (m_cpu->getVariant()->getP7REN() != 0) {
		comboBox->addItem("P7REN", QVariant(m_cpu->getVariant()->getP7REN()));
	}

	if (m_cpu->getVariant()->getP7SEL() != 0) {
		comboBox->addItem("P7SEL", QVariant(m_cpu->getVariant()->getP7SEL()));
	}

	if (m_cpu->getVariant()->getP8DIR() != 0) {
		comboBox->addItem("P8DIR", QVariant(m_cpu->getVariant()->getP8DIR()));
	}

	if (m_cpu->getVariant()->getP8IN() != 0) {
		comboBox->addItem("P8IN", QVariant(m_cpu->getVariant()->getP8IN()));
	}

	if (m_cpu->getVariant()->getP8OUT() != 0) {
		comboBox->addItem("P8OUT", QVariant(m_cpu->getVariant()->getP8OUT()));
	}

	if (m_cpu->getVariant()->getP8REN() != 0) {
		comboBox->addItem("P8REN", QVariant(m_cpu->getVariant()->getP8REN()));
	}

	if (m_cpu->getVariant()->getP8SEL() != 0) {
		comboBox->addItem("P8SEL", QVariant(m_cpu->getVariant()->getP8SEL()));
	}
}

void MemoryTracker::setCPU(MSP430 *cpu) {
	m_cpu = cpu;
	if (!m_cpu)
		return;

	initComboBox();

	refresh();
}
