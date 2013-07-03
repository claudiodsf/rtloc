#!/usr/bin/env python
# -*- coding: utf8 -*- 

# @file plot.py main plotting routine
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

import os
from glob import glob
import numpy as np
#import matplotlib
#matplotlib.rcParams['pdf.fonttype'] = 42
from matplotlib import rc
rc('text', usetex=True)
rc('font',**{'family':'sans-serif','sans-serif':['Helvetica']})
rc('text.latex', preamble='\usepackage{sfmath}')
#import matplotlib.pyplot as plt
from matplotlib import figure
from matplotlib.backends.backend_agg import FigureCanvasAgg
from mpl_toolkits.axes_grid1 import make_axes_locatable
from NLLGrid import NLLGrid
from rtloc_cmap import rtloc_cmap
from parse_stat_file import parse_stat_file
from parse_geo_data import parse_geo_data


def plot(grd, stationfile=None, statfile=None, data_geog=None, plotdir='.'):

    Xmin, Xmax, Ymin, Ymax, Zmin, Zmax = grd.get_extent()
    ratio = (Xmax - Xmin) / (Ymax - Ymin)

    fig = figure.Figure(figsize=(8, 8))
    #fig = plt.figure(figsize=(8, 8))
    plot_xz_size = ((Zmax - Zmin)/(Xmax - Xmin))*100
    plot_zy_size = plot_xz_size / ratio
    plot_cbar_size = 5 #percent
    xz_size = '%f %%' % plot_xz_size
    zy_size = '%f %%' % plot_zy_size
    cb_size = '%f %%' % plot_cbar_size
    hypo_smbl_size = 100 / ratio
    trig_smbl_size = 600 / ratio

    time = grd.type.split('_')[2]
    time = float(time)

#--ax1: stacked grid
#--ax1_xy
    ax1_xy = fig.add_subplot(111)
    divider1 = make_axes_locatable(ax1_xy)

    xy_array = grd[:,:,:].sum(axis=2)
    xy_array /= xy_array.max()
    hnd=ax1_xy.imshow(np.flipud(np.transpose(xy_array)),
                             extent=grd.get_xy_extent(), cmap=rtloc_cmap, rasterized=True)
    ax1_xy.axis('tight')
    ax1_xy.set_xlim(Xmin,Xmax)
    ax1_xy.set_ylim(Ymin,Ymax)
    ax1_xy.set_xlabel('X[km]')
    ax1_xy.set_ylabel('Y[km]')
    ax1_xy.xaxis.set_label_position('top')
    ax1_xy.xaxis.set_ticks_position('top')

    if statfile:
        trigged, hypo = parse_stat_file(statfile, time) 
        ax1_xy.scatter(hypo[0], hypo[1], marker='*', s=hypo_smbl_size, linewidths=1, c='w')

    if stationfile:
        try:
            recordtype = np.dtype([('name', np.str_, 20), ('x', float), ('y', float), ('z', float)])
            stations = np.loadtxt(stationfile, dtype = recordtype)
            coords = {}
            for sta in stations:
                #sta_text = sta[0]
                sta_text = r'$\textbf{%s}$' % sta[0]
                ax1_xy.text(sta[1], sta[2], sta_text, fontsize=8, fontweight='bold',
                        ha='center', va='center', color='k', clip_on=True)
                coords[sta[0]] = (sta[1], sta[2])
            if statfile:
                for sta in trigged:
                    if trigged[sta] == True:
                        ax1_xy.scatter(coords[sta][0], coords[sta][1], marker='o', s=trig_smbl_size,
                                facecolors='none', edgecolors='k')
        except IOError:
            print 'Unable to open station file: ' + stationfile
            pass

    if data_geog:
        for filename in glob(os.path.join(data_geog, '*.xy')):
            segments = parse_geo_data(filename)
            for segment in segments:
                if len(segment) > 0:
                    ax1_xy.plot(segment[:,0], segment[:,1], c='k', linewidth=0.5, clip_on=True)

    #time_text = u'δt = %05.2f s' % time
    time_text = r'$\delta t = %05.2f s$' % time
    ax1_xy.text(1.05, -0.3, time_text, fontsize=15, color='k', transform=ax1_xy.transAxes)

    ax1_xy.set_aspect('equal', 'datalim')

