---
layout: default
title: MCU modules overview
---

## MCU modules overview

Every MCU (currently only MSP430) is QtPlugin (shared library) written in C++. It has to
implement [MCUInterface](https://github.com/hanzz/qsimkit/blob/master/GUI/MCU/MCUInterface.h).
This interface allows QSimKit to create new instance of MCU object representing the MCU.

This document describes various classes related to MCU modules and shows the relationship between
them and QSimKit

### MCU class

[MCU class](https://github.com/hanzz/qsimkit/blob/master/GUI/MCU/MCU.h) is the basic class for
MCU modules which must be implemented by any MCU.

It inherits Peripheral class which is described in [Peripherals overview](peripherals_overview)
part of the Development documentation. You should read [Peripherals overview](peripherals_overview)
before continuing reading this document to fully understand how the simulation works and how
peripherals (including the MCU) communicate together.

#### MCU Variants

Every MCU can have different variants. For example MSP430 familly has variants like msp430x11x, msp430x12x and so on.
This is reflected in MCU class by two methods:

* MCU::getVariant() - Implementation should return currently used variant.
* MCU::getVariants() - Implementation should return list of all available variants.


#### MCU Registers




