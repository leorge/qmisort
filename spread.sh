#!/bin/sh
#
#   calculate estimated standard deviation of an algorithm.
#
if [ $# -le 1 ] ; then
    echo "$0 nmemb <option of Sort command>"
else
    N=$1; shift
    for i in `seq 1 10`; do
        src/random.awk $N | Release/Sort -N $N -Z 64 $* | tail -1
    sleep 1; done > spread.tmp
    awk '{print $4}' spread.tmp | xargs src/esd.awk
fi
