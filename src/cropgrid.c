/* @file cropgrid.c
 *
 * cropgrid: crops a NLLoc grid
 * 
 * Copyright (C) 2006-2013 Claudio Satriano <satriano@ipgp.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */



#include "GridLib.h"

#define NX 101
#define NY 101
#define NZ 40
#define OX -50
#define OY -50
#define OZ 0

int main (int argc, char **argv)
{
    GridDesc ingrid, outgrid;
    char gridfile[100];
    FILE *inbuf, *inhdr, *outbuf, *outhdr;

    int i, j, k;
    float f;

    int xoffset, yoffset, zoffset;

    sprintf(gridfile, "%s", argv[1]);

    OpenGrid3dFile(gridfile, &inbuf, &inhdr, &ingrid, " ", NULL, 0);     
    ingrid.buffer = AllocateGrid(&ingrid);
    ingrid.array = CreateGridArray(&ingrid);
    ReadGrid3dBuf (&ingrid, inbuf);

    printf("%d %d %d\n", ingrid.numx, ingrid.numy, ingrid.numz);

    outgrid.numx = NX;
    outgrid.numy = NY;
    outgrid.numz = NZ;
    outgrid.origx = OX;
    outgrid.origy = OY;
    outgrid.origz = OZ;
    outgrid.dx = ingrid.dx;
    outgrid.dy = ingrid.dy;
    outgrid.dz = ingrid.dz;
    outgrid.type = ingrid.type;
    sprintf(outgrid.chr_type, "%s", ingrid.chr_type);
    outgrid.iSwapBytes = ingrid.iSwapBytes;

    outgrid.buffer = AllocateGrid(&outgrid);
    outgrid.array = CreateGridArray(&outgrid);

    xoffset = OX - ingrid.origx;
    yoffset = OY - ingrid.origy;
    zoffset = OZ - ingrid.origz;

    for (i=0; i<NX; i++)
    for (j=0; j<NY; j++)
    for (k=0; k<NZ; k++){
        f = outgrid.array[i][j][k] = ingrid.array[i+xoffset][j+yoffset][k+zoffset];
        //printf ("%d %d %d %f\n", i, j, k, f);
    }

    WriteGrid3dBuf(&outgrid, NULL, gridfile, "crop");

    return 0;
}
