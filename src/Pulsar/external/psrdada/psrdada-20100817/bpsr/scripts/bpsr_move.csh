#!/bin/csh -ef

if ( "$1" == "" ) then
  echo Please specify the UTC and the NODE to which the data will be moved
  exit -1
endif

if ( "$2" == "" ) then
  echo Please specify the NODE to which the data will be moved
  exit -1
endif

set UTC=$1
set NODE=$2
set ARCHIVES=/nfs/archives/bpsr

cd /lfs/data0/bpsr/archives
set FROM=$UTC/??

if ( ! -d $UTC ) then
  echo $PWD/$UTC does not exist
  exit -1
endif

set TO=/nfs/apsr${NODE}/bpsr/archives

if ( ! -d $TO ) then
  sleep 10
  if ( ! -d $TO ) then
    echo Directory $TO does not exit
    exit -1
  endif
endif

echo "This script will move $FROM to $TO"

if ( "$3" != "force" ) then
  echo "Press <Enter> to continue or <Ctrl-C> to abort"
  $<
endif

mkdir -p $TO/$FROM

ln -sf $TO/$FROM $ARCHIVES/$UTC

cd $UTC

rsync -av --progress --bwlimit=4096 ?? apsr${NODE}:/lfs/data0/bpsr/archives/$UTC

cd ..

rm -rf $FROM

