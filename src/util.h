/*
 * Copyright (C) 1999 Anthony Lomax <lomax@faille.unice.fr>
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


/* util.h 

   AJL utility functions.

*/


/*
	by Anthony Lomax
	Geosciences Azur, Valbonne, France
*/
#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <stdio.h>

#ifndef MAXLINE
#define MAXLINE 101
#endif

#ifndef VERY_SMALL_DOUBLE
#define VERY_SMALL_DOUBLE 1.0e-30
#endif

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)

extern char package_name[MAXLINE];
extern char prog_name[MAXLINE];
extern char prog_ver[MAXLINE];
extern char prog_date[MAXLINE];
extern char prog_copyright[MAXLINE];
extern int message_flag;
extern char MsgStr[10 * MAXLINE];


/*** function to display command usage */
void disp_usage(char* , char*);

/*** function to display error message */
void puterr(const char *);

/*** function to display error message */
void puterr2(const char *, const char *);

/*** function to display message */
void putmsg(int, const char *);

/*** function to display program information */
void DispProgInfo();

/*** function to check that int val is in range */
int checkRangeInt(const char* name, const char* param, int val, 
	int checkMin, int min, int checkMax, int max);

/*** function to check that double val is in range */
int checkRangeDouble(const char* name, const char* param, double val, 
	int checkMin, double min, int checkMax, double max);

#endif //UTIL_H
