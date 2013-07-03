# RTLoc - Real Time Location for earthquake early warning
(c) 2006-2013 Claudio Satriano <satriano@ipgp.fr>,
Anthony Lomax <anthony@alomax.net>


This is a complete test case for RTLoc.


## Generating traveltime grids

In order to run it, you must have station traveltime grids
in the `time/` directory.

Grids are generated using the programs `Vel2Grid` and
`Grid2Time` from the [NonLinLoc] suite.
[NonLinLoc]: http://alomax.free.fr/nlloc

To generate the P-wave model, run:

    Vel2Grid run/irpinia.run

Model files will be created in the `model/` dir.

To generate the S-wave model, first edit the file `run/Vel2Grid.in`
and change:

    VGTYPE P

to:

    VGTYPE S

then run again:

    Vel2Grid run/irpinia.run

Model files will be again created in the `model/` grid.


To generate P-wave traveltime grids, run:

    Grid2Time run/irpinia.run

Traveltime grid files will be created in the `time/` dir.

To generate S-wave traveltime grids, first edit the file `run/Grid2Time.in`
and change:

    GTFILES  ./model/layer  ./time/layer P

to:

    GTFILES  ./model/layer  ./time/layer S

then run again:

    Grid2Time run/irpinia.run

Traveltime grid files will be again created in the `time/` dir.


Alternatively you can download pre-generated traveltime grids for this
test case, from the following [link].
[link]: http://


## Running the test

Run:

    ./test.sh run

or:

    ../rtloc run01.rtloc.ctrl

This should create the following files and dir:

    run01.rtloc/
    run01.rtloc.log
    run01.rtloc.stat
    run01.rtloc.stations


## Plotting results

Run:

    ./test.sh plot

This should create PDF plots for each time step in the dir:

    run01.pdf/

Warning: plotting requires Python and [matplotlib].
[matplotlib]: http://matplotlib.org


## Reset the test

To reset the test, run:

    ./test.sh clean


## NLLoc location

You can locate the test event using NLLoc, by running:

    NLLoc run/irpinia.run

Location results are stored in the `loc/` dir.
They can be displayed using [Seismicity Viewer]:

    java net.alomax.seismicity.Seismicity loc/irpinia.19300723.010804.grid0.loc.hyp

Assuming that you have Java installed and that `SeismicityViewer50.jar`
(or later version) is in your Java `CLASSPATH`.
[Seismicity Viewer]: http://alomax.free.fr/seismicity
