---
layout: default
title: Peripherals overview
---

## Peripherals overview

Peripherals are the basic simulation objects. Every peripheral has following attributes:

* list of pins
* position and size of the peripheral on the screen
* method to paint the peripheral on the screen
* 5 methods controlling the simulation

### Simulation flow

There are 5 methods in every peripheral controlling the simulation:

#### timeAdvance() method

This method is used to schedule next event of peripheral. It returns decimal number
(number of seconds in simulation time) after which `internalTransition()` method is called.

It is the first method called by QSimKit. If peripheral returns 0.1 in this method, it means
that when simulation is 0.1, `internalTransition()` method will be called and peripheral
can do some state change (for example generate output).

#### internalTransition() method


