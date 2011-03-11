#!/bin/bash
#
if [ $# -eq 0 ]; then
 exit
else
 ra=$1
fi
hh=`echo $ra | cut -d : -f 1`
mm=`echo $ra | cut -d : -f 2`
ss=`echo $ra | cut -d : -f 3`
deg=`echo "15.*($hh+($mm+$ss/60.)/60.)" | bc -l | awk '{printf "%.8f\n", $1}' -`
echo $deg
