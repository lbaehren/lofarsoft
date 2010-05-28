#!/bin/bash

echo Starting live feed from $1 using file dated $2. User name is $3
ssh $3@portal.lofar.eu "ssh lfe001 \"ssh $1\C tail -fn 10000 /opt/lofar/log/$2_TRIGGER.dat\"" | VHECRtest

