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
#include <iostream>

#include <QWidget>
#include <QPainter>
#include <QAction>
#include <QMenu>
#include <QToolTip>
#include <QString>
#include <QMouseEvent>
#include <QDebug>

#define PLOT_HEIGHT 30
#define PLOT_SPACE_LEFT 75
#define PLOT_SPACE_RIGHT 10
#define PLOT_SPACE_BETWEEN 5
#define PLOT_WIDTH (width() - PLOT_SPACE_LEFT - PLOT_SPACE_RIGHT)

Plot::Plot(QWidget *parent) : QWidget(parent), m_maxX(1.0), m_minX(0), m_maxY(3.3), m_pinHistory0(0),
	m_pinHistory1(0), m_fromX(-1), m_toX(-1)  {
	setMouseTracking(true);

	// Test:
// 	PinHistory *m_pinHistory = new PinHistory();
// 	m_pinHistory->addEvent(0.5, 1.0, 0);
// 	m_pinHistory->addEvent(0.6, 3.3, 0);
// 	m_pinHistory->addEvent(0.65, 0.3, 0);
// 	m_pinHistory->addEvent(0.8, 0.0, 0);
// 	m_pins << m_pinHistory;
// 	m_pins << m_pinHistory;

	refreshSize();
}

void Plot::setMaximumX(double x) {
	m_maxX = x;
	m_realMaxX = x;
	repaint();
}

void Plot::setMaximumY(double y) {
	m_maxY = y;
	repaint();
}

void Plot::resetView() {
	m_minX = 0;
	m_maxX = m_realMaxX;
	repaint();
}

void Plot::refreshSize() {
	parentWidget()->setMinimumHeight(PLOT_SPACE_BETWEEN + m_pins.size() * (PLOT_HEIGHT + PLOT_SPACE_BETWEEN) + 15);
	parentWidget()->setMaximumHeight(PLOT_SPACE_BETWEEN + m_pins.size() * (PLOT_HEIGHT + PLOT_SPACE_BETWEEN) + 15);
}

void Plot::clear() {
	m_pins.clear();
	refreshSize();
	repaint();
}

void Plot::addPinHistory(const QString &name, PinHistory *pinHistory) {
	PlotPin p;
	p.name = name;
	p.pin = pinHistory;
	m_pins.append(p);

	refreshSize();
	repaint();
}

void Plot::removePinHistory(const QString &name) {
	
// 	m_pins.erase(name);
	refreshSize();
	repaint();
}

