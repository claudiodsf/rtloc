# @file rtloc_cmap.py
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

from matplotlib.colors import LinearSegmentedColormap

cdict = {
    'red': (
        (0.0, 235/255., 235/255.),
        (0.1, 233/255., 233/255.),
        (0.2, 231/255., 231/255.),
        (0.3, 228/255., 228/255.),
        (0.4, 226/255., 226/255.),
        (0.5, 224/255., 224/255.),
        (0.6, 222/255., 222/255.),
        (0.7, 220/255., 220/255.),
        (0.8, 217/255., 217/255.),
        (0.9, 215/255., 215/255.),
        (1.0, 213/255., 213/255.),
    ),
    'green': (
        (0.0, 222/255., 222/255.),
        (0.1, 208/255., 208/255.),
        (0.2, 193/255., 193/255.),
        (0.3, 179/255., 179/255.),
        (0.4, 164/255., 164/255.),
        (0.5, 150/255., 150/255.),
        (0.6, 136/255., 136/255.),
        (0.7, 121/255., 121/255.),
        (0.8, 107/255., 107/255.),
        (0.9, 92/255., 92/255.),
        (1.0, 78/255., 78/255.),
    ),
    'blue': (
        (0.0, 175/255., 175/255.),
        (0.1, 158/255., 158/255.),
        (0.2, 141/255., 141/255.),
        (0.3, 124/255., 124/255.),
        (0.4, 107/255., 107/255.),
        (0.5, 91/255., 91/255.),
        (0.6, 74/255., 74/255.),
        (0.7, 57/255., 57/255.),
        (0.8, 40/255., 40/255.),
        (0.9, 23/255., 23/255.),
        (1.0, 6/255., 6/255.),
    )
}
rtloc_cmap = LinearSegmentedColormap('rtloc_colormap', cdict, 256)
