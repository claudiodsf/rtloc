# -*- coding: utf8 -*-

# @file parse_stat_file.py
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

def parse_stat_file(filename, ref_time):

    try:
        fp = open(filename, 'r')
    except IOError:
        print(f'Unable to open file: {filename}')
        return

    triggered = {}
    go = False
    for line in fp:
        word = line.split()
        if len(word) == 0:
            continue

        if word[0] == 'TIME':
            time = float(word[1])
            go = time == ref_time
            continue

        if not go:
            continue

        if word[0] == 'STA':
            sta = word[1]
            ptime = float(word[9])
            triggered[sta] = time >= ptime
        if word[0] == 'HYPOCENTER':
            hypo = (float(word[2]), float(word[4]), float(word[6]))

    fp.close()
    return triggered, hypo


if __name__ == '__main__':
    import sys
    trig, hypo = parse_stat_file(sys.argv[1], 0)
    print(trig)
    print(hypo)
