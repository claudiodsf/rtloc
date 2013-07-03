/*
 * @file Read4dBuf.c utility for reading a 4d buffer file
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

float Read4dBuf (float *buf, int ix, int iy, int numy, int iz, int numz, int is, int nums)
{
    int offset;

    offset = nums*numz*numy*ix + nums*numz*iy + nums*iz + is;

    return buf[offset];
}
