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
#include <QPushButton>

PlotHeader::PlotHeader(QWidget *parent) : QWidget(parent) {
	m_index = 0;
	m_layout = new QHBoxLayout(this);
	m_layout->setContentsMargins(0, 0, 0, 0);
	setLayout(m_layout);

	m_label = new QLabel(" Tracked pins:");
	m_layout->addWidget(m_label);

	m_redPin = new QComboBox();
	m_redPin->setMinimumWidth(130);
	m_redPin->addItem("None");
	m_redPin->setItemData(0, -1);
	m_layout->addWidget(m_redPin);

	m_greenPin = new QComboBox();
	m_greenPin->addItem("None");
	m_greenPin->setMinimumWidth(130);
	m_greenPin->setItemData(0, -1);
	m_layout->addWidget(m_greenPin);

	QPushButton *button = new QPushButton("Show in table");
	connect(button, SIGNAL(clicked(bool)), this, SLOT(showTable()));
	m_layout->addWidget(button);

	m_layout->addStretch();

	connect(m_redPin, SIGNAL(currentIndexChanged(int)), this, SLOT(handleRedIndexChanged(int)));
	connect(m_greenPin, SIGNAL(currentIndexChanged(int)), this, SLOT(handleGreenIndexChanged(int)));
}

PlotHeader::~PlotHeader() {

}

void PlotHeader::showTable(bool checked) {
	onShowTable();
}

void PlotHeader::handleRedIndexChanged(int id) {
	onPinChanged(0, m_redPin->itemData(id).toInt());
}

void PlotHeader::handleGreenIndexChanged(int id) {
	onPinChanged(1, m_greenPin->itemData(id).toInt());
}

void PlotHeader::clear() {
	m_index = 0;
// 	m_redPin->clear();
// 	m_greenPin->clear();
// 	m_redPin->addItem("None");
// 	m_greenPin->addItem("None");
}

void PlotHeader::addPin(const QString &label) {
	int id = m_redPin->findText(label);
	if (id != -1) {
		m_redPin->setItemData(id, m_index);
		m_greenPin->setItemData(id, m_index);
		handleRedIndexChanged(m_redPin->currentIndex());
		handleGreenIndexChanged(m_greenPin->currentIndex());
		m_index++;
		return;
	}

	m_redPin->addItem(label);
	m_greenPin->addItem(label);
	m_redPin->setItemData(m_redPin->count() - 1, m_index);
	m_greenPin->setItemData(m_greenPin->count() - 1, m_index);
	m_index++;

	if (m_redPin->currentIndex() == 0) {
		m_redPin->setCurrentIndex(m_redPin->count() - 1);
	}
	else if (m_greenPin->currentIndex() == 0) {
		m_greenPin->setCurrentIndex(m_greenPin->count() - 1);
	}
}
