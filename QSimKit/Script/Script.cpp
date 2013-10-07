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

#include "Script.h"
#include <QDebug>

Script::Script(PythonQtObjectPtr pyobject)
   : m_context(pyobject)
{
}

void Script::registerObject(const QString& name, QObject *obj)
{
	return m_context.addObject(name,obj);
}

void Script::setVariable(const QString& name, const QVariant& value)
{ 
	return m_context.addVariable(name,value);
}
      
QVariant Script::getVariable(const QString& name)
{
	return m_context.getVariable(name);
}

QVariant Script::eval(const QString& code)
{
	return m_context.evalScript(code, Py_eval_input);
}

void Script::evalFile(const QString& file)
{
	return m_context.evalFile(file);
}

QVariant Script::call(const QString& callable, const QVariantList& argv)
{
	PythonQtObjectPtr obj = 0;

	QMap<QString, PythonQtObjectPtr>::iterator it = m_objCache.find(callable);
	if (it == m_objCache.end()) {
		obj = PythonQt::self()->lookupCallable(m_context, callable);
		if (!obj) {
			return QVariant();
		}
		m_objCache[callable] = obj;
	}
	else {
		obj = it.value();
	}

	return PythonQt::self()->call(obj, argv);
}
