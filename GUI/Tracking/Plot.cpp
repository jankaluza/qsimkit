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

#include "Plot.h"
#include "PinHistory.h"

#include <QWidget>
#include <QPainter>
#include <QAction>
#include <QMenu>
#include <QToolTip>
#include <QString>
#include <QMouseEvent>
#include <QDebug>

Plot::Plot(QWidget *parent) : QWidget(parent), m_maxX(1.0), m_maxY(3.3), m_pinHistory(0),
	m_fromX(-1), m_toX(-1)  {
	setMouseTracking(true);

	// Test:
// 	m_pinHistory = new PinHistory();
// 	m_pinHistory->addEvent(0.5, 1.0);
// 	m_pinHistory->addEvent(0.6, 1.5);
// 	m_pinHistory->addEvent(0.65, 0.3);
}

void Plot::setMaximumX(double x) {
	m_maxX = x;
	repaint();
}

void Plot::setMaximumY(double y) {
	m_maxY = y;
	repaint();
}

void Plot::showPinHistory(PinHistory *pinHistory) {
	m_pinHistory = pinHistory;
	repaint();
}

void Plot::paintEvent(QPaintEvent *e) {
	QPainter p;
	p.begin(this);
	p.fillRect(QRect(0, 0, width(), height()), QBrush(QColor(255, 255, 255)));
	p.setPen(QPen(QColor(0, 0, 0), 1, Qt::SolidLine));

	// Draw axis
	p.drawLine(25, height() - 20, 25, 10);
	p.drawLine(25, height() - 20, width() - 10, height() - 20);

	// Draw axis descriptions
	p.drawText(21, height() - 20, 10, 20, Qt::AlignCenter, "0");
	p.drawText(0, 0, 20, 20, Qt::AlignCenter, QString::number(m_maxY));

	double x = (double(m_pos.x() - 25) / (width() - 35)) * m_maxX;
	double y = m_maxY - (double(m_pos.y() - 10) / (height() - 30)) * m_maxY;

	if (m_fromX != -1 && m_toX != -1) {
		p.fillRect(m_fromX, 10, m_toX - m_fromX, height() - 30, palette().highlight());
	}
	else if (m_fromX != -1 && m_toX == -1) {
		p.fillRect(m_fromX, 10, m_pos.x() - m_fromX, height() - 30, palette().highlight());
	}

	p.setPen(QPen(QColor(255, 0, 0), 2, Qt::SolidLine));
	if (m_pinHistory) {
		double toX = 0;
		double toY = 0;
		double fromX = 25;
		double fromY = height() - 20;
		bool draw = false;
		double previousV = 0;
		QLinkedList<PinEvent>::iterator it = m_pinHistory->getEvents().begin();
		for (; it != m_pinHistory->getEvents().end(); ++it) {
			if ((*it).t > m_maxX) {
				break;
			}
			toX = ((*it).t / m_maxX) * (width() - 35) + 25;
			toY = height() - 20 - ((*it).v / m_maxY) * (height() - 30);
			p.drawLine(fromX, fromY, toX, fromY);
			p.drawLine(toX, fromY, toX, toY);
			
			if (m_pos.x() > toX - 5 && m_pos.x() < toX + 5) {
				p.drawRect(toX - 4, toY - 4, 8, 8);
				QString label = QString("t=") + QString::number((*it).t) + ", v=" + QString::number((*it).v);
				p.drawText(toX - 150, height() - 20, 300, 20, Qt::AlignCenter, label);
				draw = true;
			}
			else if (!draw && m_pos.x() > fromX && m_pos.x() < toX) {
				p.drawRect(m_pos.x() - 4, fromY - 4, 8, 8);
				QString label = QString("t=") + QString::number(x) + ", v=" + QString::number(previousV);
				if (m_pos.x() > m_fromX && m_pos.x() < m_toX) {
					label += ", delta t=" + QString::number(m_toT - m_fromT);
				}
				p.drawText(m_pos.x() - 150, height() - 20, 300, 20, Qt::AlignCenter, label);
			}

			previousV = (*it).v;
			fromX = toX;
			fromY = toY;
		}

		if (!draw && m_pos.x() > fromX && m_pos.x() < width() - 20) {
			it--;
			p.drawRect(m_pos.x() - 4, fromY - 4, 8, 8);
			QString label = QString("t=") + QString::number(x) + ", v=" + QString::number((*it).v);
			p.drawText(m_pos.x() - 150, height() - 20, 300, 20, Qt::AlignCenter, label);
		}
		p.drawLine(fromX, fromY, width() - 20, fromY);
	}

// 	// Show gray lines at cursor position
// 	p.setPen(QPen(QColor(210, 210, 210), 1, Qt::SolidLine));
// 	p.drawLine(25, m_pos.y(), width() - 10, m_pos.y());
// 	p.drawLine(m_pos.x(), 10, m_pos.x(), height() - 20);
// 
// 	p.setPen(QPen(QColor(0, 0, 0), 1, Qt::SolidLine));
// 
// 	// Show time and value of current cursor position
// 	if (m_pos.x() > 100) {
// 		p.drawText(m_pos.x() - 100, m_pos.y(), 100, 20, Qt::AlignCenter, QString::number(y));
// 	}
// 	else {
// 		p.drawText(m_pos.x() + 20, m_pos.y(), 100, 20, Qt::AlignCenter, QString::number(y));
// 	}

	p.end();
}

void Plot::mouseReleaseEvent(QMouseEvent *event) {
}

int Plot::correctX(int x, double &t) {
	t = -1;
	if (!m_pinHistory) {
		return x;
	}

	QLinkedList<PinEvent>::iterator it = m_pinHistory->getEvents().begin();
	for (; it != m_pinHistory->getEvents().end(); ++it) {
		if ((*it).t > m_maxX) {
			break;
		}
		double toX = ((*it).t / m_maxX) * (width() - 35) + 25;
		double toY = height() - 20 - ((*it).v / m_maxY) * (height() - 30);		
		if (x > toX - 5 && x < toX + 5) {
			t = (*it).t;
			return toX;
		}
	}

	return x;
}

void Plot::mousePressEvent(QMouseEvent *event) {
	if (event->button() == Qt::RightButton) {
		m_fromX = -1;
		m_toX = -1;
		repaint();
		return;
	}

	if (m_fromX == -1 && m_toX == -1) {
		m_fromX = correctX(event->pos().x(), m_fromT);
		m_toX = -1;
	}
	else if (m_toX == -1) {
		m_toX = correctX(event->pos().x(), m_toT);
	}
	else {
		m_fromX = correctX(event->pos().x(), m_fromT);
		m_toX = -1;
	}
	repaint();
}

void Plot::mouseMoveEvent(QMouseEvent *event) {
	m_pos = event->pos();
	repaint();
}
