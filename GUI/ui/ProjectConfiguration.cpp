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

#include "CPU/Variants/Variant.h"
#include "CPU/Variants/VariantManager.h"

#include "Peripherals/Peripheral.h"
#include "Peripherals/PeripheralManager.h"

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

ProjectConfiguration::ProjectConfiguration(QWidget *parent, MSP430 *cpu) :
QDialog(parent) {
	setupUi(this);

	std::vector<_msp430_variant *> variants = getVariants();
	for (std::vector<_msp430_variant *>::iterator it = variants.begin(); it != variants.end(); it++) {
		MSP430Variants->addItem((*it)->name);
		if (cpu && QString((*it)->name) == QString(cpu->getVariant()->getName())) {
			MSP430Variants->setCurrentRow(MSP430Variants->count() - 1);
		}
	}

	connect(MSP430Variants, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(handleCurrentItemChanged(QListWidgetItem *, QListWidgetItem *)));

	if (cpu) {
		MSP430Variants->setEnabled(false);
		handleCurrentItemChanged(0, 0);
	}
}

MSP430 *ProjectConfiguration::getMSP430() {
	Variant *v = getVariant(MSP430Variants->currentItem()->text().toStdString().c_str());
	MSP430 *cpu = new MSP430(v, QString("Packages/") + MSP430Variants->currentItem()->text() + ".xml");
	return cpu;
}


void ProjectConfiguration::handleCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *) {
	preview->setObject(getMSP430());
}


