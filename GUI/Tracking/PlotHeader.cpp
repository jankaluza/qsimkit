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

#include "PlotHeader.h"

#include <QWidget>
#include <QPainter>
#include <QAction>
#include <QMenu>
#include <QToolTip>
#include <QString>
#include <QMouseEvent>
#include <QDebug>

PlotHeader::PlotHeader(QWidget *parent) : QWidget(parent) {
	m_layout = new QHBoxLayout(this);
	m_layout->setContentsMargins(0, 0, 0, 0);
	setLayout(m_layout);

	m_label = new QLabel(" Tracked pins:");

	m_layout->addWidget(m_label);
	m_layout->addStretch();
}

PlotHeader::~PlotHeader() {

}

void PlotHeader::clear() {
	while (!m_pins.isEmpty()) {
		m_layout->removeWidget(m_pins[0]);
		delete m_pins[0];
		m_pins.removeFirst();
	}
}

void PlotHeader::addPin(const QString &label) {
	QToolButton *button = new QToolButton(this);
	button->setText(label);
	button->setCheckable(true);
	
	m_layout->insertWidget(m_layout->count() - 1, button);
	m_pins.append(button);
}