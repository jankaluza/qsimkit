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

	connect(m_simkit, SIGNAL(onSimulationStep(double)), this, SLOT(handleSimulationStep(double)));
	connect(m_simkit, SIGNAL(onSimulationStarted(bool)), this, SLOT(handleSimulationStarted(bool)));
	connect(plot, SIGNAL(onPointToInstruction(int)), m_simkit, SLOT(pointToInstruction(int)));
	connect(m_simkit->getScreen(), SIGNAL(onPinTracked(QObject *, int)), this, SLOT(handlePinTracked(QObject *, int)));
	connect(m_simkit->getScreen(), SIGNAL(onPinUntracked(QObject *, int)), this, SLOT(handlePinUntracked(QObject *, int)));
}

void TrackedPins::showTable() {
// 	Table dialog(this);

// 	for (int i = 0; i < m_history.size(); ++i) {
// 		dialog.addPinHistory(m_history[i]);
// 	}

// 	dialog.populate();
// 	dialog.exec();
}

void TrackedPins::refreshPins() {
	plot->clear();
	plot->resetView();

	std::map<ScreenObject *, SimulationObjectWrapper *> &wrappers = m_simkit->getScreen()->getWrappers();
	std::map<ScreenObject *, QList<int> > &pins = m_simkit->getScreen()->getTrackedPins();

	for (std::map<ScreenObject *, QList<int> >::iterator it = pins.begin(); it != pins.end(); ++it) {
		foreach(int pin, it->second) {
			PinHistory *history = 0;
			std::map<ScreenObject *, SimulationObjectWrapper *>::iterator itw = wrappers.find(it->first);
			if (itw != wrappers.end()) {
				QVector<PinHistory *> &h = (*itw).second->getPinHistory();
				if (pin < h.size()) {
					history = h[pin];
				}
			}

			plot->addPinHistory((*it).first->getPins()[pin].name, history);
		}
	}

	plot->repaint();
}

void TrackedPins::handlePinTracked(QObject *obj, int pin) {
	refreshPins();
}

void TrackedPins::handlePinUntracked(QObject *obj, int pin) {
	refreshPins();
}

void TrackedPins::handleSimulationStarted(bool wasPaused) {
	refreshPins();
}

void TrackedPins::handleSimulationStep(double t) {
	plot->setMaximumX(t);
}


