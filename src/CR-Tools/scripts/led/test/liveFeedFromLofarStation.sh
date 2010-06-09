#!/bin/bash

# Parameters are:
# - station name (e.g. RS205)
# - date string (e.g. 2010-05-28)
# - user name to log into portal.lofar.eu

echo Starting live feed from $1 using file dated $2. User name is $3
echo VHECRtest parameters are:
echo antenna selection $4
echo nof channels $5
echo fit type $6
echo antenna positions file $7

ssh $3@portal.lofar.eu "ssh lfe001 \"ssh $1\C tail -fn 10000 /opt/lofar/log/$2_TRIGGER.dat\"" | VHECRtest -S $4 -C $5 -D $6 -P $LOFARSOFT/usg/data/calibration/AntennaArrays/$1\-AntennaArrays.conf


