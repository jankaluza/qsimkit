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

#include "QSimKit.h"

#include "CPU/Variants/Variant.h"
#include "CPU/Variants/VariantManager.h"
#include "Peripherals/MSP430/MSP430.h"

#include <QWidget>
#include <QTime>
#include <QMainWindow>
#include <QString>
#include <QFileDialog>
#include <QInputDialog>
#include <QFile>
#include <QIcon>
#include <QIODevice>
#include <QDebug>

QSimKit::QSimKit(QWidget *parent) : QMainWindow(parent), m_variant(0),
m_dig(0), m_sim(0) {
	setupUi(this);

	connect(actionLoad_A43, SIGNAL(triggered()), this, SLOT(loadA43()) );

	QAction *action = toolbar->addAction(QIcon("./icons/22x22/actions/media-playback-start.png"), tr("Start &simulation"));
	connect(action, SIGNAL(triggered()), this, SLOT(startSimulation()));

	action = toolbar->addAction(QIcon("./icons/22x22/actions/media-playback-pause.png"), tr("P&ause simulation"));
	action->setCheckable(true);
	connect(action, SIGNAL(triggered(bool)), this, SLOT(pauseSimulation(bool)));

	action = toolbar->addAction(QIcon("./icons/22x22/actions/media-playback-stop.png"), tr("Sto&p simulation"));
	connect(action, SIGNAL(triggered()), this, SLOT(stopSimulation()));

	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(simulationStep()));
}

void QSimKit::setVariant(const QString &variant) {
	m_variant = getVariant(variant.toStdString().c_str());
}

void QSimKit::simulationStep() {
	statusbar->showMessage(QString::number(m_sim->nextEventTime()));
// 	QTime perf;
// 	perf.start();
	for (int i = 0; i < 20000; ++i) {
		m_sim->execNextEvent();
	}
// 	qDebug() << perf.elapsed();
}

void QSimKit::resetSimulation() {
	m_timer->stop();

	delete m_dig;
	delete m_sim;

	m_dig = new adevs::Digraph<SimulationEvent *>();
	screen->prepareSimulation(m_dig);
	m_sim = new adevs::Simulator<adevs::PortValue<SimulationEvent *> >(m_dig);
}

void QSimKit::startSimulation() {
	resetSimulation();
	m_timer->start(100);
}

void QSimKit::stopSimulation() {
	resetSimulation();
}

void QSimKit::pauseSimulation(bool checked) {
	if (checked) {
		m_timer->stop();
	}
	else {
		m_timer->start(100);
	}
}

bool QSimKit::loadA43File(const QString &f) {
	if (!m_variant) {
		chooseVariant();
	}

	QFile file(f);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return false;

	MSP430 *cpu = new MSP430(m_variant);
	cpu->loadXML("Packages/msp430x241x.xml");
	screen->setCPU(cpu);

	return screen->getCPU()->loadA43(file.readAll().data());
}

void QSimKit::loadA43() {
	QString filename = QFileDialog::getOpenFileName(this);
	if (filename.isEmpty()) {
		return;
	}

	loadA43File(filename);
}

void QSimKit::chooseVariant() {
	QStringList items;
	std::vector<_msp430_variant *> variants = getVariants();
	for (std::vector<_msp430_variant *>::iterator it = variants.begin(); it != variants.end(); it++) {
		items.append((*it)->name);
	}

	bool ok = false;
	QString item = QInputDialog::getItem(this, "Choose MSP430 variant", "MSP430 Variant:", items, 0, false, &ok);
	if (ok && !item.isEmpty()) {
		setVariant(item);
	}
}
