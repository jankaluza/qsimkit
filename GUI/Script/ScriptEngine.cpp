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

#include <QFileInfo>
#include <PythonQt/PythonQt.h>
#include <PythonQt/PythonQt_QtAll.h>

#include "ScriptEngine.h"
#include "Script.h"

ScriptEngine::ScriptEngine() : m_quiet(false) {
	// Initialize engine
	//PythonQt::init(PythonQt::RedirectStdOut);
	PythonQt::init(PythonQt::IgnoreSiteModule | PythonQt::RedirectStdOut);
	PythonQt_QtAll::init();
	m_engine = PythonQt::self();

	// Initialize main module
	m_scripts.append(new Script(m_engine->getMainModule()));
	m_scripts.front()->eval("from PythonQt import *");
	m_scripts.front()->eval("import sys");
	m_scripts.front()->eval("setdefaultencoding = sys.setdefaultencoding");
	m_scripts.front()->eval("import site");
	m_scripts.front()->eval("setdefaultencoding('utf-8')");

	// Connect stdout
	connect(m_engine, SIGNAL(pythonStdOut(const QString&)), this, SLOT(printOutput(const QString&)));

	connect(m_engine, SIGNAL(pythonStdErr(const QString&)), this, SLOT(printOutput(const QString&)));

	// Set defaults
	setQuiet(false);
}

ScriptEngine::~ScriptEngine() {
	foreach(Script* script, m_scripts)
		delete script;

	m_scripts.clear();
	//PythonQt::cleanup();

	qDebug("ScriptEngine: cleanup finished");
}

void ScriptEngine::printOutput(const QString& str) {
	qDebug("%s",str.toStdString().c_str());
	emit(outputPrinted(str));
}

Script* ScriptEngine::load(const QString& file, const QString& module) {
	// If module is empty, parse in main
	if(module.isEmpty())
	{
		Script* main = m_scripts[0];
		main->evalFile(file);
		return main;
	}

	// Parse python script
	PythonQtObjectPtr code = m_engine->createUniqueModule();

	// Set import path
	QString importPath = QFileInfo(file).absolutePath();
	m_engine->setModuleImportPath(code.object(), QStringList() << importPath);

	// Evaluate file
	code.evalFile(file);
	if(code.isNull())
	{
		qDebug("DEBUG: ScriptEngine: cannot parse file '%s'.", file.toStdString().c_str());
		return 0;
	}

	// Create Script
	Script* script = new Script(code);
	m_scripts.append(script);

	// Register objects
	for(ObjectMap::iterator it = m_objects.begin(); it != m_objects.end(); ++it)
		script->registerObject(it.key(), it.value());

	return script;
}

void ScriptEngine::unload(Script* script)
{
	if(!m_scripts.contains(script))
		return;

	int index = m_scripts.indexOf(script);
	m_scripts.remove(index);
	delete script;
}

void ScriptEngine::registerObject(const QString& name, QObject* obj)
{
   // For every already registered
   foreach(Script* script, m_scripts)
      script->registerObject(name, obj);

   // Store for new scripts
   m_objects[name] = obj;
}

QVariant ScriptEngine::eval(const QString& script)
{
   return m_scripts.front()->eval(script);
}

void ScriptEngine::registerClass(const QMetaObject* obj)
{
   m_engine->registerClass(obj);
}

void ScriptEngine::setQuiet(bool state)
{
   m_quiet = state;
}

bool ScriptEngine::isQuiet() const
{
   return m_quiet;
}