void Plot::paintPin(QPainter &p, const QString &name, PinHistory *pin, int slot, double x) {
	p.setPen(QPen(QColor(0, 0, 0), 1, Qt::SolidLine));

	// Draw name of the pin
	p.drawText(10, PLOT_SPACE_BETWEEN + slot * (PLOT_HEIGHT + PLOT_SPACE_BETWEEN),
			   PLOT_SPACE_LEFT - 10, PLOT_HEIGHT, Qt::AlignCenter | Qt::TextWrapAnywhere, name);

	// Draw axis
	p.drawLine(PLOT_SPACE_LEFT, PLOT_HEIGHT + slot * (PLOT_HEIGHT + PLOT_SPACE_BETWEEN),
			   PLOT_SPACE_LEFT, PLOT_SPACE_BETWEEN + slot * (PLOT_HEIGHT + PLOT_SPACE_BETWEEN));
	p.drawLine(PLOT_SPACE_LEFT, PLOT_HEIGHT + slot * (PLOT_HEIGHT + PLOT_SPACE_BETWEEN),
			   width() - PLOT_SPACE_RIGHT, PLOT_HEIGHT + slot * (PLOT_HEIGHT + PLOT_SPACE_BETWEEN));

	// No PinHistory yet, so just return
	if (!pin) {
		return;
	}

	// Draw axis descriptions
// 	p.drawText(21, height() - 20, 10, 20, Qt::AlignCenter, "0");
// 	p.drawText(0, 0, 20, 20, Qt::AlignCenter, QString::number(m_maxY));

	// Draw selection when user wants to zoom particuular part of plot
	if (m_fromX != -1 && m_toX != -1) {
		// User already finished selection (mouse button  is released)
		p.fillRect(m_fromX, slot * (PLOT_HEIGHT + PLOT_SPACE_BETWEEN),
				   m_toX - m_fromX, PLOT_HEIGHT,
				   palette().highlight());
	}
	else if (m_fromX != -1 && m_toX == -1) {
		// User is still selecting (mouse button is down, so use m_pos.x())
		p.fillRect(m_fromX, slot * (PLOT_HEIGHT + PLOT_SPACE_BETWEEN),
				   m_pos.x() - m_fromX, PLOT_HEIGHT,
				   palette().highlight());
	}

	// Set color of the plot line
	p.setPen(QPen(QColor(255, 0, 0, 128), 2, Qt::SolidLine));

	double toX = 0;
	double toY = 0;
	double fromX = PLOT_SPACE_LEFT;
	double fromY = PLOT_HEIGHT + slot * (PLOT_HEIGHT + PLOT_SPACE_BETWEEN);
	bool draw = false;
	double previousV = 0;
	bool skipped = false;

	bool mouseInPlot = m_pos.y() < PLOT_HEIGHT + slot * (PLOT_HEIGHT + PLOT_SPACE_BETWEEN) &&
		m_pos.y() > PLOT_SPACE_BETWEEN + slot * (PLOT_HEIGHT + PLOT_SPACE_BETWEEN);

	// Skip all events which happend before current 'x'
	QLinkedList<PinEvent>::iterator it = pin->getEvents().begin();
	for (; it != pin->getEvents().end(); ++it) {
		if ((*it).t < m_minX) {
			continue;
		}

		// go backward one pin to be able to decide later from which value
		// we want to draw initial line in plot
		if (it != pin->getEvents().begin()) {
			it--;
		}
		break;
	}


	for (; it != pin->getEvents().end(); ++it) {
		// Do not paint pins which are out of boundaries
		if ((*it).t > m_maxX) {
			break;
		}

		// Get the coordinates of the next point to which we will draw to line to
		toX = ((*it).t / (m_maxX - m_minX)) * (PLOT_WIDTH) + PLOT_SPACE_LEFT - (m_minX / (m_maxX - m_minX)) * (PLOT_WIDTH);
		toY = PLOT_HEIGHT + slot * (PLOT_HEIGHT + PLOT_SPACE_BETWEEN) - ((*it).v / m_maxY) * (PLOT_HEIGHT);

		// If these two points are too close together, show them
		// as vertical dashed line.
		if (toX - fromX < 3 && toX != fromX) {
			previousV = (*it).v;
			QPen pen = p.pen();
			QPen n = pen;
			n.setWidth(1);
			n.setStyle(Qt::DashLine);
			p.setPen(n);
			p.drawLine(toX, fromY, toX, toY);
			p.setPen(pen);
			skipped = true;
			fromY = toY;
			continue;
		}

		// Draw the horizontal line representing value change
		p.drawLine(fromX, fromY, toX, fromY);

		if (it + 1 != pin->getEvents().end()) {
			// Get the 'x' coordinate of the next point we will draw vertical line to
			int toX2 = ((*(it + 1)).t / (m_maxX - m_minX)) * (PLOT_WIDTH) + PLOT_SPACE_LEFT - (m_minX / (m_maxX - m_minX)) * (PLOT_WIDTH);

			// If the next 'x' coordinate is close to this 'x' coordinate,
			// show only vertical dashed line on this place. Otherwise
			// draw proper vertical line.
			if (toX2 - toX < 3 && toX2 != toX) {
				QPen pen = p.pen();
				QPen n = pen;
				n.setWidth(1);
				n.setStyle(Qt::DashLine);
				p.setPen(n);
				p.drawLine(toX, fromY, toX, toY);
				p.setPen(pen);
			}
			else {
				p.drawLine(toX, fromY, toX, toY);
			}
		}
		else {
			p.drawLine(toX, fromY, toX, toY);
		}

		// If the mouse pointer is close to some point, snap to this point
		if (mouseInPlot) {
			if (!draw && m_pos.x() > toX - 5 && m_pos.x() < toX + 5) {
				p.drawRect(toX - 4, toY - 4, 8, 8);
				QString label = QString("t=") + QString::number((*it).t) + ", v=" + QString::number((*it).v);
				if (m_pos.x() > m_fromX && m_pos.x() < m_toX) {
					label += ", delta t=" + QString::number(m_toT - m_fromT);
				}
				p.drawText(10, height() - 20, 300, 20, Qt::AlignLeft, label);
				draw = true;
			}
			else if (!draw && m_pos.x() > fromX && m_pos.x() < toX) {
				p.drawRect(m_pos.x() - 4, fromY - 4, 8, 8);
				QString label = QString("t=") + QString::number(x) + ", v=" + QString::number(previousV);
				if (m_pos.x() > m_fromX && m_pos.x() < m_toX) {
					label += ", delta t=" + QString::number(m_toT - m_fromT);
				}
				p.drawText(10, height() - 20, 300, 20, Qt::AlignLeft, label);
			}
		}

		previousV = (*it).v;
		fromX = toX;
		fromY = toY;
	}

	// Display label for last line
	if (mouseInPlot && !draw && m_pos.x() > fromX && m_pos.x() < width() - 20) {
		it--;
		p.drawRect(m_pos.x() - 4, fromY - 4, 8, 8);
		QString label = QString("t=") + QString::number(x) + ", v=" + QString::number((*it).v);
		if (m_pos.x() > m_fromX && m_pos.x() < m_toX) {
			label += ", delta t=" + QString::number(m_toT - m_fromT);
		}
		p.drawText(10, height() - 20, 300, 20, Qt::AlignLeft, label);
	}

	// Last line is only horizontal and ends up in the infinity
	p.drawLine(fromX, fromY, width() - 20, fromY);
}

