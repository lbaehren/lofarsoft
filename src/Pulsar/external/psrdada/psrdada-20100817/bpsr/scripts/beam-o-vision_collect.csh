#!/bin/csh

#
# beam-o-vision-collect.csh
#
# It looks for pulsar-mode observations only (rejecting the survey ones) 
# for which png files of "frequency vs phase" have been produced. Then
# it waits for completion of the observation and then copies the 
# relevant png's into a new dir (prefixed with BEAM-O-VISION)
# in the "results" dir. When the source is looked at in different
# beams, it collects all the png files in the same aforementioned
# directory, so to produce a simultaneous 13-beam plot of the source
# 
# AP 20 nov 2008  ver 1.0  
#
# next2do: to include copying of pulse profile png files
#          to check for the number of "BEAM-O-VISION"-kind of dirs in 
# the dir "Results" and remove all but the most recent 10 dirs
#

# grabbing the commandline input
set nargs = `echo $argv | wc | awk '{print $2}'`
if ($nargs != 3) then
 echo Usage: beam-o-vision_collect archive-dir png-dir killflag
 exit
endif
set archivedir = $1
set pngdir = $2  
set killflag = $3

# setting some variables
set startdir = `pwd`
set debug = 0
set timesleep = 5
set timewait = 20
unalias cp

# check if the script must not be run
if ( (-e $startdir"/"$killflag) ) then
  rm $startdir"/"$killflag
  echo "beam-o-vision-collect: existing before entering the loop..."
  exit
endif

if ( $debug != 0 ) then
    echo "beam-o-vision-collect: entering the loop..."
    echo "startdir = " $startdir
    echo "archivedir = " $archivedir
    echo "pngdir =" $pngdir
    echo "kill flagfile = " $killflag
endif

#
# starting the neverending loop
#
LoopOnObs: 

# check if the script must be exited
if ( (-e $startdir"/"$killflag) ) then
  rm $startdir"/"$killflag
  echo "beam-o-vision-collect: now existing..."
  exit
endif

# search for the presence of useful observations
cd $archivedir
set lastobs = ` ls -Fl | awk '{print $9}' | tail -1`
set lastheader = $lastobs"01/obs.start"
if ( !(-e $lastheader) ) then
 echo "beam-o-vision-collect: No obs in "$archivedir": Waiting for a new observation"
 sleep $timesleep
 goto LoopOnObs
endif

# rejecting the survey obs (ie those having code G)
set sourcepoch = `grep -i source $lastheader | awk '{printf("%s\n",substr($2,1,1))}'`
if (  $sourcepoch != "J" && $sourcepoch != "B" ) then
 echo "beam-o-vision-collect: No useful obs in "$lastobs$refbeam": Waiting for a new observation"
 sleep $timesleep
 goto LoopOnObs
endif

# checking the presence of pulse_vs_phase files (pvf) in the relevant directory
set sourcename = `grep -i source $lastheader | awk '{print $2}'`
set refbeam =  `grep -i refbeam $lastheader | awk '{printf("%0.2d \n", $3)}'`
set rootplotdir = $pngdir"/"$lastobs
set goodplotdir = $pngdir"/"$lastobs$refbeam"/"
ls -Fl $goodplotdir*pvf*.png >& 0000check_pvf.txt
set pvfcheck = `grep -i "no match" 0000check_pvf.txt | wc -l`
rm -rf 0000check_pvf.txt
if ( $pvfcheck != 0 ) then
  echo "beam-o-vision-collect: No useful pvf-file in "$goodplotdir": Waiting for a new observation"
  sleep $timesleep
  goto LoopOnObs
endif 

# now it has to wait for the presence of the obs.finished file 
# in the relevant directory before going on 
while ( !(-e $rootplotdir"obs.finished") ) 
  echo "beam-o-vision-collect: Waiting for completion of obs in "$goodplotdir
  sleep $timewait
end

# checking if a Beam-o-vision directory already exist for this source,
# and, if not, it creates that and the subdirs structure
set collectdir = $pngdir"/BEAM-O-VISION_"$sourcename
if ( !(-d $collectdir) ) then
  mkdir $collectdir
  cd $collectdir
  foreach subdir ( 01 02 03 04 05 06 07 08 09 10 11 12 13 )
    mkdir $subdir
  end
  echo "beam-o-vision-collect: Created new directory "$collectdir 
else
  echo "beam-o-vision-collect: The directory "$collectdir" already exists"  
endif

# copying the relevant png files to the proper Beam-o-vision directory
cp $goodplotdir*pvf*.png $collectdir"/"$refbeam"/."
cp $goodplotdir"obs.start" $collectdir"/"$refbeam"/."
cp $rootplotdir"obs.info" $collectdir"/."
cp $rootplotdir"obs.finshed" $collectdir"/."

if ( $debug != 0) then
    echo "cp "$goodplotdir"*pvf*.png to "$collectdir"/"$refbeam"/."
    echo "cp "$goodplotdir"obs.start to "$collectdir"/"$refbeam"/."
    echo "cp "$rootplotdir"obs.info to "$collectdir"/."
    echo "cp "$rootplotdir"obs.finished to "$collectdir"/."
else 
    echo "beam-o-vision-collect: Copied the files to "$goodplotdir
endif

# fixing the name of the source in the obs.start file in $collectdir/$refbeam/
cd $collectdir"/"$refbeam
cp obs.start obs.start.orig
set newsourcename = "BEAM-O-VISION_"$sourcename
replace -s $sourcename $newsourcename -- obs.start

if ( $debug != 0) then
    echo "sourcename = "$sourcename
    echo "newsourcename = "$newsourcename
    echo "beam-o-vision-collect: Replaced the source name in "$collectdir"/"$refbeam"/obs.start"
else
    echo "beam-o-vision-collect: Replaced the source name in "$collectdir"/"$refbeam"/obs.start"
endif 

sleep $timesleep
goto LoopOnObs

exit
