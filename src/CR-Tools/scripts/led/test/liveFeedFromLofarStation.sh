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

#ssh $3@portal.lofar.eu "ssh lfe001 \"ssh $1\C tail -fn 10000 /localhome/data/$2_TRIGGER.dat\"" | VHECRtest -S $4 -C $5 -D $6 -P $LOFARSOFT/data/calibration/AntennaArrays/$1\-AntennaArrays.conf
# This seems to be no longer possible, looks like someone restricted the portal access even more to prevent ssh-chaining :(

ssh $1\C tail -fn 10000 /localhome/data/$2_TRIGGER.dat | VHECRtest -S $4 -C $5 -D $6 -P $LOFARSOFT/data/calibration/AntennaArrays/$1\-AntennaArrays.conf
# This needs the user to have stuff inside .ssh/config to get direct access to the station (ask me).



