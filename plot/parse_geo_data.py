# -*- coding: utf8 -*- 

# @file parse_geo_data.py
# 
# Copyright (C) 2013 Claudio Satriano <satriano@ipgp.fr>
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

import numpy as np

def parse_geo_data(filename):

    try:
        fp = open(filename, 'r')
    except IOError:
        print 'Unable to open file: ' + filename
        return

    segments = []
    segment = []
    for line in fp.readlines():
        word = line.split()

        if word[0] == '>':
            segments.append(np.array(segment))
            segment = []
            continue

        try:
            segment.append((float(word[0]), float(word[1])))
        except:
            pass

    fp.close()
    return segments  
