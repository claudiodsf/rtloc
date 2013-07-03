/*
 * @file SearchEdt.c grid search routines
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
extern float tnow;
extern float sigma;
extern int edt_null;



/** full normalization routine - Grid -> 0.0-1.0 */

void testFullNormalizationAJL(GridDesc *Grid) {

    int ix, iy, iz;

    double val_max = -1.0e30;
    double val_min = 1.0e30;

    int numx = Grid->numx;
    int numy = Grid->numy;
    int numz = Grid->numz;

    for (ix=0; ix<numx; ix++)
        for (iy=0; iy<numy; iy++)
            for (iz=0; iz<numz; iz++) {
                if (Grid->array[ix][iy][iz] > val_max)
                    val_max = Grid->array[ix][iy][iz];
                if (Grid->array[ix][iy][iz] < val_min)
                    val_min = Grid->array[ix][iy][iz];
            }

    for (ix=0; ix<numx; ix++)
        for (iy=0; iy<numy; iy++)
            for (iz=0; iz<numz; iz++) {
                Grid->array[ix][iy][iz] = Grid->array[ix][iy][iz] / val_max;
                //Grid->array[ix][iy][iz] = (Grid->array[ix][iy][iz] - val_min) / (val_max - val_min);
            }

    printlog(">>> testFullNormalizationAJL: val_min = %f val_max = %f\n", val_min, val_max);

}


/*
   SearchEdt: computes the edt map associated to the event evid (Grid[evid])
   and returns the maximum probability value over the grid, using all the
   picks associated to evid ("true" edt) and the stations not associated
   (conditional edt)
*/
float SearchEdt (GridDesc *Grid, struct Pick *pick, struct Station *station, GridDesc *Pgrid, GridDesc *Sgrid, int evid, struct Control *params, int writeToDisk, int *imean)
{
    int ix, iy, iz;
    int numx, numy, numz;
    int nevaluated;

    double prob_max;
    double f_prob_max;

    char suffix[80];

    Vect3D ml_hypo; //maximum likelihood hypocenter

    if ( params->sum )
        sprintf (Grid->chr_type, "RTLOC_SUM_%05.2f_%2.2f", tnow, sigma);
    else
        sprintf (Grid->chr_type, "RTLOC_MUL_%05.2f_%2.2f", tnow, sigma);


    imean[0]=imean[1]=imean[2]=0;
    prob_max=0.0;
    f_prob_max = 0.0;

    /* Call to grid search algorithm */
    if (params->search_type == SEARCH_GRID) {
        f_prob_max = GridSearch(Grid, pick, station, Pgrid, Sgrid,
                                evid, params, &nevaluated, &prob_max, &ml_hypo);
    }
    else if (params->search_type == SEARCH_OCTTREE) {
        f_prob_max = OctTreeSearch(Grid, pick, station, Pgrid, Sgrid,
                                   evid, params, &nevaluated, &prob_max, &ml_hypo);
    }

    printlog("prob_max = %f nevaluated = %d f_prob_max = %f\n", prob_max, nevaluated, f_prob_max);

    numx = Grid->numx;
    numy = Grid->numy;
    numz = Grid->numz;

    if ( params->renorm ) {
        for (ix=0; ix<numx; ix++)
        for (iy=0; iy<numy; iy++)
        for (iz=0; iz<numz; iz++)
            Grid->array[ix][iy][iz] /= f_prob_max;
    }

// AJL - 20070116
//testFullNormalizationAJL(Grid);
// - AJL

    //Write grid to disk
    if ( writeToDisk ) {
        sprintf (suffix, "ev%2.2d.%05.2f", evid, tnow);
        WriteGrid3dBuf(Grid, NULL, params->outfilename, suffix);
        fprintf (stderr, "Finished! Output basename: %s.%s\n", params->outfilename, suffix);

    }

    // Compute location statistics
    LocStat (Grid, f_prob_max, Pgrid, Sgrid, &ml_hypo, station, evid, pick, params, writeToDisk, imean);
    printlog("(%4.2f) imean = (%d,%d,%d)\n", tnow, imean[0], imean[1], imean[2]);

    return f_prob_max;
}


