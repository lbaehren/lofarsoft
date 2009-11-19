#!/bin/bash

source /users/loose/mwimager/LOFAR/installed/gnu_opt/lofarinit.sh

for (( i=0; i<3; i++ ))
do
  for (( j=0; j<12; j++ ))
  do
    ms=$(printf "/lifs%03d/pipeline/L2007_03463_SB%d.MS" \
       $(expr $j + 1) $(expr 12 "*" $i + $j))
    echo "Processing  $(basename $ms)"
    makevds $HOME/lioff.clusterdesc $ms $(basename $ms).vds
  done
done

