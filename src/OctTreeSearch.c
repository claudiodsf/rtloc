/*
 * @file OctTreeSearch.c octree implementation for RTLoc
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

//extern float tnow;
//extern float sigma;
//extern int edt_null;

// Octtree
//#include "octtree.h"
Tree3D* octTree;        // the Octtree
ResultTreeNode* resultTreeRoot;    // Octtree likelihood*volume results tree root node
#define OCTREE_UNDEF_VALUE -VERY_SMALL_DOUBLE

Tree3D*  InitializeOcttree(GridDesc* ptgrid, OcttreeParams* octtreeParams);




/** function to perform Octree core solution evaluation */


INLINE long double LocOctree_core(double xval, double yval, double zval,
                 struct Pick *pick, struct Station *station, 
                 GridDesc *Pgrid, GridDesc *Sgrid, int evid,
                 OctNode* poct_node,
                 double *misfit, double log_normalization,
                 int *nevaluated,
                 double *prob_max, int interpolate,
                 double *total_weight, double *f_prob_max,
                 Vect3D *ml_hypo, struct Control *params)
{

    long double prob;
    long double log_prob;
    double Pmax; //theoretical max probability density

    double volume, log_value_volume;
    double dsx, dsy, dsz;

    int ntriggered;    // AJL - 20070117

    int nsta;

    nsta=params->nsta;

    // calculate cell volume
    dsx = poct_node->ds.x;
    dsy = poct_node->ds.y;
    dsz = poct_node->ds.z;
    volume = dsx * dsy * dsz;

    // calc misfit and prob density
    prob = calcEDTProb (pick, station, Pgrid, Sgrid,
                evid, xval, yval, zval, interpolate, 
                &ntriggered, nevaluated, prob_max, total_weight,
                ml_hypo, params, &Pmax);

    if (ntriggered < 1)
        ntriggered = 1;

    // rtloc power and normalization
    if (params->pow) {
        //Grid->array[ix][iy][iz] = exp(-nsta * (1.0 - normalize (prob, nevaluated)));
        // AJL - 20070116 - 20070117
        //prob = pow (normalize (prob, *nevaluated), nsta);
#if USE_NOT_YET_TRIGGERED==1
        prob = pow (normalize (prob, *total_weight), nsta);
        Pmax = pow (normalize (Pmax, *total_weight), nsta);
#else
        prob = pow (normalize (prob, *total_weight), ntriggered); //c.s. 20090113
        Pmax = pow (normalize (Pmax, *total_weight), ntriggered);
#endif

        //prob = pow (normalize (prob, *total_weight), ntriggered);
        //*f_prob_max = pow (normalize (*prob_max, *nevaluated), nsta); // TODO: move to calling function
        *f_prob_max = pow (normalize (*prob_max, *total_weight), nsta); // TODO: move to calling function
        //*f_prob_max = pow (normalize (*prob_max, *total_weight), ntriggered); // TODO: move to calling function
        // - AJL
    }
    else {
        prob = normalize (prob, *nevaluated);
        *f_prob_max = normalize (*prob_max, *nevaluated); // TODO: move to calling function
    }

    log_prob = log(prob);
    // calculate cell volume * prob density and put cell in resultTree
    log_value_volume =  log_prob + log(volume);    // used only for sorting
    poct_node->value = prob;

//printf("addResult(resultTreeRoot %ld, log_value_volume %lf, volume %lf, poct_node %ld\n",
//resultTreeRoot, log_value_volume, volume, poct_node);
    resultTreeRoot = addResult(resultTreeRoot, log_value_volume, volume, poct_node);

    return(log_prob);

}




/** function to perform Octree location */

