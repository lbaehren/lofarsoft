#!/bin/csh

# example use in crontab
#
# PATH=/home/ssi/pulsar/linux_64/bin:${PATH}
# 
# 00 * * * * swin_clear_disk.csh shrek211a delete
# 20 * * * * swin_clear_disk.csh shrek211a delete
# 40 * * * * swin_clear_disk.csh shrek211a delete
# 
# 10 * * * * swin_clear_disk.csh shrek211b delete
# 30 * * * * swin_clear_disk.csh shrek211b delete
# 50 * * * * swin_clear_disk.csh shrek211b delete

if ( $1 == "" ) then
  echo "Error: must provide an argument"
  echo "Usage $0 shrek21[0a|0b|1a|1b] [delete]"
  exit 1
endif

if ($2 == "delete") then
  echo "Actually Deleting files"
  set deletethem = 1
else 
  echo "Not deleting files, add 'delete' argument to delete files"
  set deletethem = 0
endif

set basedir = "/export/$1/on_tape"

set n_delete = 0
set n_skip = 0
set n_nodelete = 0

cd /nfs/cluster/pulsar/hitrun

# list all files processed by llevin and sbates

foreach student ( llevin sbates )
  find . -maxdepth 1 -name "*${student}*" -printf "%f\n" | awk -F. '{print $1"/"$2}' \
	> /tmp/${student}.done_$1
end

# list all files processed by both

cd /tmp; cat llevin.done_$1 sbates.done_$1 | sort | uniq -d > both.done_$1

# list all files processed by nobody, in reverse time order (newest first)

cd $basedir
ls -1rd */?? > /tmp/nobody.done_$1

# clean up any empty directories

foreach obsdir ( ????-??-??-??:??:?? )
  # count the number of beams remaining to be deleted
  set num_items = `ls -1 $obsdir/?? | wc -l`
  if ($num_items == 0) then
    if ($deletethem == 1 ) then
      echo "rm -rf $basedir/$obsdir"
      rm -rf $basedir/$obsdir
      mv /nfs/cluster/pulsar/hitrun/${obsdir}* /nfs/cluster/pulsar/hitrun/cleared
    else
      echo "$obsdir would be deleted with: rm -rf $basedir/$obsdir"
    endif
  endif
end

# do not check minimum space requirement when clearing files processed by both
set check_percent = 0

foreach processed ( both llevin sbates nobody )

  echo "Clearing data that have been processed by $processed"

  unlink /tmp/hitrun_delete_$1
  touch /tmp/hitrun_delete_$1

  foreach beamdir ( `cat /tmp/${processed}.done_$1` ) 

    # If the beam dir in question also exists
    if ( -d "$basedir/$beamdir" ) then
      @ n_delete = $n_delete + 1
      if ( $deletethem == 1 ) then
        echo "$beamdir" >> /tmp/hitrun_delete_$1
      endif
    else
      @ n_skip = $n_skip + 1
    endif

  end

  echo "Found $n_delete eligble for deletion on $1"
  echo "Found $n_skip eligble, but not on disk $1"

  if ($deletethem == 1) then

    echo To be deleted: `wc -l /tmp/hitrun_delete_$1`

    cd $basedir

    foreach deleteable ( `cat /tmp/hitrun_delete_$1` )
  
      if ( -d $deleteable ) then

        date
        echo Deleting $deleteable
        time rm -rf $deleteable

        if ( $check_percent ) then
          set used = `df -k . | grep $1 | awk '{print $5}' | sed -e 's/%//'`
          echo "Percent used: $used"
          if ( $used < 90 ) exit 0
        endif

        echo Sleeping 3 seconds
        sleep 3

      endif

    end

  endif 

  set check_percent = 1

end

