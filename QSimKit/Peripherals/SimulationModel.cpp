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

#include "SimulationModel.h"

#include <QDebug>


void SimulationModel::add(Component* model)
{
	assert(model != this);
	models.insert(model);
	model->setParent(this);
}

void SimulationModel::getComponents(adevs::Set<Component*>& c)
{
	c = models;
}

void SimulationModel::
route(const SimulationEvent& x, Component* model, 
adevs::Bag<adevs::Event<SimulationEvent, double> >& r)
{
	SimulationObjectWrapper *obj = static_cast<SimulationObjectWrapper *>(model);

	adevs::Event<SimulationEvent> event;
	event.model = obj->getTarget(x.port, event.value.port);
	if (!event.model) {
		return;
	}

	event.value.value = x.value;
	r.insert(event);

}

SimulationModel::~SimulationModel()
{ 
	adevs::Set<Component*>::iterator i;
	for (i = models.begin(); i != models.end(); i++)
	{
		delete *i;
	}
}

