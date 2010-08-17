#!/bin/tcsh

#
# For the given observation, tar the single pulse files located in the 
# observations archive directory into a single.tar file
#

set nargs = `echo $argv | wc | awk '{print $2}'`
if ( $nargs != 1 ) then
  echo Usage: tar_single_pulse.csh UTC_START
  exit
endif

set utc_start = $1

set dir = "/lfs/data0/apsr/archives/$utc_start"

if ( ! -d $dir ) then
  echo "Obs directory [$dir] did not exist"
  exit
endif

echo "UTC_START = $utc_start"

set band = `find $dir -mindepth 1 -maxdepth 1 -type d -printf "%f\n"`

echo "BAND = $band"

chdir $dir/$band

if ( -f pulse_list.txt ) then
 rm -f pulse_list.txt
endif

# create a list of files to be tarred
find . -mindepth 1 -maxdepth 1 -type f -name "pulse_*.ar" -printf "%f\n" | sort > pulse_list.txt

# get the number of pulses records
set num_pulses = `cat pulse_list.txt | wc -l`

echo "Found $num_pulses pulse files"

if ( $num_pulses > 0 ) then

  # get the first and last pulses for this archive
  set first_pulse = `head -n 1 pulse_list.txt | awk -F. '{print substr($1,7)}'`
  set last_pulse = `tail -n 1 pulse_list.txt | awk -F. '{print substr($1,7)}'`
  set tar_file = "pulses_${first_pulse}_${last_pulse}.tar"
  set cmd = "tar -cf $tar_file -T pulse_list.txt"
  echo $cmd
  $cmd

  # check the tar archive matched the number of pulses
  set num_tarred_pulses = `tar -tf $tar_file | wc -l`

  if ( $num_tarred_pulses == $num_pulses ) then
    find . -mindepth 1 -maxdepth 1 -type f -name 'pulse_*.ar' -delete
  endif

endif

if ( -f pulse_list.txt ) then
 rm -f pulse_list.txt
endif

echo "Done $utc_start/$band"

exit

