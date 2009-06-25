#!/bin/bash
#

mkdir /home/veen/data/hdf5data/tue23062009
DireName="/home/veen/data/hdf5data/tue23062009/"

RCUNumber=0

#while :
# do
ssh CS001C tbbctl --free
sleep 0.2
ssh CS001C tbbctl --alloc
sleep 0.2
ssh CS001C tbbctl --rec
sleep 1.0
ssh CS001C tbbctl --stop

HourTag=$(date +%H)
((HourTag=HourTag+2))

MinuteTag=$(date +%M)
SecondsTag=$(date +%S)

FileName=CS001C-RCU"$RCUNumber"T"$HourTag":"$MinuteTag":"$SecondsTag".h5
FullName="$DireName""$FileName"

if [ "$RCUNumber" -le 15 ]
 then
nohup /bin/bash -e | ssh 10.181.0.1 /app64/usg/bin/tbb2h5 --ip 10.151.1.1 --port 31664 \
 --outfile "$FullName" & >> /dev/null
 else
nohup /bin/bash -e | ssh 10.181.0.1 /app64/usg/bin/tbb2h5 --ip 10.151.1.1 --port 31665 \
 --outfile "$FullName" & >> /dev/null
fi

ssh CS001C --readall=2000 --select="$RCUNumber"

#done 
