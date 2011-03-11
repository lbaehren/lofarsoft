#!/bin/sh
#
# Check the RAW and Reduced data for chosen ObsID in sub4, sub5, and sub6
# Also, script is looking for the Parset file in /globalhome/lofarsystem
# (be careful, though, only the suffix of ObsID is checked in the parset file name,
# so, there maybe other "wrong" parset files found)
#
# Vlad Kondratiev (c)
#

if [ $# -eq 0 ]; then
 echo "Usage: $0 <ObsID>"
 exit 0
fi

obsid=$1
echo "OBSID: $obsid"
echo "*******************************************************"
echo "Look for RAW data ..."
cexec lse:9-17 "find /data? -maxdepth 1 -type d -name '$obsid' -exec du -sh {} \;"
echo "*******************************************************"
echo "Look for REDUCED data in Pulsar Archive Area ..."
cexec lse:9-17 "find /data4/LOFAR_PULSAR_ARCHIVE_lse0?? -type d -name '$obsid*' -exec du -sh {} \;" | grep -v Permission
echo "*******************************************************"
echo "Look for Parset file in /globalhome/lofarsystem ..."
obsid_suffix=`echo $obsid | cut -f 2 -d _`
find /globalhome/lofarsystem -name "*$obsid_suffix*" -print 2>&1 | grep -v Permission
