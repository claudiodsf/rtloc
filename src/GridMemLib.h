/*
 * Copyright (C) 1999-2000 Anthony Lomax <anthony@alomax.net, http://www.alomax.net>
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
#ifndef GRIDMEMLIB_H
#define GRIDMEMLIB_H

/*------------------------------------------------------------*/
/** grid memory management routines */

typedef struct gridMem {    /* 3D grid data in memory */

    GridDesc* pgrid;    /* pointer to copy of grid description structure */
    float* buffer;        /* corresponding buffer (contiguous floats) */
    float*** array;        /* corresponding array access to buffer */
    int grid_read;        /* gread read flag  = 1 if grid has been read from disk */
    int active;        /* active flag  = 1 if grid is being used in current location */


} GridMemStruct;

/* array of gridMem structure pointers for storing list of grids in memory */
extern GridMemStruct** GridMemList;
extern int GridMemListSize;
extern int GridMemListNumElements;
extern int Num3DGridReadToMemory, MaxNum3DGridMemory;

/* GridLib wrapper functions */
float* NLL_AllocateGrid(GridDesc* pgrid);
void NLL_FreeGrid(GridDesc* pgrid);
float*** NLL_CreateGridArray(GridDesc* pgrid);
void NLL_DestroyGridArray(GridDesc* pgrid);
int NLL_ReadGrid3dBuf(GridDesc* pgrid, FILE* fpio);
GridMemStruct* GridMemList_AddGridDesc(GridDesc* pgrid);
void GridMemList_AddElement(GridMemStruct* pnewGridMemStruct);
void GridMemList_RemoveElementAt(int index);
GridMemStruct* GridMemList_ElementAt(int index);
int GridMemList_IndexOfGridDesc(int verbose, GridDesc* pgrid);
int GridMemList_NumElements();


/** end of grid memory management routines */
/*------------------------------------------------------------*/
#endif //GRIDMEMLIB_H
