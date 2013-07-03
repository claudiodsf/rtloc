#!/bin/bash
# Copyright (C) 2013 Claudio Satriano <satriano@ipgp.fr>

function run()
{
    ../rtloc run01.rtloc.ctrl
}

function plot()
{
    ../plot/plot.py .
}

function clean()
{
    /bin/rm run01.rtloc.log run01.rtloc.stat run01.rtloc.stations
    /bin/rm -r run01.rtloc run01.pdf
}

function usage()
{
    echo "Usage: $0 run|plot|clean"
}    

if [ $# -lt 1 ]
then
    usage
    exit 1
fi

case $1 in
    "run" )
        run ;;
    "plot" )
        plot ;;
    "clean" )
        clean ;;
    * )
        usage
        exit 1 ;;
esac
