# @file Makefile gnu make makefile for RTLoc
#
# Copyright (C) 2006-2013 Claudio Satriano <satriano@ipgp.fr>
# This file is part of RTLoc.
# 
# RTLoc is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# RTLoc is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with RTLoc; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
# 

PREFIX=.
CC=g++
#CC=gcc

# Do not edit below this line!

PROGNAME=rtloc
VERSION=0.50
DATE=2013/07/03


#CFLAGS for SPARC/MOTOROLA architecture
#CFLAGS=-Wall -g -D_SWAPBYTES
# CFLAGS for INTEL architecture
#CFLAGS=-Wall -g -m32 -O3
# AJL
CFLAGS=-Wall -g -DUSE_NOT_YET_TRIGGERED=1 -DVERSION=$(VERSION) -DDATE=$(DATE)

INCLUDE=
LIB=
LDFLAGS=-lm


all: $(PROGNAME)

%.o:%.c
	$(CC) -o $@ $(CFLAGS) $(INCLUDE) -c $<

#--rtloc
# AJL 20070110 original
#SRC1=rtloc.c edt.c stat_lookup.c ReadCtrlFile.c GridLib.c geo.c util.c nrutil.c nrmatrix.c ran1.c map_project.c SearchEdt.c Read4dBuf.c initLocGrid.c GetRms.c printlog.c
# remove Read4dBuf
#SRC1=rtloc.c edt.c stat_lookup.c ReadCtrlFile.c GridLib.c geo.c util.c nrutil.c nrmatrix.c ran1.c map_project.c SearchEdt.c initLocGrid.c GetRms.c printlog.c
# OctTree
SRC1=src/rtloc.c src/edt.c src/stat_lookup.c src/ReadCtrlFile.c src/GridLib.c src/geo.c src/util.c src/nrutil.c src/nrmatrix.c src/ran1.c src/map_project.c src/SearchEdt.c src/OctTreeSearch.c src/octtree.c src/initLocGrid.c src/GetRms.c src/printlog.c src/printstat.c src/LocStat.c

OBJS1=$(patsubst %.c,%.o,$(SRC1))
PDF_C1=$(patsubst %.c,pdf/%.c.pdf,$(SRC1))

$(PROGNAME): $(OBJS1)
	$(CC) -o $@ $(LIB) $(OBJS1) $(LDFLAGS)

rtloc-pdf: $(PDF_C1)

#--

#--cropgrid
#SRC1=cropgrid.c GridLib.c geo.c util.c nrutil.c nrmatrix.c ran1.c map_project.c
#
#OBJS1=$(patsubst %.c,%.o,$(SRC1))
#PDF_C1=$(patsubst %.c,pdf/%.c.pdf,$(SRC1))
#
#cropgrid: $(OBJS1)
#	$(CC) -o $@ $(CFLAGS) $(LIB) $(OBJS1) $(LDFLAGS)
#


pdf/%.c.pdf:%.c
	a2ps -o - $< | ps2pdf -sPAPERSIZE=a4 - $@

install: $(PROGNAME)
#	cp $(PROGNAME) $(PREFIX); cp cropgrid $(PREFIX)
	cp $(PROGNAME) $(PREFIX);

clean:
	(rm -f $(OBJS1))
