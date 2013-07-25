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

#include "ProjectConfiguration.h"

#include "Peripherals/Peripheral.h"
#include "Peripherals/PeripheralManager.h"
#include "MCU/MCUManager.h"
#include "MCU/MCU.h"

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

ProjectConfiguration::ProjectConfiguration(QWidget *parent, MCUManager *manager, MCU *mcu) :
QDialog(parent), m_manager(manager) {
	setupUi(this);

	MCU *currentMCU = mcu;

	foreach (const MCUInfo &m, m_manager->getMCUs()) {
		family->addItem(m.getName(), m.getLibrary());
		if (!currentMCU) {
			currentMCU = m_manager->getMCU(m.getLibrary()).create("");
		}
	}

	if (currentMCU) {
		QStringList variants = currentMCU->getVariants();
		for (int i = 0; i < variants.size(); ++i) {
			MSP430Variants->addItem(variants[i]);
			if (variants[i] == currentMCU->getVariant()) {
				MSP430Variants->setCurrentRow(MSP430Variants->count() - 1);
			}
		}
		if (currentMCU != mcu) {
			delete currentMCU;
		}
	}

	connect(MSP430Variants, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(handleCurrentItemChanged(QListWidgetItem *, QListWidgetItem *)));

	if (mcu) {
		MSP430Variants->setEnabled(false);
		handleCurrentItemChanged(0, 0);
	}
}

MCU *ProjectConfiguration::getMCU() {
	if (MSP430Variants->currentItem() == 0) {
		return 0;
	}
	return m_manager->getMCU(family->itemData(family->currentIndex()).toString()).create(MSP430Variants->currentItem()->text());
}


void ProjectConfiguration::handleCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *) {
	MCU *mcu = getMCU();
	if (mcu) {
		preview->setObject(mcu);
	}
}


