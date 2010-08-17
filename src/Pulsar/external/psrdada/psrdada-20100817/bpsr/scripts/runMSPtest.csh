#!/bin/csh 

#
# runMSPsearch.csh
#
# It folds with the catalog ephemeris the observation of a
# pulsar and compare the results with the best period
# candidates resulting from a seek search of the same data
# Upon successful completion, it creates a flag file mspsearch.finalized
#
# AP 24 nov 2008  ver 1.0
#

setenv PULSARHUNTER_HOME /home/dada/pulsarhunter  # needed for bestexe to run
set headerexe    = '/home/mkeith/ph-pipeline/packages/sigproc_4.3.mk3/bin/header'
set dedispersexe = '/home/dada/linux_64/bin/dedisperse_all'
set seekexe      = '/home/mkeith/ph-pipeline/packages/sigproc_4.3.mk3/bin/seek'
set bestexe      = '/home/dada/pulsarhunter/scripts/ph-best'
set dspsrexe     = '/home/dada/linux_64/bin/dspsr'
set flagfile_ini = 'mspsearch.processing'
set flagfile_end = 'mspsearch.finalized'
set workarea     = '/nfs/results/bpsr' # the dir where we finally run the code and save results
set snmin        = 9  # minimum s/n for the candidates to be retained
unalias rm
unalias ls
unalias cp
unalias mv 

# grabbing the commandline input
set nargs = `echo $argv | wc | awk '{print $2}'`
if ($nargs != 2) then
 echo "Usage: runMSPsearch <datafile.fil> <local(Y/N)>             "
 echo "                                                            "
 echo "   where:                                                   "
 echo "   <datafile.fil>       input data file                     "
 echo "   <local (Y/N)>        if Y, it creates the output locally "
 echo "                        otherwise in "$workarea
 echo "AP 24nov08                                               "
 exit
endif

# determining the working area
touch $flagfile_ini
if (( $2 == Y ) || ( $2 == y )) then
  set workarea = "."
  rm $flagfile_ini
endif

# setting various variables and getting the pulsar name and DM
set tsubint  = 10   # subint time for archiving with dspsr
set startdir = `pwd`
set inpname  = $1
set filname  = `echo $inpname | awk '{print substr($1,length($1)-22)}'`
set datename = `echo $inpname | awk '{print substr($1,length($1)-22,19)}'`
set pathname = `echo $inpname | awk '{print substr($1,1,length($1)-23)}'`
set pathnam2 = `echo $inpname | awk '{print substr($1,1,length($1)-26)}'`
set psrname  = `$headerexe $inpname | grep "Source Name" | awk '{print $4}'` 
set psrname2 = `psrcat -nohead -nonumber -o short -c "jname dm" $psrname | awk '{print $1}'`
set psrdm    = `psrcat -nohead -nonumber -o short -c "jname dm" $psrname | awk '{print $2}'`
if ( $psrname != $psrname2 ) then
  echo "PSR name mismatch btw file header and catalogue" 
  exit
endif

# creating the needed directory structure for the search and a soft link to the input file
# cd $workarea   # make this line effective before committing the code
set testdir = $datename"_PSRtest"
if ( !(-d $testdir) ) then
  cd $workarea
  mkdir $testdir
  cd $testdir
  cp $startdir"/"$pathnam2"obs.info" .
  cp $startdir"/"$pathnam2"obs.finished" .
  mkdir 01
  cd 01
  ln -s $startdir"/"$inpname
  cp $startdir"/"$pathname"obs.start" .
  cp obs.start obs.start.orig
  set newsourcename = $psrname"_PSRtest"
  replace -s $psrname $newsourcename -- obs.start
  rm obs.start.orig
  echo "runMSPtest: Set up new directory "$testdir" and soft link to "$inpname 
else
  echo "runMSPtest: The directory "$testdir" already exists. Something is wrong, exiting..."
  exit
endif

# running the dedispersion at the DM of the pulsar
echo "runMSPtest: Running the dedispersion on "$psrname2"-data with DM = "$psrdm"..."
$dedispersexe $filname -d $psrdm $psrdm -g 1000000

# running the search on the dedispersed file
set dedispfilname = `echo $filname $psrdm | awk '{printf("%s.%07.2f.tim\n",$1,$2)}'`
echo "runMSPtest: Running the search on "$dedispfilname"..."
$seekexe $dedispfilname -fftw

# grouping and sorting the cands resulting from the search 
set candsfilname = `echo $filname $psrdm | awk '{printf("%s.%07.2f.prd\n",$1,$2)}'`
set rootcandsfilname = `echo $filname $psrdm | awk '{printf("%s.%07.2f\n",$1,$2)}'`
echo "runMSPtest: Grouping and sorting the candidates listed in "$candsfilname"..."
$bestexe $candsfilname $rootcandsfilname -nophcx -minsnr $snmin 

# running dspsr on the original file and producing the subint archives
echo "runMSPtest: Running dspsr on "$filname" in "$tsubint" sec subintegrations"
$dspsrexe -L $tsubint $filname

# extracting the relevant period info from *ar files
set arfilestart = `ls *.ar | head -1`
set arfileend  = `ls *.ar | tail -1`
set periodstart = `vap -n -c period $arfilestart | awk '{printf("%12.7f\n",$2*1000)}' `
set periodend = `vap -n -c period $arfileend | awk '{printf("%12.7f\n",$2*1000)}' `
echo "runMSPtest: Initial and final folding period = "$periodstart" ms and "$periodend" ms"

