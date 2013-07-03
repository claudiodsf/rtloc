/*
 * @file GetRms.c event RMS computation
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

float GetRms (int *imean, GridDesc *Pgrid, GridDesc *Sgrid, struct Pick *pick, int pickid, GridDesc *Grid, struct Control *params)
{
    int n, m=0;
    float tta, ttb;
    float timea, timeb;
    float rms = 0;
    int evid;

    evid = pick[pickid].evid;

    timea = pick[pickid].time;
    // AJL 20070110
    //tta = Read4dBuf (Ptt, imean[0], imean[1], Grid->numy, imean[2], Grid->numz, pick[pickid].statid, params->nsta);
    tta = ReadGrid3dValue (NULL, imean[0], imean[1], imean[2], &(Pgrid[pick[pickid].statid]));
    // -AJL


    for (n=0; n<params->npick; n++) {
        //If pick is not associated and it's not the reference pick...
        if ( (pick[n].evid == evid) && (n != pickid) ) {
            timeb = pick[n].time;
            // AJL 20070110
            //ttb = Read4dBuf (Ptt, imean[0], imean[1], Grid->numy, imean[2], Grid->numz, pick[n].statid, params->nsta);
            ttb = ReadGrid3dValue (NULL, imean[0], imean[1], imean[2], &(Pgrid[pick[n].statid]));
            // -AJL

            rms += pow( ((tta - ttb) - (timea - timeb)), 2);

            m++;
        }
    }

    rms = sqrt (rms/m);

    return rms;
}
