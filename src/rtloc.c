/*
 * @file rtloc.c main routine
 * 
 * Copyright (C) 2006-2013 Claudio Satriano <satriano@ipgp.fr> and
 * Anthony Lomax <anthony@alomax.net>
 * This file is part of RTLoc.
 * 
 * RTLoc is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * RTLoc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with RTLoc; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */ 
#include "rtloclib.h"
#include <stdarg.h>
#include <stdio.h>

#define WRITE_YES 1
#define WRITE_NO 0

float tnow; //Current time has to be a global variable
float sigma;
int edt_null;

char *stationfilename;
char *statfilename;
char *logfilename;

int main (int argc, const char * argv[])
{
    char ctrlfilename[80];
    FILE *ctrlfile;
    struct Station *station;
    struct Pick *pick;
    struct Control params;
    //int is;
    int nsta;
    int npick;
    int n, t;

    FILE **Phdr, **Pbuf;
    FILE **Shdr, **Sbuf;
    GridDesc *Pgrid; // P-Travel time grid for each station
    GridDesc *Sgrid; // S-Travel time grid for each station

    // AJL 20070110
    //float *Ptt; // P-Travel times at each station for a given grid point
    //float *Stt; // S-Travel times at each station for a given grid point
    // -AJL

    GridDesc Grid; // Location grid
    //int ix, iy, iz;
    //int numx, numy, numz; // Size of the grid

    float *prob_max;
    float new_prob_max;

    int evid;
    int evcounter=0;
    int associated;
    int statid;

    int imean[3];
    float rms;


    //message_flag=0; // Verbosity level
    message_flag=1; // AJL 20070110

    if (argc < 2) {
        DispProgInfo();
        printf("Usage: %s ctrlfile\n", argv[0]);
        return 0;
    }

    sprintf (ctrlfilename, "%s", argv[1]);
    ctrlfile = ReadCtrlFile (ctrlfilename, &params);

    nsta = params.nsta;
    npick = params.npick;
    sigma = params.sigma;

    station = NULL;
    station = ReadStation (ctrlfile, station, nsta);
    pick = NULL;
    pick = ReadPick (ctrlfile, pick, npick, station, nsta);


    // AJL 20070111
    // initialize random number generator - usef by OctTree
    SRAND_FUNC(9837);
    // -AJL


    /* Set null value for edt according to the approach chosen */
    if ( params.sum ) edt_null = 0;
    else edt_null = 1;

    /* We need some error checking here... */
    Phdr = (FILE **) calloc (nsta, sizeof(FILE *));
    Shdr = (FILE **) calloc (nsta, sizeof(FILE *));
    Pbuf = (FILE **) calloc (nsta, sizeof(FILE *));
    Sbuf = (FILE **) calloc (nsta, sizeof(FILE *));
    Pgrid = (GridDesc *) calloc (nsta, sizeof(GridDesc));
    Sgrid = (GridDesc *) calloc (nsta, sizeof(GridDesc));

    /* Allocation to fit 10 events
    (TODO: check if we can use realloc() )*/
    prob_max = (float *) calloc (10, sizeof (float));


    /* Let's open all the time grid files... */
    FILE *station_fp = fopen(stationfilename, "w");
    for (n=0; n<nsta; n++) {
        // P
        if ( OpenGrid3dFile(station[n].Pfile, &Pbuf[n], &Phdr[n], &Pgrid[n], "time", &station[n].desc, 0) < 0) {
            puterr2 ("ERROR opening grid file: ", station[n].Pfile);
            exit (EXIT_ERROR_FILEIO);
        }
        // AJL 20070110 - set up standard NLL grid storage and array access
        AllocateGrid(&Pgrid[n]);
        CreateGridArray(&Pgrid[n]);
        ReadGrid3dBuf(&Pgrid[n], Pbuf[n]);
        CloseGrid3dFile(&Pbuf[n], &Phdr[n]); //c.s. 20090109

        // S
        if ( OpenGrid3dFile(station[n].Sfile, &Sbuf[n], &Shdr[n], &Sgrid[n], "time", NULL, 0) < 0) {
            puterr2 ("ERROR opening grid file: ", station[n].Sfile);
            exit (EXIT_ERROR_FILEIO);
        }
        // AJL 20070110 - set up standard NLL grid storage and array access
        AllocateGrid(&Sgrid[n]);
        CreateGridArray(&Sgrid[n]);
        ReadGrid3dBuf(&Sgrid[n], Sbuf[n]);
        CloseGrid3dFile(&Sbuf[n], &Shdr[n]); //c.s. 20090109

        /*TODO: we have to check all the grids to be the same or
        think at defining a grid which contains them all */

        char tmp_hdr_fname[200];
        FILE *tmpfp;
        char tmpline[100];
        sprintf(tmp_hdr_fname, "%s.hdr", station[n].Pfile);
        tmpfp = fopen(tmp_hdr_fname, "r");
        fgets(tmpline, 100, tmpfp);
        fgets(tmpline, 100, tmpfp);
        fprintf(station_fp, "%s", tmpline);
        fclose(tmpfp);
    }
    fclose(station_fp);

    /* If all the grids are the same, we can use one of them
    as a prototype for the location grid */
    initLocGrid(&Pgrid[0], &Grid);

//should we sort the picks????
//I don't think so

    /** LOOP OVER TIME **/
    /* We loop over tnow */
    /* This loop eventually could be substituted by a while(1) infinite
    loop, thus making a continuous scanning of the location grid.
    Of course, there should be a condition to break the loop... or not?
    hmmm....
    */
    t=0;
    tnow = params.tnow + t*params.dt;
    while ( tnow <= params.endtime )
    {


        //New pick?
        /* Note: this is getting more and more chaotic.
            Should we move this to a function?
        */
        for (n=0; n<npick; n++) if ( (pick[n].time <= tnow) && (pick[n].evid < 0) ) {
            evid=0;
            /* If there's no event yet... */
            if ( ! evcounter ) {
            //Declare a new event
                evcounter = 1;
                pick[n].evid = 0;
            //Associate the event with the station
                statid = pick[n].statid;
                station[statid].evid[0] = 1;
                printlog ("(%4.2f) Pick %d stat %d associated to new event %d\n", tnow, n, statid, pick[n].evid);
//fprintf (stderr, "(%4.2f) Pick %d associated to event %d\n", tnow, n, pick[n].evid);
            }
            /* If there's already an event... */
            else {
                associated = 0;
                for (evid=0; evid<evcounter; evid++) {
                /* temporaly associate the pick and
                    the station to the existing event
                */
                    pick[n].evid = evid;
                    statid = pick[n].statid;
                    station[statid].evid[evid] = 1;
                /* compute the true_edt using all the
                    picks associated to the event evid */
                    new_prob_max = SearchEdt (&Grid, pick, station, Pgrid, Sgrid, evid, &params, WRITE_NO, imean); //FIRST CALL TO SearchEdt

                // WARNING!!!!! This is crucial
                //OLDCODE: check if prob_max gets bigger
//fprintf (stderr, "(%4.2f) event: %d new_prob_max: %f old: %f\n", tnow, evid, new_prob_max, prob_max[evid]);
//                if ( (new_prob_max >= prob_max[evid]) || (prob_max[evid] - new_prob_max <= params.probthreshold) ) {

                //NEWCODE: check if new rms is smaller
                    rms = GetRms (imean, Pgrid, Sgrid, pick, n, &Grid, &params);
                    printlog("(%4.2f) event %d: rms = %f\n", tnow, pick[n].evid, rms);
                    if ( rms < params.maxrms ){
                        associated = 1;
                        printlog("(%4.2f) Pick %d stat %d associated to existing event %d\n", tnow, n, statid, pick[n].evid);
                        break; //we don't eveluate, for now, other possible associations
                    }
                }
            //if the pick has not been associated...
                if ( ! associated ) {
                //Declare a new event and associate it to the station
                    evid = evcounter++;
                    pick[n].evid = evid;
                    statid = pick[n].statid;
                    station[statid].evid[evid] = 1;
                    printlog("(%4.2f) Pick %d stat %d associated to event %d\n", tnow, n, statid, pick[n].evid);
                }
            }
        }


    //Compute for all the events the new probability map
        for (evid=0; evid<evcounter; evid++)
            prob_max[evid] = SearchEdt (&Grid, pick, station, Pgrid, Sgrid, evid, &params, WRITE_YES, imean); //SECOND CALL TO SearchEdt


        t++;
        tnow = params.tnow + t*params.dt;
    } /**END OF LOOP OVER TIME**/




    for (n=0; n<nsta; n++) {
        //CloseGrid3dFile(&Pbuf[n], &Phdr[n]); c.s. 20090109
        FreeGrid(&Pgrid[n]);
        DestroyGridArray(&Pgrid[n]);
        //CloseGrid3dFile(&Sbuf[n], &Shdr[n]); c.s. 20090109
        FreeGrid(&Sgrid[n]);
        DestroyGridArray(&Sgrid[n]);
        //fclose (Pbuf[n]);
        //fclose (Sbuf[n]);
    }


    return 0;
    //Phew, it's done!!!
}
