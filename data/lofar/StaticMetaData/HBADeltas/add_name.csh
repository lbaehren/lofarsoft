#!/bin/csh
#
set list = `ls *-*`
foreach file ($list)
  echo $file
  echo "#" > tmpfile
  set station = `echo $file | awk -F- '{print $1}'`
  set filetype = `echo $file | awk -F- '{print $2}'`
  echo "# $filetype for $station"
  echo "# $filetype for $station" >> tmpfile
  echo "#" >> tmpfile
  cat $file >> tmpfile
  mv -f tmpfile $file
end