/**
   Grid Search
 */
double GridSearch (GridDesc *Grid, struct Pick *pick, struct Station *station,
           GridDesc *Pgrid, GridDesc *Sgrid, int evid,
           struct Control *params, int *nevaluated,
           double *prob_max, Vect3D *ml_hypo)
{
    int ix, iy, iz;
    int numx, numy, numz;

    int nsta;
    int npick;

    double prob;
    double f_prob_max = 0.0;
    double Pmax; //theoretical max probability density

    double xloc, yloc, zloc;

    int interpolate;
    double total_weight; //weighting factor for normalizing edt
    int ntriggered;    // AJL - 20070117

    interpolate = Grid->dx <= Pgrid->dx;    // may be too simple a test, check y, z ?

    nsta = params->nsta;
    npick = params->npick;

    numx = Grid->numx;
    numy = Grid->numy;
    numz = Grid->numz;

    xloc = Grid->origx;    // grid centered
    //xloc = Grid->origx + Grid->dx / 2.0;    // cell centered
    for (ix=0; ix<numx; ix++) {

        yloc = Grid->origy;    // grid centered
        //yloc = Grid->origy + Grid->dy / 2.0;    // cell centered
        for (iy=0; iy<numy; iy++) {

            zloc = Grid->origz;    // grid centered
            //zloc = Grid->origz + Grid->dz / 2.0;    // cell centered
            for (iz=0; iz<numz; iz++) {

                prob = calcEDTProb (pick, station, 
                            Pgrid, Sgrid, evid, xloc,
                            yloc, zloc, interpolate,
                            &ntriggered,
                            nevaluated, prob_max,
                            &total_weight, ml_hypo, params, &Pmax);

                /* Write the result to the output grid... */
                if (params->pow) {
            //Grid->array[ix][iy][iz] = exp(-nsta * (1.0 - normalize (prob, nevaluated)));
                    Grid->array[ix][iy][iz] = pow (normalize (prob, *nevaluated), nsta);
                    f_prob_max = pow (normalize (*prob_max, *nevaluated), nsta);
                }
                else {
                    Grid->array[ix][iy][iz] = normalize (prob, *nevaluated);
                    f_prob_max = normalize (*prob_max, *nevaluated);
                }

                zloc += Grid->dz;

            }

            yloc += Grid->dy;

        }

        xloc += Grid->dx;

    }    // end of loop over grid points

    return(f_prob_max);

}


/**
   Calculate EDT probability
 *
 *    interpolate - use interpolatation in reading time grid, otherwise,
 *        use time at largest grid node less than x,y,z (floor)
 *
 */
INLINE double calcEDTProb (struct Pick *pick, struct Station *station,
               GridDesc *Pgrid, GridDesc *Sgrid, int evid,
               double xloc, double yloc, double zloc,
               int interpolate, int *ntriggered,
               int *nevaluated, double *prob_max,
               double *total_weight, Vect3D *ml_hypo,
               struct Control *params, double *Pmax)
{

    int n, m;
    //int triggered;
    int statid;
    double weight;
    double ttA, ttB;
    double edt;
    double prob;

    int npick;
    int nsta;

    npick=params->npick;
    nsta=params->nsta;


    *Pmax = prob = (double) edt_null; //così dimentico tutto quello fatto agli step precedenti! forse posso cambiare

    *ntriggered = 0;    // AJL 20070117
    *nevaluated=0;
    *total_weight = 0.0;    // AJL 20070116 - weighting factor for normalizing edt

    // AJL 20070116 - Bug fix?  !!!Claudio: Is this now correct?
    //for (n=0; n<npick; n++) if (pick[n].evid == evid) {
    //    (*ntriggered)++;
    //}
    // - AJL