void Plot::paintEvent(QPaintEvent *e) {
	QPainter p;
	p.begin(this);
	p.fillRect(QRect(0, 0, width(), height()), QBrush(QColor(255, 255, 255)));

	// Get the X value of mouse pointer, so we can show the exact Y value
	// for this X in paintPin later.
	double x = (double(m_pos.x() - PLOT_SPACE_LEFT) / PLOT_WIDTH) * (m_maxX - m_minX) + m_minX;

	int slot = 0;
	foreach(const PlotPin &plotpin, m_pins) {
		paintPin(p, plotpin.name, plotpin.pin, slot++, x);
	}

	p.end();
}

void Plot::mouseReleaseEvent(QMouseEvent *event) {
}

int Plot::correctX(int x, double &t) {
	m_context = 0;
	t = (double(x - 25) / (width() - 35)) * (m_maxX - m_minX) + m_minX;
	if (!m_pinHistory0) {
		return x;
	}

	QLinkedList<PinEvent>::iterator it = m_pinHistory0->getEvents().begin();
	for (; it != m_pinHistory0->getEvents().end(); ++it) {
		if ((*it).t < m_minX) {
			continue;
		}
		if ((*it).t > m_maxX) {
			break;
		}
		double toX = ((*it).t / (m_maxX - m_minX)) * (width() - 35) + 25 - (m_minX / (m_maxX - m_minX)) * (width() - 35);
		if (x > toX - 5 && x < toX + 5) {
			t = (*it).t;
			m_context = (*it).context;
			return toX;
		}
	}

	return x;
}

void Plot::mousePressEvent(QMouseEvent *event) {
	if (event->button() == Qt::RightButton) {
		QList<QAction *> actions;

		double dummy;
		correctX(event->pos().x(), dummy);
		if (m_context != 0) {
			QAction *action = new QAction("Point to instruction", 0);
			action->setData(2);
			actions.append(action);
		}

		if (m_fromX != -1 && m_toX != -1) {
			QAction *action = new QAction("Zoom", 0);
			action->setData(0);
			actions.append(action);
		}

		if (m_realMaxX != m_maxX) {
			QAction *action = new QAction("Reset zoom", 0);
			action->setData(1);
			actions.append(action);
		}

		if (!actions.isEmpty()) {
			QAction *action = QMenu::exec(actions, event->globalPos(), 0, 0);
			if (action) {
				if (action->data() == 0) {
					std::cout << "Zoom " << m_fromT << " " << m_toT << "\n";
					m_minX = m_fromT;
					m_maxX = m_toT;
					m_fromX = -1;
					m_toX = -1;
					repaint();
				}
				else if (action->data() == 1) {
					resetView();
				}
				else if (action->data() == 2) {
					onPointToInstruction(m_context);
				}
			}
		}
		return;
	}

	if (m_fromX == -1 && m_toX == -1) {
		m_fromX = correctX(event->pos().x(), m_fromT);
		m_toX = -1;
	}
	else if (m_toX == -1) {
		m_toX = correctX(event->pos().x(), m_toT);
		if (m_toX < m_fromX) {
			double tmp = m_toX;
			m_toX = m_fromX;
			m_fromX = tmp;

			tmp = m_toT;
			m_toT = m_fromT;
			m_fromT = tmp;
		}
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
