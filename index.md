---
layout: default
title: Welcome
---

## About

QSimKit is cross-platform, modular, discrete MSP430 (theoretically not only MSP430) simulator. It supports
various MSP430 features and external peripherals such as LED, LCD display or keyboard.
QSimKit is released under GPLv2+ license.

## Features

*QSimKit graphical user interface:*
- Adding peripherals written in C++ or Python
- Connecting pins
- Inspecting MCU memory and registers
- Stepping assembler or C code
- Adding register-based breakpoints (simulation stops when register, for example PC, has particular value)
- Adding memory-based breakpoints (simulation stops when byte in memory has particular value)
- Tracking pins in oscilloscope-like view (ability to "point to instruction" which caused particular change)

*MSP430 MCU module:*
- Loading ELF/A43 code and disassembling the code using msp430-objdump (only msp430-gcc supported so far)
- All instructions implemented
- BasicClock module supported (TimerA/TimerB support)