    /*First loop on associated picks, coupled with...*/
    for (n=0; n<npick; n++) if (pick[n].evid == evid) {
        // AJL 20070110
        //ttA = Read4dBuf (Ptt, ix, iy, numy, iz, numz, pick[n].statid, nsta);
        //ttA = ReadGrid3dValue (NULL, ix, iy, iz, &(Pgrid[pick[n].statid]));
        if (interpolate)
            ttA = ReadAbsInterpGrid3d(NULL, &(Pgrid[pick[n].statid]), xloc, yloc, zloc);
        else
            ttA = ReadAbsGrid3dValue(NULL, &(Pgrid[pick[n].statid]), xloc, yloc, zloc, 1);
        // -AJL
        //if (n == 0)    // AJL 20070116 - Bug fix?  !!!Claudio: Is this now correct?
            *ntriggered = 1;


        /*...a second loop on associated picks...*/
#if 1
        for (m=n; m<npick; m++) if (pick[m].evid == evid) {
            // AJL 20070110
            //ttB = Read4dBuf (Ptt, ix, iy, numy, iz, numz, pick[m].statid, nsta);
            //ttB = ReadGrid3dValue (NULL, ix, iy, iz, &(Pgrid[pick[m].statid]));
            if (interpolate)
                ttB = ReadAbsInterpGrid3d(NULL, &(Pgrid[pick[m].statid]), xloc, yloc, zloc);
            else
                ttB = ReadAbsGrid3dValue(NULL, &(Pgrid[pick[m].statid]), xloc, yloc, zloc, 1);
            // -AJL
            edt = do_edt (ttA, ttB, pick[n].time, pick[m].time);
            (*nevaluated)++;
            *total_weight += 1.0;    // AJL 20070116
            //if (n == 0)    // AJL 20070116 - Bug fix?  !!!Claudio: Is this now correct?
            (*ntriggered)++;

            if ( params->sum ) {
                prob += edt;
                *Pmax += 1;
            }
            else {
                prob *= edt;
                *Pmax *= 1;
                if (!prob) break; // The system of equations is not fulfilled
            }
            if ( prob > *prob_max ) {
                *prob_max = prob;
                ml_hypo->x = xloc;
                ml_hypo->y = yloc;
                ml_hypo->z = zloc;
            }
        }
#endif

#if USE_NOT_YET_TRIGGERED==1
        /*...and/or a loop over not associated stations*/
        for (statid=0; statid<nsta; statid++) if (station[statid].evid[evid] != 1) {
            // AJL 20070110
            //ttB = Read4dBuf (Ptt, ix, iy, numy, iz, numz, statid, nsta);
            //ttB = ReadGrid3dValue (NULL, ix, iy, iz, &(Pgrid[statid]));
            if (interpolate)
                ttB = ReadAbsInterpGrid3d(NULL, &(Pgrid[statid]), xloc, yloc, zloc);
            else
                ttB = ReadAbsGrid3dValue(NULL, &(Pgrid[statid]),  xloc, yloc, zloc, 1);
            // -AJL
            edt = do_edt (ttA, ttB, pick[n].time, 10000);
            (*nevaluated)++;

            weight = 1.0;
            if ( *ntriggered >= 3 ) //was 3 c.s.
                weight = exp (-(*ntriggered - 3));
            *total_weight += weight;    // AJL 20070116

            if ( params->sum ) {
                prob += weight * edt;
                *Pmax += weight;
            }
            else {
                prob *= edt;
                *Pmax *= 1;
                if (!prob) break; // The system of equations is not fulfilled
            }
            if ( prob > *prob_max ) {
                *prob_max = prob;
                ml_hypo->x = xloc;
                ml_hypo->y = yloc;
                ml_hypo->z = zloc;
            }
        }
#endif


    }    //end of first loop on associated picks

    *ntriggered=1;
    for (n=0; n<npick; n++) if (pick[n].evid == evid) {
        (*ntriggered)++;
    }


    return(prob);

}
