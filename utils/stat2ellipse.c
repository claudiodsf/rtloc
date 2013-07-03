/*
 * @file stat2ellipse.c
 * Parses an RTLoc stat file and outputs location
 * ellipsoid projected on horizontal or vertical plane.
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
#include "GridLib.h"
#include "GridGraphLib.h"

#define LINEBUFSIZE 400
#define PARAMSIZE 30
#define NPTS_ELLIPSE 50

void readStatFile (char *statfilename, float reftime, Vect3D *pexpect, Ellipsoid3D *pellipsoid)
{
	FILE *statfile;
	char line[LINEBUFSIZE];
	char param[PARAMSIZE];
	
	int go=0;
	int parsed=0;
	float time;


	if ((statfile = fopen (statfilename, "r")) == NULL) {
		perror (statfilename);
		exit (1);
	}


	while ( fgets (line, LINEBUFSIZE, statfile) != NULL ) {
 		if ( sscanf(line, "%s", param) < 0 ) continue;
		if ( !strncmp (param, "#", 1) || isspace(param[0]) ) continue;
		if ( !strcmp (param, "TIME") ) {
			sscanf (line, "%*s %f", &time); 
			if (time==reftime) go=1;
			else go=0;
		}
		if ( !strcmp (param, "STATISTICS") && go ) {
			sscanf (line, "%*s %*s %lf %*s %lf %*s %lf %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %lf %*s %lf %*s %lf %*s %lf %*s %lf %*s %lf %*s %lf",
				&pexpect->x, &pexpect->y, &pexpect->z,
				&pellipsoid->az1, &pellipsoid->dip1, &pellipsoid->len1,
				&pellipsoid->az2, &pellipsoid->dip2, &pellipsoid->len2,
				&pellipsoid->len3
			);
			parsed=1;
		}
	}

	if(!parsed) {
		fprintf(stderr, "Time not valid: %f\n", reftime);
		exit(1);
	}

	fclose(statfile);
}



int main (int argc, char **argv)
{
	int n;
	float reftime;
	int projection;

	Vect3D pexpect;
	Ellipsoid3D pellipsoid;

	int npts_ellipse;
	Vect3D axis1, axis2, axis3;
	Vect3D *ellArray12, *ellArray13, *ellArray23;

	Vect2D* ellipse_array;

	if ( argc < 4) {
		fprintf(stderr, "Usage: %s statfile time projection (12|13|32)\n", argv[0]);
		exit(1);
	}
	sscanf(argv[2], "%f", &reftime);
	sscanf(argv[3], "%d", &projection);
	if (projection!=12 && projection!=13 && projection!=32) {
		fprintf(stderr, "Invalid projection: %d\n", projection);
		exit(1);
	}


	SetConstants();
	SetGraphicConstants();

	readStatFile(argv[1], reftime, &pexpect, &pellipsoid);

	initEllipsiodAxes(&pellipsoid, &axis1, &axis2, &axis3);

#ifdef DEBUG
fprintf(stderr, "%f %f %f\n", 
		pexpect.x, pexpect.y, pexpect.z);
fprintf(stderr, "%f %f %f\n%f %f %f\n%f\n", 
		pellipsoid.az1, pellipsoid.dip1, pellipsoid.len1,
		pellipsoid.az2, pellipsoid.dip2, pellipsoid.len2,
		pellipsoid.len3);
fprintf(stderr, "%f %f %f\n", axis1.x, axis1.y, axis1.z);
fprintf(stderr, "%f %f %f\n", axis2.x, axis2.y, axis2.z);
fprintf(stderr, "%f %f %f\n", axis3.x, axis3.y, axis3.z);
#endif



	npts_ellipse = NPTS_ELLIPSE;
	ellipse_array = 
		(Vect2D *) malloc((size_t) npts_ellipse * sizeof(Vect2D));
	
	ellArray12 = toEllipsoid3D(axis1, axis2, pexpect, npts_ellipse);
	ellipse_array = Vect3D2To2D(
				ellArray12, ellipse_array, npts_ellipse, projection);
	for (n=0; n<npts_ellipse; n++) {
		//fprintf(stderr, "%f %f %f\n", ellArray12[n].x, ellArray12[n].y, ellArray12[n].z);
		printf("%f %f\n", ellipse_array[n].x, ellipse_array[n].y);
	}
#ifdef GNUPLOT
	printf("\n");
#else
	printf(">\n");
#endif

	ellArray13 = toEllipsoid3D(axis1, axis3, pexpect, npts_ellipse);
	ellipse_array = Vect3D2To2D(
				ellArray13, ellipse_array, npts_ellipse, projection);
	for (n=0; n<npts_ellipse; n++) {
		//fprintf(stderr, "%f %f %f\n", ellArray13[n].x, ellArray13[n].y, ellArray13[n].z);
		printf("%f %f\n", ellipse_array[n].x, ellipse_array[n].y);
	}
#ifdef GNUPLOT
	printf("\n");
#else
	printf(">\n");
#endif

	ellArray23 = toEllipsoid3D(axis2, axis3, pexpect, npts_ellipse);
	ellipse_array = Vect3D2To2D(
				ellArray23, ellipse_array, npts_ellipse, projection);
	for (n=0; n<npts_ellipse; n++) {
		//fprintf(stderr, "%f %f %f\n", ellArray23[n].x, ellArray23[n].y, ellArray23[n].z);
		printf("%f %f\n", ellipse_array[n].x, ellipse_array[n].y);
	}
#ifdef GNUPLOT
	printf("\n");
#else
	printf(">\n");
#endif

	return 0;
}
