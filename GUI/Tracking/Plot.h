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

#include <QWidget>
#include <QString>
#include <QList>

class PinHistory;

class Plot : public QWidget
{
	Q_OBJECT

	public:
		Plot(QWidget *parent = 0);

		void setMaximumX(double x);
		void setMaximumY(double y);

		void showPinHistory0(PinHistory *pinHistory);
		void showPinHistory1(PinHistory *pinHistory);
		void clear();

	protected:
		void paintEvent(QPaintEvent *e);
		void mouseMoveEvent(QMouseEvent *event);
		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);

	private:
		int correctX(int x, double &t);
		void paintPin(QPainter &p, PinHistory *pin, double x, double y);

	private:
		double m_maxX;
		double m_maxY;
		QPoint m_pos;
		PinHistory *m_pinHistory0;
		PinHistory *m_pinHistory1;
		int m_fromX;
		int m_toX;
		double m_fromT;
		double m_toT;

};

