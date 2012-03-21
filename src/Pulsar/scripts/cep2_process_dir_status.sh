#!/bin/bash
#
# This script is used from obsinfo-cep2.py and called through cexec from lhn001
# to collect info about processed data and return status line to be used 
# by parent script
#
# Vlad Kondratiev, May 27, 2011
###################################################################################
psrarchive=""
obsid=""
mode=""  # either CS or IS

usage="Usage: `basename $0` [-t CS|IS|CV] [-id OBSID] [-d TOP-LEVEL PSRARCHIVE DIR]"

if [[ $# == 0 ]]; then
 echo $usage
 exit 0
fi

# parsing command line
while [ $# -gt 0 ]
do
    case "$1" in
    -id)   obsid=$2; shift;;
        -d)    psrarchive="$2"; shift;;
        -t)    mode=$2; shift;;
        -*)
            echo "Usage: `basename $0` [-t CS|IS|CV] [-id OBSID] [-d TOP-LEVEL PSRARCHIVE DIR]"
            exit 1;;
        *)  break;;
    esac
    shift
done

if [[ $mode == "" ]]; then
 echo "Type of data was not given: CS or IS"
 exit 1
fi
if [[ $obsid == "" ]]; then
 echo "ObsId was not given"
 exit 1
fi
if [[ $psrarchive == "" ]]; then
 echo "The top-level psrarchive directory was not given"
 exit 1
fi

# getting the processing directory
if [[ $mode == "CS" ]]; then
 procdir=`find $psrarchive -type d -name "$obsid"_CSplots -print 2>/dev/null | egrep 'CSplots' | grep -v Permission | grep -v such`
fi
if [[ $mode == "CV" ]]; then
 procdir=`find $psrarchive -type d -name "$obsid"_CVplots -print 2>/dev/null | egrep 'CVplots' | grep -v Permission | grep -v such`
fi
if [[ $mode == "IS" ]]; then
 procdir=`find $psrarchive -type d -name "$obsid"_redIS -print 2>/dev/null | egrep 'redIS' | grep -v Permission | grep -v such`
fi

# forming status line and getting the size of processing directory
if [[ $procdir == "" ]]; then
 echo
 exit 0
