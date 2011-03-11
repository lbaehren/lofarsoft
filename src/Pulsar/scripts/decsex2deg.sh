#!/bin/bash
#
if [ $# -eq 0 ]; then
 exit
else
 dec=$1
fi
d=`echo $dec | cut -d : -f 1`
is_minus=`echo $d | grep - | wc -l`
if [ $is_minus -eq 0 ]; then
 sign=1.
else
 sign=-1.
 d=`echo $d | cut -d - -f 2`
fi
m=`echo $dec | cut -d : -f 2`
s=`echo $dec | cut -d : -f 3`
deg=`echo "($d+($m+$s/60.)/60.) * $sign" | bc -l | awk '{printf "%.8f\n", $1}' -`
echo $deg
