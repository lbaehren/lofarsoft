#!/bin/bash

# Parameters are:
# - station name (e.g. RS205)
# - date string (e.g. 2010-05-28)
# - user name to log into portal.lofar.eu

echo Starting live feed from $1 using file dated $2. User name is $3
ssh $3@portal.lofar.eu "ssh lfe001 \"ssh $1\C tail -fn 10000 /opt/lofar/log/$2_TRIGGER.dat\"" | VHECRtest