double LocOctree(GridDesc *Grid, struct Pick *pick, int npick,
         struct Station *station, int  nsta,
         GridDesc *Pgrid, GridDesc *Sgrid, HypoDesc* phypo,
         OcttreeParams *octtreeParams, Tree3D* pOctTree,
         int evid, int *nevaluated,
         double *prob_max, double *f_prob_max,
         double *poct_node_value_max, double *poct_tree_integral,
         Vect3D *ml_hypo, struct Control *params)
{

    int nSamples, ipos;
    int nInitial;

    int ix, iy, iz;
    double xval, yval, zval;

    double min_node_size_x;
    double min_node_size_y;
    double min_node_size_z;
    double smallest_node_size_x = -1.0;
    double smallest_node_size_y = -1.0;
    double smallest_node_size_z = -1.0;

    OctNode* poct_node;
    OctNode* pparent_oct_node;
    ResultTreeNode* presult_node;

    int n_neigh;
    OctNode* neighbor_node;
    Vect3D coords;

    double hypo_dx = -1.0, hypo_dz = -1.0;

    double log_prob;
    double misfit = VERY_LARGE_DOUBLE;    // not used in rtloc

    double log_normalization = 0.0;    // norm = 1.0

    long double log_prob_max = (long double) -VERY_LARGE_DOUBLE;
    double misfit_min = VERY_LARGE_DOUBLE, misfit_max = -VERY_LARGE_DOUBLE;

    int interpolate = 1;
    double total_weight;


    // set min node size (in km)
    min_node_size_x = min_node_size_y = min_node_size_z = octtreeParams->min_node_size;

    // first get solutions at each cell in Tree3D

    nSamples = 0;
    resultTreeRoot = NULL;
    for (ix = 0; ix < pOctTree->numx; ix++) {
        for (iy = 0; iy < pOctTree->numy; iy++) {
            for (iz = 0; iz < pOctTree->numz; iz++) {
                poct_node = pOctTree->nodeArray[ix][iy][iz];
// $$$ NOTE: this block must be identical to block $$$ below
                xval = poct_node->center.x;
                yval = poct_node->center.y;
                zval = poct_node->center.z;
                interpolate = poct_node->ds.x < 2.0 * Pgrid->dx; // may be too simple a test, check y, z ?
                log_prob = LocOctree_core(xval, yval, zval,
                        pick, station, 
                        Pgrid, Sgrid, evid, poct_node,
                        &misfit, log_normalization,
                        nevaluated,
                        prob_max, interpolate,
                        &total_weight, f_prob_max,
                        ml_hypo, params);
                nSamples++;
// END - this block must be identical to block $$$ below

                // save node size
                smallest_node_size_x = poct_node->ds.x;
                smallest_node_size_y = poct_node->ds.y;
                smallest_node_size_z = poct_node->ds.z;

                if (message_flag >= 1 && nSamples % 5000 == 0) {
                    fprintf(stdout,
                            "OctTree num samples = %d / %d\r", nSamples, octtreeParams->max_num_nodes);
                    fflush(stdout);
                }

            }
        }
    }
    nInitial = nSamples;


    // loop over octree nodes

    ipos = 0;

    while (nSamples < octtreeParams->max_num_nodes) {

        if (octtreeParams->stop_on_min_node_size) {
            presult_node = getHighestLeafValue(resultTreeRoot);
        } else {
            presult_node = getHighestLeafValueMinSize(resultTreeRoot,
                min_node_size_x, min_node_size_y, min_node_size_z);
        }
        // check if null node
        if (presult_node == NULL) {
            if (message_flag >= 1)
                fprintf(stdout, "\nINFO: No more nodes larger than min_node_size, terminating Octree search.");
            break;
        }

//if (nSamples % 100 == 0)
//fprintf(stderr, "%d getHighestLeafValue %lf\n", nSamples, presult_node->value);

        if (presult_node == NULL)
            fprintf(stderr, "\npresult_node == NULL!!\n");

        pparent_oct_node = presult_node->pnode;

        // subdivide all HighestLeafValue neighbors

        for (n_neigh = 0; n_neigh < 7; n_neigh++) {

            if (n_neigh == 0) {
                neighbor_node = pparent_oct_node;
            } else {
                coords.x = pparent_oct_node->center.x;
                coords.y = pparent_oct_node->center.y;
                coords.z = pparent_oct_node->center.z;
                if (n_neigh == 1) {
                    coords.x = pparent_oct_node->center.x
                            + (pparent_oct_node->ds.x + smallest_node_size_x) / 2.0;
                } else if (n_neigh == 2) {
                    coords.x = pparent_oct_node->center.x
                            - (pparent_oct_node->ds.x + smallest_node_size_x) / 2.0;
                } else if (n_neigh == 3) {
                    coords.y = pparent_oct_node->center.y
                            + (pparent_oct_node->ds.y + smallest_node_size_y) / 2.0;
                } else if (n_neigh == 4) {
                    coords.y = pparent_oct_node->center.y
                            - (pparent_oct_node->ds.y + smallest_node_size_y) / 2.0;
                } else if (n_neigh == 5) {
                    coords.z = pparent_oct_node->center.z
                            + (pparent_oct_node->ds.z + smallest_node_size_z) / 2.0;
                } else if (n_neigh == 6) {
                    coords.z = pparent_oct_node->center.z
                            - (pparent_oct_node->ds.z + smallest_node_size_z) / 2.0;
                }

                // find neighbor node
                neighbor_node = getLeafNodeContaining(pOctTree, coords);
                // outside of octTree volume
                if (neighbor_node == NULL)
                    continue;
                // already subdivided
                if (neighbor_node->ds.x < 0.99 * pparent_oct_node->ds.x)
                    continue;
            }


            // subdivide node and evaluate solution at each child
            subdivide(neighbor_node, OCTREE_UNDEF_VALUE, NULL);

            for (ix = 0; ix < 2; ix++) {
                for (iy = 0; iy < 2; iy++) {
                    for (iz = 0; iz < 2; iz++) {

                        poct_node = neighbor_node->child[ix][iy][iz];

//if (poct_node->ds.x < octtreeParams->min_node_size || poct_node->ds.y < octtreeParams->min_node_size || poct_node->ds.z < octtreeParams->min_node_size)
//fprintf(stderr, "\nnode size too small!! %lf %lf %lf\n", poct_node->ds.x, poct_node->ds.y, poct_node->ds.z);

                        // save node size if smallest so far
                        if (poct_node->ds.x < smallest_node_size_x)
                            smallest_node_size_x = poct_node->ds.x;
                        if (poct_node->ds.y < smallest_node_size_y)
                            smallest_node_size_y = poct_node->ds.y;
                        if (poct_node->ds.z < smallest_node_size_z)
                            smallest_node_size_z = poct_node->ds.z;

// $$$ NOTE: this block must be identical to block $$$ above
                xval = poct_node->center.x;
                yval = poct_node->center.y;
                zval = poct_node->center.z;
                interpolate = poct_node->ds.x < 2.0 * Pgrid->dx; // may be too simple a test, check y, z ?
                log_prob = LocOctree_core(xval, yval, zval,
                        pick, station,
                        Pgrid, Sgrid, evid, poct_node,
                        &misfit, log_normalization,
                        nevaluated, prob_max,
                        interpolate, &total_weight, f_prob_max,
                        ml_hypo, params);
                nSamples++;
// END - this block must be identical to block $$$ above

                        if (message_flag >= 1 && nSamples % 5000 == 0) {
                            fprintf(stdout,    "OctTree num samples = %d / %d\r", nSamples, octtreeParams->max_num_nodes);
                            fflush(stdout);
                        }

                        // check for maximum likelihood
                        if (log_prob > log_prob_max) {
                            log_prob_max = log_prob;
                            misfit_min = misfit;
                            phypo->misfit = misfit;
                            phypo->x = xval;
                            phypo->y = yval;
                            phypo->z = zval;
                            hypo_dx = poct_node->ds.x;
                            hypo_dz = poct_node->ds.z;
                            *poct_node_value_max = poct_node->value;
                            //*f_prob_max = pow (normalize (*prob_max, *nevaluated), nsta); // moved to calling function
                            //*f_prob_max = pow (normalize (*prob_max, total_weight), nsta); // moved to calling function
                        }
                        if (misfit > misfit_max)
                            misfit_max = misfit;


                    } // end triple loop over node children
                }
            }

        }  // end loop over HighestLeafValue neighbors

        // check if minimum node size reached
        if (octtreeParams->stop_on_min_node_size && (smallest_node_size_x < min_node_size_x
                  || smallest_node_size_y < min_node_size_y
                  || smallest_node_size_z < min_node_size_z)) {
            if (message_flag >= 1)
                fprintf(stdout, "\nINFO: Min node size reached, terminating Octree search.");
            break;
                  }

    }  // end while (nSamples < octtreeParams->max_num_nodes)

    if (message_flag >= 1)
        fprintf(stdout, "\n");

//printf("HYPOCENTER  x %lf y %lf z %lf  OT %lf  ix %d iy %d iz %d\n",
//            phypo->x, phypo->y, phypo->z, (double) phypo->sec,
//            phypo->ix, phypo->iy, phypo->iz);
//printf("HYPOCENTER  x %lf y %lf z %lf\n",
//            phypo->x, phypo->y, phypo->z);

    // check reject location conditions

    /*
    // maximum like hypo on edge of grid
    if ((iBoundary = isOnGridBoundary(phypo->x, phypo->y, phypo->z, ptgrid, hypo_dx, hypo_dz, 0))) {
    sprintf(MsgStr,
    "WARNING: max prob location on grid boundary %d, rejecting location.", iBoundary);
    printlog("%s", MsgStr);
    sprintf(phypo->locStatComm, "%s", MsgStr);
    iReject = 1;
}
    */

    // determine integral of all oct-tree leaf node pdf values
    /*
    *poct_tree_integral = integrateResultTree(resultTreeRoot, 0.0, *poct_node_value_max);
    sprintf(MsgStr, "Octree oct_node_value_max= %le oct_tree_integral= %le", *poct_node_value_max, *poct_tree_integral);
    printlog("%s", MsgStr);
    */



    // construct search information string
    sprintf(phypo->searchInfo, "OCTREE nInitial %d nSamples %d smallestNodeSide %lf/%lf/%lf oct_tree_integral %le",
        nInitial, nSamples, smallest_node_size_x, smallest_node_size_y, smallest_node_size_z, *poct_tree_integral);
    // write message
    printlog("%s\n", phypo->searchInfo);


    /*
    // check for termination
    if (iReject) {
    sprintf(Hypocenter.locStat, "REJECTED");
}
    */


    return(*f_prob_max);

}





