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

#include <QDialog>
#include <QString>
#include <QTimer>
#include <QLineEdit>

#include "ui_PathDialog.h"

class PathDialog : public QDialog, public Ui::PathDialog
{
	Q_OBJECT

	public:
		PathDialog(const QString &objcopy, const QString &objdump, QWidget *parent = 0);

		QString getObjdump();

		QString getObjcopy();

	private slots:
		void browse(QLineEdit *line);
		void browseObjdump(bool checked = false);
		void browseObjcopy(bool checked = false);

};

