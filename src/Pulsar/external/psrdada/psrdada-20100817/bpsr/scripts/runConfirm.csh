#!/bin/csh 

#
# runConfirm.csh
#
# It folds a set of obs of a pulsar candidate with the trail ephemeris 
# provided by the observer. Then runs the code for optimizing P and DM  
# and dumps the results in a series of plots and in two files for
# subsequent human inspection 
#
# AP 21 jun 2009  ver 1.0
#

set flagfile_ini = 'confirm.processing'
set flagfile_end = 'confirm.finalized'
unalias rm
unalias ls
unalias cp
unalias mv 

# grabbing the commandline input
set nargs = `echo $argv | wc | awk '{print $2}'`
if ( ($nargs < 1) || ($nargs > 3) ) then
 echo "Usage: runConfirm.csh  <eph.file> [per-range] [dm-range]    "
 echo "   where:                                                   "
 echo "   <eph.file> (required) is a tempo1 ephemeris file, e.g.   "
 echo "                            PSRJ   0011-2233D               "
 echo "                            RAJ    00:11:22                 "
 echo "                            DECJ   -33:44:55                "
 echo "                            PEPOCH 66666                    "
 echo "                            P      7.77777                  "
 echo "                            DM     888                      "
 echo "  [per-range] (optional) period half-range (in microsec)    "
 echo "  [dm-range]  (optional) DM half-range (in pc/cm^3)         "
 echo "                                                            "
 echo "                                                            "
 echo "  N.B.1 It processes ONLY the files with extension .fil     "
 echo "        contained in the dir from where it is launched      "
 echo "  N.B.2 The eph.file must also be in the same dir           "
 echo "                                                            "
 echo "ver 1.0 AP 21jun09                                          "
 exit
endif

# setting various variables and getting the pulsar name and DM
set tsubint  = 10   # subint time for archiving with dspsr
set startdir = `pwd`
set ephname  = $1
set periodrange = $2
set dmrange = $3

# check for the existence of the required eph file
touch $flagfile_ini
echo "runConfirm started..."
echo " "
if ( !(-e $ephname) ) then
 echo "Ephemeris file <"$ephname"> not found!"
 echo "Note that <"$ephname"> must be in the local directory"
 echo "runConfirm exiting..."
 rm $flagfile_ini
 exit
endif

# assessing the range in P and DM to be searched (if blank=natural range)
set pr = " "
if ($periodrange != "") then
 set pr = `echo "-pr "$periodrange`
 echo "Operating over a range of "$periodrange" microsec in spin period"
else
 echo "Operating over a natural range in spin period"
endif
set dr = " "
if ($dmrange != "") then
 set dr = `echo "-dr "$dmrange`
 echo "Operating over a range of "$dmrange" pc/cm^3 in DM"
else
 echo "Operating over a natural range in DM"
endif

# getting the pulsar name and the number of files to be processed
set psrname  = `cat $ephname | grep "PSR" | awk '{print $2}'`
set numbfilfile = `ls *.fil | wc -l`
echo "Found "$numbfilfile" files to be processed"
echo " "

# looping on the suitable files and creating the related archives and plots"
set counter = 1
while ( $counter <= $numbfilfile )
 set inpname = `ls *.fil | head -$counter | tail -1`
 set rootfilname = `echo $inpname | awk '{print substr($1,1,index($1,".")-1)}'`
 set shortname   = `echo $inpname | awk '{print substr($1,1,index($1,".")-3)}'`
 set pointname = `header $inpname | grep "Source Name" | awk '{print $4}'` 
 echo " "
 echo "File "$counter": reducing pointing "$pointname" for the pulsar "$psrname
 echo "-------------------------------------------------------------------------------"
 echo "dspsr -L "$tsubint" -A -E "$ephname $rootfilname".fil"
 dspsr -L $tsubint -A -E $ephname $rootfilname".fil"
 set archivename = `ls $shortname*.ar`
 set rootarchivename = `echo $archivename | awk '{print substr($1,1,index($1,".")-1)}'`
 echo "pam --setnchn 32 -e far "$rootarchivename".ar"
 pam --setnchn 32 -e far $rootarchivename".ar"
 echo "pdmp -eph-coord "$pr $dr" -g /png "$rootarchivename".far"
 pdmp -eph-coord $pr $dr -g /png $rootarchivename".far"
 echo "mv pgplot.png "$rootarchivename"%"$pointname".png"
 mv pgplot.png $rootarchivename"%"$pointname".png"
@ counter++
end

echo " "
echo "runConfirm exiting..."
echo "You can find the results in the files pdmp.per and pdmp.posn"
echo "as well as in the "$numbfilfile"  .png plots"

rm $flagfile_ini
touch $flagfile_end

exit
