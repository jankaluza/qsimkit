---
layout: default
title: Simulation basics
---

## Simulation basics

QSimKit is [Discrete event simulator](http://en.wikipedia.org/wiki/Discrete_event_simulation). That means
there is list of scheduled events sorted by simulation time which are executed subsequently. Events are
produced by Simulation objects.

Every Simulation object has following 4 methods:

* `double timeAdvance()` - Returns number of seconds in simulation time (decimal number) after which `internalTransition()` method is called. This method is used to schedule the Simulation object execution.
* `void internalTransition()` - When there is proper simulation time and this Simulation object should be executed, QSimKit executes this method. It is the main method where the Simulation object does its work.
* `void output(SimulationEventList &output)` - This method is called to determine if the Simulation object wants to generate some output, which is set to `externalEvent(...)` method of another object. See the next chapter for more information about communication between Simulation objects.
* `void externalEvent(double t, const SimulationEventList &)` - This method is called when another object generates output using `output(...)` method.

### Communication between Simulation objects

Every Simulation object has the list of pins (integers `0 .. N`). Simulation object can generate Simulation event in its `output(...)` by appending
it to Simulation event list. Every Simulation event contains following data:

* ID (integer) of pin on which the output is generated
* Value (decimal number) representing the voltage on this pin.

In QSimKit, user can interconnect Simulation objects with wires. When there is output generated on particular pin of particular Simulation object,
it is forwarded to `externalEvent(...)` method of another object (if they are connected by wire). This way, Simulation objects can communicate.

### Simulation flow

The simulation starts with calling `timeAdvance()` method to schedule all Simulation objects. Usually, Simulation objects return 0 as a
return value of the first `timeAdvance()` call. Then there is `internalTransition()` method called in order depending on `timeAdvance()` return
value. After the `internalTransition()` method execution, `output(...)` method of particular Simulation object is called to determine possible
output. If there is some output, `externalEvent(...)` method of Simulation object connected to this object is called. After that, `timeAdvance()`
method is called again to reschedule the object. This loop repeats until the simulation is stopped.

### Oscillator example

To better understand usa-cases for the 4 methods in Simulation object, this subchapter describes them in oscillator Simulation object example.

Simulation start (Scheduling):

* `double timeAdvance()` - As first value returns 0 to schedule `internalTransition()` method to be called right after the simulation started in time 0.

Simulation event (Simulation time = 0):

* `void internalTransition()` - Prepares SimulationEvent according to the current state of the oscillator and changes oscillator state.
* `void output(SimulationEventList &output)` - Appends SimulationEvent to SimulationEventList.
* `double timeAdvance()` - Returns decimal number `t` according to frequency of oscillator. This schedules next Simulation event for this Simulation object.

Simulation event (Simulation time = 0 + `t`):

The methods are the same as in previous Simulation event. This is basically how oscillator could be implemented.

### LED example

In comparison to Oscillator, LED has only input (it does not generate any output).

Simulation start (Scheduling):

* `double timeAdvance()` - Since LED does not generate any output, this method returns any "big enough" value like 9999 to not schedule any event for LED.

Simulation event (Simulation time = `t`):

Another Simulation object generated output and it is connected with this Simulation object. `externalEvent(...)` method is therefore called with the
decimal value representing the voltage on the wire.

* `void externalEvent(double t, const SimulationEventList &)` - According to voltage of SimulationEvents in SimulationEventList, turn the LED on/off.

