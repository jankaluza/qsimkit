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

#include "ScreenObjectPreview.h"
#include "ScreenObject.h"


#include <QWidget>
#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QToolTip>
#include <QString>
#include <QApplication>
#include <QFileDialog>
#include <QInputDialog>
#include <QFile>
#include <QCursor>
#include <QIODevice>
#include <QMouseEvent>
#include <QDebug>


ScreenObjectPreview::ScreenObjectPreview(QWidget *parent) : QWidget(parent), m_object(0) {
}

ScreenObjectPreview::~ScreenObjectPreview() {
	delete m_object;
}

void ScreenObjectPreview::setObject(ScreenObject *object) {
	delete m_object;
	m_object = object;

	object->setX(width()/2 - object->height()/2);
	object->setY(height()/2 - object->width()/2);
	repaint();
}

void ScreenObjectPreview::paintEvent(QPaintEvent *e) {
	QPainter p;
	p.begin(this);
	p.fillRect(QRect(0, 0, width(), height()), QBrush(QColor(255, 255, 255)));
	p.setPen(QPen(QColor(245, 245, 245), 1, Qt::SolidLine));
	for (int i = 0; i < 1500; i += 12) {
		p.drawLine(i, 0, i, 1500);
	}

	for (int i = 0; i < 1500; i += 12) {
		p.drawLine(0, i, 1500, i);
	}

	p.end();

	if (m_object) {
		m_object->paint(this);
	}
}