# producing the relevant png plots
echo "runMSPtest: Producing the png plot from the dspsr output"
psradd -o $datename"_all.temp" *.ar 
rm *.ar
mv  $datename"_all.temp" $datename".ar"
pav -DFTpd -g /png $datename".ar"
mv pgplot.png $datename".DFT.png"
pav -YFpd -g /png $datename".ar"
mv pgplot.png $datename".YF.png"
pav -GTpd -g /png $datename".ar"
mv pgplot.png $datename".GT.png"

# comparing cands' period in hicandsfilname with folded periods
# and storing the line number of the matching candidates
set hicandsfilname = $rootcandsfilname".lis"
cat $hicandsfilname |  awk '{printf("%6.1f    %12.6f      %i     %i \n",$2,$3,$7,$8)}' >  temp.lis
mv temp.lis $hicandsfilname
echo "runMSPtest: Inspecting the candidate list "$hicandsfilname" from the seek output"
set oklist = 'ok_list'
if ( (-e $oklist) ) then
 rm $oklist
endif
set nsuscand = `cat $hicandsfilname | wc -l`
set i = 1
Loop:
 if( $i <= $nsuscand ) then
   set periodcand = `cat $hicandsfilname | head -$i |tail -1 | awk '{printf("%12.7f \n",$2)}'`
   echo $periodstart $periodcand $periodend $i | awk '{if (($2>=$1)&&($2<=$3)) printf("%i \n",$4)}' >> $oklist
@ i++
    goto Loop
 endif

# preparing the header of the $hicandsfilname file
echo "===== CANDIDATES WITH s/n > "$snmin" ==============" > head.lis
echo " Spec SNR      Period     #Harm    Fold " >> head.lis
echo "--------------------------------------------" >> head.lis


# reporting the results of the comparion above
set outputfile = $datename"_"$psrname".report"
echo "============================  RESULTS =======================================" > $outputfile
echo " EXPECTED PERIODICITY for "$psrname" btw: ["$periodstart"-"$periodend"] ms" >> $outputfile
echo "-----------------------------------------------------------------------------" >> $outputfile
set nokcand = `cat $oklist | wc -l`

# case NO period matching
set k0 = 0
if ( $k0 == $nokcand )  then  
  echo " SUSPICIOUSLY, no matching period has been automatically found... " >> $outputfile
  echo " However, the used ephemeris for "$psrname" might not be the latest." >> $outputfile
  echo " Therefore, please look at the plot "$datename".YF.png for a drift" >> $outputfile
  echo " in the signal and inspect the full candidate list located " >> $outputfile 
  echo " in "$hicandsfilname >> $outputfile
  echo " looking for periodicity close to the expected value." >> $outputfile
  echo " " >> $outputfile 
  echo " If no such periodicity exists or no signal is visible in the "  >> $outputfile 
  echo " plot "$datename".YF.png" >> $outputfile
  echo " please check the system and the RFI status, since " >> $outputfile
  echo " SOMETHING IS LIKELY GOING WRONG! " >> $outputfile 
  echo "---------------------------------------------------------------------------" >> $outputfile
  echo " You can find this summary in "$datename"_"$psrname".report" >> $outputfile
  echo "===========================================================================" >> $outputfile
  cat head.lis $hicandsfilname > good.lis
  mv  good.lis $hicandsfilname
  rm  head.lis
  cat $outputfile
  rm $rootcandsfilname".tim"
  rm $rootcandsfilname".top"
  rm $rootcandsfilname".prd"
  rm $datename".fil"
  rm $oklist
  touch $flagfile_end
  if (( $2 == Y ) || ( $2 == y )) then
    rm $flagfile_end
  endif
  exit
endif

# case SOME period matching
set k1 = 1
Okloop:
if ( $k1 <= $nokcand )  then 
   set candline = `cat $oklist | head -$k1 |tail -1`
   set periodcand = `cat $hicandsfilname | head -$candline |tail -1 | awk '{printf("%12.7f \n",$2)}'`
   set sn = `cat $hicandsfilname | head -$candline |tail -1 | awk '{printf("%5.1f \n",$1)}'` 
   echo " FOUND a matching period P"$k1"="$periodcand" ms with S/N="$sn" at pos="$candline" in the list" >> $outputfile
@ k1++
    goto Okloop
endif
echo "-----------------------------------------------------------------------------" >> $outputfile
echo " You can find this summary in file "$datename"_"$psrname".report.ok" >> $outputfile
echo " whereas the full candidate list is in "$hicandsfilname >> $outputfile
echo "=============================================================================" >> $outputfile
  cat head.lis $hicandsfilname > good.lis
  mv  good.lis $hicandsfilname
  rm  head.lis
  mv  $outputfile $outputfile".ok"
  cat $outputfile".ok"
  rm $rootcandsfilname".tim"
  rm $rootcandsfilname".top"
  rm $rootcandsfilname".prd"
  rm $datename".fil"
  rm $oklist
  touch $flagfile_end
  if (( $2 == Y ) || ( $2 == y )) then
    rm $flagfile_end
  endif

exit
