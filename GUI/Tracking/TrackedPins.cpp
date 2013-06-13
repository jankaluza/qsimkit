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

#include "TrackedPins.h"
#include "Table.h"

#include "ui/QSimKit.h"
#include "ui/Screen.h"
#include "ui/ScreenObject.h"
#include "Peripherals/SimulationObject.h"

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

TrackedPins::TrackedPins(QSimKit *simkit, QWidget *parent) :
QDockWidget(parent), m_simkit(simkit) {
	setupUi(this);
	setMinimumHeight(100);

	connect(m_simkit, SIGNAL(onSimulationStep(double)), this, SLOT(handleSimulationStep(double)));
	connect(m_simkit, SIGNAL(onSimulationStarted(bool)), this, SLOT(handleSimulationStarted(bool)));
	connect(plotHeader, SIGNAL(onPinChanged(int, int)), this, SLOT(handlePinChanged(int, int)));
	connect(plotHeader, SIGNAL(onShowTable()), this, SLOT(showTable()));
	connect(m_simkit->getScreen(), SIGNAL(onPinTracked(QObject *, int)), this, SLOT(handlePinTracked(QObject *, int)));

	setTitleBarWidget(plotHeader);
}

void TrackedPins::showTable() {
	Table dialog(this);

	for (int i = 0; i < m_history.size(); ++i) {
		dialog.addPinHistory(m_history[i]);
	}

	dialog.populate();
	dialog.exec();
}

void TrackedPins::handlePinTracked(QObject *obj, int pin) {
	ScreenObject *object = static_cast<ScreenObject *>(obj);
	plotHeader->addPin(object->getPins()[pin].name);
}

void TrackedPins::handlePinChanged(int color, int id) {
	if (m_history.size() <= id || id < 0) {
		if (id == -1) {
			if (color == 0) {
				plot->showPinHistory0(0);
			}
			else {
				plot->showPinHistory1(0);
			}
		}
		return;
	}

	if (color == 0) {
		plot->showPinHistory0(m_history[id]);
	}
	else {
		plot->showPinHistory1(m_history[id]);
	}
	plot->repaint();
}

void TrackedPins::handleSimulationStarted(bool wasPaused) {
	m_history.clear();
	plot->clear();
	plotHeader->clear();

	std::map<ScreenObject *, SimulationObjectWrapper *> &wrappers = m_simkit->getScreen()->getWrappers();
	for (std::map<ScreenObject *, SimulationObjectWrapper *>::iterator it = wrappers.begin(); it != wrappers.end(); ++it) {
		QList<PinHistory *> &history = (*it).second->getPinHistory();
		for (int i = 0; i < history.size(); ++i) {
			if (history[i]) {
				m_history.append(history[i]);
				plotHeader->addPin((*it).first->getPins()[i].name);
			}
		}
	}

	plot->repaint();
}

void TrackedPins::handleSimulationStep(double t) {
	plot->setMaximumX(t);
}