else
 # for CS data
 if [[ $mode == "CS" ]]; then
  statusline="+CS"
  procsize=`du -s -B 1 $procdir 2>/dev/null | cut -f 1 | grep -v such`
  istar=`find $procdir -name "*_nopfd.tar.gz" -print 2>/dev/null | grep -v Permission | grep -v such`
  if [[ $istar == "" ]]; then
   statusline="$statusline (-tar"
  else
   statusline="$statusline (+tar"
  fi
  isrfi=`find $procdir/stokes/*/ -name "*.rfirep" -print 2>/dev/null | grep -v Permission | grep -v such`
  if [[ $isrfi == "" ]]; then
   statusline="$statusline,-rfi"
  else
   statusline="$statusline,+rfi"
  fi
  isrfiA=`find $procdir/stokes -maxdepth 1 -name "*.rfirep" -print 2>/dev/null | grep -v Permission | grep -v such`
  if [[ $isrfiA == "" ]]; then
   statusline="$statusline,-rfiA)"
  else
   statusline="$statusline,+rfiA)"
  fi
  echo $procdir
  echo $statusline
  echo $procsize
  is_combined=`ls -1 $procdir/combined.th.png 2>/dev/null | grep -v such`
  if [[ $is_combined == "" ]]; then
   echo "no" # combined plot does not exist
  else
   echo "yes"
  fi
  is_status=`ls -1 $procdir/status.th.png 2>/dev/null | grep -v such`
  if [[ $is_status == "" ]]; then
   echo "no" # status map plot does not exist
  else
   echo "yes"
  fi
  if [[ $is_combined == "" ]]; then
   :
  else
   # pattern allows both RSP* and BEAM* directories to be looked at
   prepfold_png=`find $procdir/stokes -name "*.pfd.png" -print 2>/dev/null | grep -v such | grep _PSR_ | grep -v _nomask_PSR_ | head -n 1`
   echo $prepfold_png
  fi
  exit 0
 fi # $mode == "CS"

 # for CV data
 if [[ $mode == "CV" ]]; then
  statusline="+CV"
  procsize=`du -s -B 1 $procdir 2>/dev/null | cut -f 1 | grep -v such`
  istar=`find $procdir -name "*_nopfd.tar.gz" -print 2>/dev/null | grep -v Permission | grep -v such`
  if [[ $istar == "" ]]; then
   statusline="$statusline (-tar"
  else
   statusline="$statusline (+tar"
  fi
  isrfi=`find $procdir/rawvoltages/*/ -name "*.rfirep" -print 2>/dev/null | grep -v Permission | grep -v such`
  if [[ $isrfi == "" ]]; then
   statusline="$statusline,-rfi"
  else
   statusline="$statusline,+rfi"
  fi
  echo $procdir
  echo $statusline
  echo $procsize
  is_combined=`ls -1 $procdir/combined.th.png 2>/dev/null | grep -v such`
  if [[ $is_combined == "" ]]; then
   echo "no" # combined plot does not exist
  else
   echo "yes"
  fi
  is_status=`ls -1 $procdir/status.th.png 2>/dev/null | grep -v such`
  if [[ $is_status == "" ]]; then
   echo "no" # status map plot does not exist
  else
   echo "yes"
  fi
  if [[ $is_combined == "" ]]; then
   :
  else
   # pattern allows both RSP* and BEAM* directories to be looked at
   prepfold_png=`find $procdir/rawvoltages -name "*.pfd.png" -print 2>/dev/null | grep -v such | grep _PSR_ | grep -v _nomask_PSR_ | head -n 1`
   echo $prepfold_png
  fi
  exit 0
 fi # $mode == "CV"

 if [[ $mode == "IS" ]]; then
  statusline="+IS"
  procsize=`du -s -B 1 $procdir 2>/dev/null | cut -f 1 | grep -v such`
  istar=`find $procdir -name "*_plots.tar.gz" -print 2>/dev/null | grep -v Permission | grep -v such`
  if [[ $istar == "" ]]; then
   statusline="$statusline (-tar"
  else
   statusline="$statusline (+tar"
  fi
  # there is no BEAMA directory as it is introduces already after IS 2nd transpose (now there is only one fits file for IS data)
  isRSPA=`ls -d $procdir/incoherentstokes/RSPA 2>/dev/null | grep -v such`
  if [[ $isRSPA == "" ]]; then
   statusline="$statusline,-all"
  else
   statusline="$statusline,+all"
  fi
  isrfi=`find $procdir/incoherentstokes/*/ -name "*.rfirep" -print 2>/dev/null | grep -v Permission | grep -v such`
  if [[ $isrfi == "" ]]; then
   statusline="$statusline,-rfi"
  else
   statusline="$statusline,+rfi"
  fi
  isrfiA=`find $procdir/incoherentstokes -maxdepth 1 -name "*.rfirep" -print 2>/dev/null | grep -v Permission | grep -v such`
  if [[ $isrfiA == "" ]]; then
   statusline="$statusline,-rfiA)"
  else
   statusline="$statusline,+rfiA)"
  fi
  echo $procdir
  echo $statusline
  echo $procsize
  is_combined=`ls -1 $procdir/combined.th.png 2>/dev/null | grep -v such`
  if [[ $is_combined == "" ]]; then
   echo "no" # combined plot does not exist
  else
   echo "yes"
  fi
  if [[ $is_combined == "" ]]; then
   :
  else
   if [[ $isRSPA == "" ]]; then
    # pattern allows both RSP* and BEAM* directories to be looked at
    prepfold_png=`find $procdir/incoherentstokes -name "*.pfd.png" -print 2>/dev/null | grep -v such | grep _PSR_ | grep -v _nomask_PSR_ | head -n 1`
    # if this is FE observation, then we have to check beam_N subdirectories...
   else
    prepfold_png=`ls -1 $procdir/incoherentstokes/RSPA/*.pfd.png 2>/dev/null | grep -v such | grep RSPA_PSR | head -n 1`
   fi
   echo $prepfold_png
  fi
  exit 0
 fi # $mode == "IS"
fi
