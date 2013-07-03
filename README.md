# RTLoc - Real Time Location for earthquake early warning
(c) 2006-2013 Claudio Satriano <satriano@ipgp.fr>,
Anthony Lomax <anthony@alomax.net>


## Introduction

RTLoc is an evolutionary, real-time earthquake location technique
based on an equal differential time (EDT) formulation and a
probabilistic approach for describing the hypocenter estimation.

The algorithm, at each timestep, relies on the information from
triggered arrivals and not-yet-triggered stations. With just one
recorded arrival, the hypocentral location is constrained by the
Voronoi cell around the first triggering station constructed using
the travel times to the not-yet-triggered stations.

With two or more triggered arrivals, the location is constrained
by the intersection of the volume defined by the Voronoi cells for
the remaining, not-yet-triggered stations and the EDT surfaces
between all pairs of triggered arrivals.

As time passes and more triggers become available, the evolutionary
location converges to a standard EDT location.


## Compiling and Installing

RTLoc is written in pure C, without any external library dependence.
It can be compiled by a C or C++ compiler.

To compile, edit the `Makefile` by selecting the installation prefix
and the compiler executable, then run:

    make

and, optionally,

    make install

in case you want to copy the executable to your prefix path.

`Grid2Time` program from Anthony Lomax (part of the [NonLinLoc]
suite) is required to generate traveltime grids.
[NonLinLoc]: http://alomax.free.fr/nlloc

Plotting routines are written in Python and require [matplotlib].
[matplotlib]: http://matplotlib.org


## Usage

See `doc/usage.md` for usage instructions.


## Testing the code

A complete text case is available in the `testrun/` directory.
See `testrun/README.md` for instructions on how to run the test.


## Important Note

This is a research implementation of the code, with no actual
real-time capabilities.

Feel free to pick the relevant parts of this (GPL licensed) code
and build your own real-time module!


## References

> Satriano, C., Lomax, A., Zollo, A. 2008.
> Real-Time Evolutionary Earthquake Location for Seismic Early Warning.
> Bull. Seismol. Soc. Am., 98(3), 1482–1494. doi: [10.1785/0120060159].


[10.1785/0120060159]: http://dx.doi.org/10.1785/0120060159


### DISCLAIMER
THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER
EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS
WITH YOU. SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF
ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
