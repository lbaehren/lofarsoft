#!/bin/bash

echo $1

ssh corstanje@portal.lofar.eu "ssh lfe001 \"ssh RS205C tail -fn 10000 /opt/lofar/log/$1_TRIGGER.dat\"" | VHECRtest

