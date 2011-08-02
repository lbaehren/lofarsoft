#!/bin/bash

echo Running feedfile.py script, passing to VHECRtest:
echo file $1
echo running time $2
echo antenna selection $3
echo nof channels $4
echo fit type $5
echo station name $6
echo antenna pos file $7\-AntennaArrays.conf
./test/feedfile.py $1 $2 | VHECRtest -S $3 -C $4 -D $5 -P $LOFARSOFT/data/lofar/StaticMetaData/AntennaArrays/$6\-AntennaArrays.conf

