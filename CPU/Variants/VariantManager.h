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

// #include <stdint.h>
// #include <string>
// #include <vector>

class Variant;
class _msp430_variant;

void addVariant(_msp430_variant *variant);

class _msp430_variant {
	public:
		_msp430_variant(const char *name, Variant *(*fnc)());
		const char *name;
		Variant *(*create_variant)();
};

#define MSP430_VARIANT(NAME, CLASS) static Variant *create_##CLASS() { \
		return (Variant *) new Variant_##CLASS(); \
	} \
	static const char *variant_name_##CLASS = NAME;\
	_msp430_variant _msp430_variant_##CLASS(variant_name_##CLASS, &create_##CLASS);
