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

There are 5 methods in every peripheral controlling the simulation. If you are going to write your
own peripheral, you will have to implement all of them and they are the only way how to
interact with simulation and with other peripherals.

#### timeAdvance() method

This method is used to schedule next event of peripheral. It returns decimal number
(number of seconds in simulation time) after which `internalTransition()` method is called.

It is the first method called by QSimKit. If peripheral returns 0.1 in this method, it means
that when simulation time is `t + 0.1`, `internalTransition()` method will be called and peripheral
can do some state change (for example generate output).

#### internalTransition() method

This method is called by QSimKit after the delay defined by `timeAdvance()` method return value.

It is usually used if the peripheral has some independet behaviour. For example in case of
oscillator peripheral, the oscillator would return proper time value according to its frequency in
`timeAdvance()` method and later, in `internalTransition()` method, the oscillator would generate
desired output using `output()` method.

#### output() method

Every peripheral has list of pins. When the peripheral wants to output some decimal value (voltage) on particular
pin, it can do it using this method. QSimKit calls this method after `internalTransition()` method.

#### externalTransition() method

When there is another peripheral connected to particular pin and its `output()` method generates the output
value, then this peripheral will receive this value (voltage on the wire) using this method.
