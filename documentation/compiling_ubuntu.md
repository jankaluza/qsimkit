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

	cmake . -DCMAKE_INSTALL_PREFIX=/usr
	make
	sudo make install

Run QSimKit:

	qsimkit
