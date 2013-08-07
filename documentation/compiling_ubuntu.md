---
layout: default
title: Compiling in Ubuntu/Debian
---

## Compiling in Ubuntu/Debian

Install dependencies:

	sudo apt-get install libqt4-dev python-dev

Get the sources from Git repository:

	git clone https://github.com/hanzz/qsimkit.git

Compile and install QSimKit:

	cmake . -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Debug
	make
	sudo make install

You can omit "-DCMAKE_BUILD_TYPE=Debug", but that way QSimKit developers won't be
able to get proper crash report if QSimKit crashes for you and you will ask for help.

Run QSimKit:

	qsimkit
