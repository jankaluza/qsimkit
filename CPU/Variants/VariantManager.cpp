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

#include "CPU/Variants/VariantManager.h"
#include "CPU/Variants/Variant.h"

#include <iostream>
#include <sstream>
#include <map>

static std::map<const char *, _msp430_variant *> *variants;

void addVariant(_msp430_variant *variant) {
	if (variants == 0) {
		variants = new std::map<const char *, _msp430_variant *>;
	}
	(*variants)[variant->name] = variant;
	std::cout << (*variants)[variant->name]->name << "\n";
}

_msp430_variant::_msp430_variant(const char *name, Variant *(*fnc)()) {
	this->name = name;
	create_variant = fnc;
	addVariant(this);
}