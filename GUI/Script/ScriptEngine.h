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
 * Foundation, Inc->, 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 **/

/***************************************************************************
 *   Copyright (C) 2008 Brno University of Technology,                     *
 *   Faculty of Information Technology                                     *
 *   Author(s): Marek Vavrusa    <xvavru00 AT stud.fit.vutbr.cz>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#pragma once

#include <QObject>
#include <QVector>
#include <QMap>


/* Typemaps */
class Script;
class PythonQt;

typedef QVector<Script*> ScriptList;
typedef QMap<QString, QObject*> ObjectMap;

class ScriptEngine : public QObject {
	Q_OBJECT
	Q_PROPERTY(bool quiet READ isQuiet WRITE setQuiet)
	public:
		ScriptEngine();
		~ScriptEngine();

		/* Script runners */
		Script* load(const QString& file, const QString& module = QString());
		QVariant eval(const QString& script);

		void unload(Script* script);

		/* Object registration */
		void registerObject(const QString& name, QObject* obj);
		void registerClass(const QMetaObject* obj);

		/* Output control */
		void setQuiet(bool state);
		bool isQuiet() const;

	protected slots:
		void printOutput(const QString& str);

	signals:
		void outputPrinted(const QString& output);

	private:
		bool m_quiet;
		PythonQt *m_engine;
		ScriptList m_scripts;
		ObjectMap m_objects;
};
