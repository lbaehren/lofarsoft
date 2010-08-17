#!/bin/csh -f

if ( "$1" == "" ) then
  echo Please specify the UTC and the PID of the single-beam observation
  exit -1
endif

if ( "$2" == "" ) then
  echo Please specify the PID of the single-beam observation
  exit -1
endif

set UTC=$1
set PID=$2
set RESULTS=/nfs/results/bpsr
set ARCHIVES=/nfs/archives/bpsr

echo "This script will delete BPSR beams 02 through 13 of $UTC"
echo "and change the project ID to $PID"
echo "Press <Enter> to continue or <Ctrl-C> to abort"

$<

if ( ! -d $RESULTS/$UTC ) then
  echo $RESULTS/$UTC directory not found ... aborting.
  exit -1
endif

if ( ! -d $ARCHIVES/$UTC ) then
  echo $ARCHIVES/$UTC directory not found ... aborting.
  exit -1
endif

if ( -f $ARCHIVES/$UTC/obs.single ) then
  echo $ARCHIVES/$UTC/obs.single exists ... aborting.
  exit -1
endif

echo Deleting BPSR beams 02 through 13 of $UTC

touch $ARCHIVES/$UTC/obs.single
touch $RESULTS/$UTC/obs.single

chgrp -R $PID $ARCHIVES/$UTC
chgrp -R $PID $RESULTS/$UTC

perl -p -i -e 's/P630/'$PID'/' $ARCHIVES/$UTC/obs.info  
perl -p -i -e 's/P630/'$PID'/' $RESULTS/$UTC/obs.info
perl -p -i -e 's/P743/'$PID'/' $ARCHIVES/$UTC/obs.info  
perl -p -i -e 's/P743/'$PID'/' $RESULTS/$UTC/obs.info

foreach beam ( 02 03 04 05 06 07 08 09 10 11 12 13 )

  if ( ! -d $ARCHIVES/$UTC/$beam ) then
    echo $ARCHIVES/$UTC/$beam not found ... aborting.
    exit -1
  endif

  set node=`ls -l $ARCHIVES/$UTC/$beam | awk -F"->" '{print $2}' | awk -F/ '{print $3}'`

  echo Deleting beam $beam on node $node

  rm -rf $RESULTS/$UTC/$beam
  rm -f $ARCHIVES/$UTC/$beam

  ssh -x -f bpsr@$node "rm -rf /lfs/data0/bpsr/archives/$UTC"
    
end

