#!/bin/bash

echo Running feedfile.py script, passing to VHECRtest:
echo file $1
echo antenna selection $2
echo nof channels $3
echo fit type $4
echo station name $5
echo antenna pos file $5\-AntennaArrays.conf
./test/feedfile.py $1 | VHECRtest -S $2 -C $3 -D $4 -P /Users/acorstanje/usg/data/calibration/AntennaPos/$5\-AntennaArrays.conf

