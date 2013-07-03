# RTLoc - Real Time Location for earthquake early warning
(c) 2006-2013 Claudio Satriano <satriano@ipgp.fr>,
Anthony Lomax <anthony@alomax.net>

## Usage

RTLoc requires the following input files:

 1. Travel-time grids at the seismic network stations
    for a given 3D velocity model. Grid format is NonLinLoc 3D grid
    (http://alomax.free.fr/nlloc). Grids can be computed using the
    NonLinLoc software (Lomax et al., 2000).

 2. A control file containing 3D grid file paths and control
    parameters for the algorithm. Options are specified following
    a key-value mechanism; certain keys are repeatable.


### Control File

The following table shows the currently implemented keys, with the
corresponding possible values:

| Key     | Repeatable | Values      | Description                    
|---------|------------|-------------|------------------------------------------
| STA     | Yes        | name        | (string) station name
|         |            | P_grid_path | (string) basename for P time 3D grid
|         |            | S_grid_path | (string) basename for S time 3D grid
| PICK    | Yes        | name        | (string) station name
|         |            | val         | (real) pick time
| SUM     | No         | val         | (boolean)
|         |            |             | 1: stack pn,l and qn,m values
|         |            |             | 0: multiply pn,l and qn,m values
| POW     | No         | val         | (boolean)
|         |            |             | 1: rise the quantity P(i,j,k)/Pmax to the number of stations N (eq. 5)
|         |            |             | 0: do not rise to N
| SIGMA   | No         | val         | (real) value for equation (3)
| MAXRMS  | No         | val         | (real) maximum RMS threshold for declaring a new event
| OUTFILE | No         | path        | (string) basename for output files
| RUN     | No         | name        | (string) run name
| TNOW    | No         | val         | (real) current time (only for simulation)
| DELTA   | No         | val         | (real) time sampling step (only for simulation)
| ENDTIME | No         | val         | (real) stop time (only for simulation)


### Working Scheme

File `working_scheme.pdf` shows the RTLoc working scheme: after processing
the control file, 3D travel time grids are opened and the location
grid is initialized; then the program enters the main loop which,
for each time step performs a global exploration of the location grid,
searching for the maximum of the function Q(i,j,k) (eq. 5); for each step,
location statistics (mean value, standard deviation) are calculated and
the location grid is saved on the disk.


### Output

RTLoc output is:

 * at each time step, a grid file containing the values of
   Q(i,j,k);

 * a summary file (`rtloc-x.xx.out`, where `x.xx` is the time in
   seconds) with location statistics at each time step;

 * a log file.

Grid files follow the NonLinLoc structure (Lomax et al., 2000), with
two files corresponding to each grid:

 * a header file (`out.ev00.x.xx.hdr`, where `x.xx` is the time)
   with grid size and sampling step in ASCII format;

 * a buffer file (`out.ev00.x.xx.buf`) with probability values in
   binary format.
 
The (ASCII) summary file reports, for each time step, the information
on the estimated hypocentral position, location quality and, for each
station, hypocentral distance and computed P and S arrival times. The
file follows a key-value structure:

| Key           | Values                                       | Description
|---------------|----------------------------------------------|-----------------------------
| RTLOC         |                                              | Initial row
| TIME          | val                                          | Current time
| HYPOCENTER    | x val y val z val OT val                     | Maximum likelihood hypocenter
|               |                                              | (Cartesian coordinates and origin time)
| STATISTICS    | ExpectX val Y val Z val                      | Hypocenter statistics
|               | CovXX val XY val XZ val YY val YZ val ZZ val | (expected value, covariance matrix,
|               | EllAz1 val Dip1 val Len1 val                 | error ellipsoid)
|               | Az2 val Dip2 val Len2 val Len3 val           |
| GEOGRAPHIC    | Lat val Long val Depth val                   | Maximum likelihood hypocenter
|               |                                              | (geographic coordinates) 
| STAT_GEOG     | ExpectLat val Long val Depth val             | Expected hypocenter
|               |                                              | (geographic coordinates) 
| STA (repeat.) | name Dist val Ptime val Stime val            | Distance from the station,
|               |                                              | and P and S computed arrival times
| END_RTLOC     |                                              | Final row


## References

> Satriano, C., Lomax, A., Zollo, A. 2008.
> Real-Time Evolutionary Earthquake Location for Seismic Early Warning.
> Bull. Seismol. Soc. Am., 98(3), 1482–1494. doi: [10.1785/0120060159].

> Lomax, A. J., & Virieux, P. Volant & Berge, C., 2000.
> Probabilistic earthquake location in 3D and layered models:
> introduction of a Metropolis-Gibbs method and comparison with linear locations.
> Advances in Seismic Event Location, 101-134.


[10.1785/0120060159]: http://dx.doi.org/10.1785/0120060159


Last modified on Wed Jul 3 2013
