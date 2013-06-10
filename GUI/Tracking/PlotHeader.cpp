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

	m_redPin = new QComboBox();
	m_redPin->setMinimumWidth(130);
	m_redPin->addItem("None");
	m_layout->addWidget(m_redPin);

	m_greenPin = new QComboBox();
	m_greenPin->addItem("None");
	m_greenPin->setMinimumWidth(130);
	m_layout->addWidget(m_greenPin);

	m_layout->addStretch();

	connect(m_redPin, SIGNAL(currentIndexChanged(int)), this, SLOT(handleRedIndexChanged(int)));
	connect(m_greenPin, SIGNAL(currentIndexChanged(int)), this, SLOT(handleGreenIndexChanged(int)));
}

PlotHeader::~PlotHeader() {

}

void PlotHeader::handleRedIndexChanged(int id) {
	onPinChanged(0, id - 1);
}

void PlotHeader::handleGreenIndexChanged(int id) {
	onPinChanged(1, id - 1);
}

void PlotHeader::clear() {
	m_redPin->clear();
	m_greenPin->clear();
	m_redPin->addItem("None");
	m_greenPin->addItem("None");
}

void PlotHeader::addPin(const QString &label) {
	m_redPin->addItem(label);
	m_greenPin->addItem(label);
}
