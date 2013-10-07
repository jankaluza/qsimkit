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

#include "adevs.h"
#include <map>
#include <set>
#include <cstdlib>
#include "SimulationObject.h"

class Digraph: 
public adevs::Network<SimulationEvent, double>
{
	public:
		typedef adevs::Devs<SimulationEvent, double> Component;

		/// Construct a network with no components.
		Digraph():
		adevs::Network<SimulationEvent, double>()
		{
		}
		/// Add a model to the network.
		void add(Component* model);
		/// Puts the network's components into to c
		void getComponents(adevs::Set<Component*>& c);
		/// Route an event based on the coupling information.
		void route(const SimulationEvent& x, Component* model, 
		adevs::Bag<adevs::Event<SimulationEvent, double> >& r);
		/// Destructor.  Destroys all of the component models.
		~Digraph();

	private:	

		// Component model set
		adevs::Set<Component*> models;
};
