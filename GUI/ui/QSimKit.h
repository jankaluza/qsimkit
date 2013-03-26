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

#include <QMainWindow>
#include <QString>
#include <QTimer>

#include "ui_QSimKit.h"

#include "adevs.h"

class Variant;
class CPU;
class SimulationEvent;

class QSimKit : public QMainWindow, public Ui::QSimKit
{
	Q_OBJECT

	public:
		QSimKit(QWidget *parent = 0);

		void setVariant(const QString &variant);
		bool loadA43File(const QString &file);

	public slots:
		void loadA43();
		void chooseVariant();
		void simulationStep();

		void startSimulation();
		void stopSimulation();
		void pauseSimulation(bool pause);
		void resetSimulation();

	private:
		Variant *m_variant;
		adevs::Digraph<SimulationEvent *> *m_dig;
		adevs::Simulator<adevs::PortValue<SimulationEvent *> > *m_sim;
		QTimer *m_timer;
};