#--ax1_zy
    ax1_zy = divider1.append_axes('right', size=zy_size, pad=0.05, sharey=ax1_xy)
    zy_array = grd[:,:,:].sum(axis=0)
    zy_array /= zy_array.max()
    ax1_zy.imshow(np.flipud(zy_array),
                     extent=grd.get_zy_extent(), cmap=rtloc_cmap, rasterized=True)

    ax1_zy.axis('tight')
    ax1_zy.set_xlim(Zmin,Zmax)
    ax1_zy.set_ylim(Ymin,Ymax)

    ax1_zy.set_xlabel('Z[km]')
    ax1_zy.set_ylabel('Y[km]')
    ax1_zy.yaxis.set_label_position('right')
    ax1_zy.yaxis.set_ticks_position('right')
    for label in ax1_zy.yaxis.get_ticklabels():
        label.set_position((1.22, 0.))
        label.set_horizontalalignment('right')

    if statfile:
        ax1_zy.scatter(hypo[2], hypo[1], marker='*', s=hypo_smbl_size, linewidths=1, c='w')

    #ax1_zy.set_aspect('equal', 'datalim')
            

#--ax1_xz
    ax1_xz = divider1.append_axes('bottom', size=xz_size, pad=0.05, sharex=ax1_xy)
    xz_array = grd[:,:,:].sum(axis=1)
    xz_array /= xz_array.max()
    ax1_xz.imshow(np.flipud(np.transpose(xz_array)),
                     extent=grd.get_xz_extent(), cmap=rtloc_cmap, rasterized=True)

    ax1_xz.axis('tight')
    ax1_xz.set_xlim(Xmin,Xmax)
    ax1_xz.set_ylim(Zmin,Zmax)
    ax1_xz.set_ylim(ax1_xz.get_ylim()[::-1])

    ax1_xz.set_xlabel('X[km]')
    ax1_xz.set_ylabel('Z[km]')

    if statfile:
        ax1_xz.scatter(hypo[0], hypo[2], marker='*', s=hypo_smbl_size, linewidths=1, c='w')

    #ax1_xz.set_aspect('equal', 'datalim')

#--ax1-color-bar
    ax1_cb = divider1.append_axes('bottom', size=cb_size, pad=0.5)
    cb1 = fig.colorbar(hnd, cax=ax1_cb, orientation='horizontal')
    cb1.solids.set_rasterized(True)
    cb1.set_label('Location Probability')

    #plt.show()
    canvas = FigureCanvasAgg(fig)
    out_fig = os.path.basename(grd.basename) + '.pdf'
    out_fig = os.path.join(plotdir, out_fig)
    if not os.path.exists(plotdir):
        os.mkdir(plotdir)
    canvas.print_figure(out_fig)
    print 'Plot saved to: ' + out_fig


if __name__ == '__main__':
    import sys
    from optparse import OptionParser

    usage = 'usage: %prog [options] rtloc_run_dir'

    parser = OptionParser(usage=usage);
    parser.add_option('-r', '--run', dest='run', action='store', default='run01',
            help='run name (default: run01)', metavar='RUN')
    parser.add_option('-p', '--plotdir', dest='plotdir', action='store', default=None,
            help='directory for storing plots (default: RUN.pdf)', metavar='DIR')

    (options, args) = parser.parse_args()

    if options.plotdir == None:
        plotdir = options.run + '.pdf'
    else:
        plotdir = options.plotdir

    if len(args) < 1:
        parser.print_usage(file=sys.stderr)
        sys.stderr.write("\tUse '-h' for help\n\n")
        sys.exit(1)

    basedir = args[0]

    stationfile = os.path.join(basedir, options.run + '.rtloc.stations') 
    statfile = os.path.join(basedir, options.run + '.rtloc.stat') 
    data_geog = os.path.join(basedir, 'data_geog')
    plotdir = os.path.join(basedir, plotdir)

    bnames = glob(os.path.join(basedir, options.run + '.rtloc', '*.hdr')) 
    bnames = [ os.path.splitext(bname)[0] for bname in bnames ]

    for bname in bnames:
        grd = NLLGrid(bname)
        plot(grd, stationfile, statfile, data_geog, plotdir)
