#!/bin/bash

device="/dev/nst1"
project="P630"
prefix="HTRU"

usage()
{
cat <<EOF

Usage: lto_label.sh [OPTION]

Known values for OPTION are:

  --device=TAPE      tape drive device name [default: $device]
  --project=PID      project ID [default: $project]
  --prefix=NAME      tape name prefix [default: $prefix]

EOF

exit $1
}

while test $# -gt 0; do

case "$1" in
    -*=*)
    arg=`echo "$1" | awk -F= '{print $1}'`
    optarg=`echo "$1" | awk -F= '{print $2}'`
      ;;

    *)
    arg=$1
    optarg=
    ;;
esac

case "$arg" in

    --device)
    device=$optarg
    ;;

    --project)
    project=$optarg
    ;;

    --prefix)
    prefix=$prefix
    ;;

    --help)
    usage 0
    ;;
    
    *)
    usage
    exit 1
    ;;

esac

shift

done

echo Tape device: $device
echo Project ID: $project
echo Label prefix: $prefix

dbase=$HOME/bookkeeping/tape_archiver/tapes.${project}.db
dbnew=${dbase}.new

echo Database: $dbase

if [ ! -e $dbnew ]; then
  touch $dbnew
fi

last=`cat $dbase $dbnew | grep -F $prefix | awk '{print $1}' | sort | tail -1`

if test x"$last" = x; then
  number=0
else
  number=`echo $last | sed -e "s/$prefix//"`
  number=`expr $number + 1`
fi

name=`echo $number | awk '{printf("'$prefix'%04d",$1);}'`

echo New tape: $name

echo "Labeling tape in $device with label $name"
echo "Press return to continue"
read ret

mkdir -p /tmp/tapelableotron
cd /tmp/tapelableotron
touch $name

mt -f $device status > /dev/null
if [ $? -ne 0 ]; then
echo "Tape status check failed!"
exit
fi

mt -f $device rewind

mt -f $device fsf 1 2> /dev/null
if [ $? == 0 ]; then
echo "Tape already contains data ... aborting"
exit
fi

echo "Labeling tape"
tar -cf $device $name

echo "Ejecting tape"
mt -f $device rewoffl

rm $name
cd ..
rm -rf tapelableotron

echo Updating $dbnew
echo "$name  750.00    0.00  750.00  000001  0    0" >> $dbnew

echo
echo "Please label the tape with $name"
echo "then press return to continue"
read ret

exit

