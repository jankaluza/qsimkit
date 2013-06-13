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

#include <QDockWidget>
#include <QString>
#include <QTimer>

#include "ui_TrackedPins.h"

class QSimKit;
class PinHistory;

class TrackedPins : public QDockWidget, public Ui::TrackedPins
{
	Q_OBJECT

	public:
		TrackedPins(QSimKit *simkit, QWidget *parent = 0);

	private slots:
		void handleSimulationStarted(bool wasPaused);
		void handleSimulationStep(double t);
		void handlePinChanged(int color, int id);
		void handlePinTracked(QObject *obj, int pin);
		void showTable();

	private:
		QSimKit *m_simkit;
		QList<PinHistory *> m_history;
};

