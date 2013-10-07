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

#include <QVariant>
#include <QMap>
#include <PythonQt/PythonQt.h>

class Script{
	public:
		Script(PythonQtObjectPtr pyobject);

		/* Object importer */
		void registerObject(const QString& name, QObject *obj);

		/* Script API */
		void setVariable(const QString& name, const QVariant& value);
		QVariant getVariable(const QString& name);
		QVariant eval(const QString& code);
		void evalFile(const QString& file);
		QVariant call(const QString& callable, const QVariantList& argv = QVariantList());

	protected:
		PythonQtObjectPtr m_context;

		PyObject* object() {
			return m_context.object();
		}

	private:
		QMap<QString, PythonQtObjectPtr> m_objCache;

	friend class ScriptEngine;
};