/**
   OctTree Search
 */
double OctTreeSearch (GridDesc *Grid, struct Pick *pick,
              struct Station *station,
              GridDesc *Pgrid, GridDesc *Sgrid,
              int evid, struct Control *params, int *nevaluated,
              double *prob_max, Vect3D *ml_hypo)
{

    double f_prob_max = 0.0; // init to zero - CS 2009-05-12
    double dprob_max = 0; // init to zero - CS 2009-05-12
    double oct_tree_integral;
    double oct_node_value_max;

    Vect3D *ml_hypo2;
    ml_hypo2 = ml_hypo;


    // AJL
    Tree3D* pOctTree; //claudio
    HypoDesc Hypo;


    pOctTree = InitializeOcttree(Grid, &(params->octtreeParams)); //claudio

    // do octTree search
    f_prob_max = LocOctree(Grid, pick, params->npick, station, params->nsta,
                   Pgrid, Sgrid, &Hypo, &(params->octtreeParams),
                   pOctTree, evid, nevaluated,
                   &dprob_max, &f_prob_max, &oct_node_value_max,
                   &oct_tree_integral, ml_hypo2, params);

    // convert octTree structure to grid
    ConvertOctTree2Grid(pOctTree, Grid->dx, Grid->dy, Grid->dz, Grid->chr_type, Grid);

    // free octree allocations - IMPORTANT!
    // free results tree
    freeResultTree(resultTreeRoot);
    // free octree memory
    freeTree3D(pOctTree, 1);
    NumAllocations--;


    *prob_max = dprob_max;
    return(f_prob_max);
}


/** function to initialize Octree search */

Tree3D* InitializeOcttree(GridDesc* ptgrid, OcttreeParams* octtreeParams)
{

    double dx, dy, dz;
    Tree3D* newTree;
    void *pdata = NULL;

    // set up octree x, y, z grid
    dx = ptgrid->dx * (double) (ptgrid->numx - 1) / (double) (octtreeParams->init_num_cells_x);
    dy = ptgrid->dy * (double) (ptgrid->numy - 1) / (double) (octtreeParams->init_num_cells_y);
    dz = ptgrid->dz * (double) (ptgrid->numz - 1) / (double) (octtreeParams->init_num_cells_z);

    newTree = newTree3D(ptgrid->type, octtreeParams->init_num_cells_x,
                octtreeParams->init_num_cells_y, octtreeParams->init_num_cells_z,
                ptgrid->origx, ptgrid->origy, ptgrid->origz,
                dx, dy, dz, OCTREE_UNDEF_VALUE, pdata);

    return(newTree);

}










