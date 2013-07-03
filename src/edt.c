/*
 * @file edt.c equal differential time calculation
 * 
 * Copyright (C) 2006-2013 Claudio Satriano <satriano@ipgp.fr>
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

INLINE double do_edt (double tta, double ttb, double ta, double tb)
{
    double edt;
    double tt1, tt2;
    double t1, t2;
    double sigma2;

    sigma2 = sigma * sigma;

    /* Pick order is not important */

    if (ta <= tb) {
        tt1 = tta;
        tt2 = ttb;
        t1 = ta;
        t2 = tb;
    }
    else {
        tt1 = ttb;
        tt2 = tta;
        t1 = tb;
        t2 = ta;
    }

    if (t1 <= tnow) {
        if (t2 <= tnow) { //if both stations triggered
            //true edt
            edt = exp (-1.0 * pow( ((tt2 - tt1) - (t2 - t1)) , 2.0) / (2.0 * sigma2) );
            //if ( fabs(edt) < sigma ) return 1;
            //else return 0;
            return edt;
        }
        else {
            //conditional edt
            edt = (tt2 - tt1) - (tnow - t1);
            //edt = exp (-1 * pow( ((tt2 - tt1) - (tnow - t1)) ,2) / sigma2);
            //if ( edt > -sigma / 2.0 ) return 1;
            if ( edt > 0.0 ) return 1.0;
            else return 0.0;
        }
    }


    /* If no station has yet triggered, simply do nothing */
    return (double) edt_null;
}


INLINE double normalize (double edt, int nsta)
{
    double fedt;

    fedt = (double) edt;

    return (fedt / (double) nsta);
}
