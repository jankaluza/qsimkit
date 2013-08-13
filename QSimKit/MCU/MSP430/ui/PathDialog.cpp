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

#include "PathDialog.h"

#include <QWidget>
#include <QString>
#include <QFileDialog>
#include <QFile>
#include <QIcon>
#include <QDebug>

PathDialog::PathDialog(const QString &objcopy_, const QString &objdump_, QWidget *parent) :
QDialog(parent) {
	setupUi(this);

	objdump->setText(objdump_);
	objcopy->setText(objcopy_);

	connect(objdumpBrowse, SIGNAL(clicked(bool)), this, SLOT(browseObjdump(bool)) );
	connect(objcopyBrowse, SIGNAL(clicked(bool)), this, SLOT(browseObjcopy(bool)) );
}

void PathDialog::browse(QLineEdit *line) {
	QString filename = QFileDialog::getOpenFileName(this);
	if (filename.isEmpty()) {
		return;
	}

	line->setText(filename);
}

void PathDialog::browseObjdump(bool checked) {
	browse(objdump);
}

void PathDialog::browseObjcopy(bool checked) {
	browse(objcopy);
}

QString PathDialog::getObjdump() {
	return objdump->text();
}

QString PathDialog::getObjcopy() {
	return objcopy->text();
}



