#!/bin/tcsh


set before = `date +%s`

set mydir = `pwd`

setenv PSRCAT_FILE $LOFARSOFT/release/share/pulsar/data/psrcat.db


if ( $# == 0 ) then
 echo
 echo
 echo "USAGE:    doRaw.sh -f <file> -p <psr> [-c <fraction> -b <nblocks> -a -v -s <tsub> -k <nchan>]"
 echo "    e.g., doRaw.sh -f L12345_SAP000_B000_S0_P000_bf.raw -p 0329+54 -c 0.02 -b 500 -a -v -s 5 -k 8"
 echo
 echo "-- Note: It doesn't matter which of the two polarisation files (S0 or S1) you give to the script, it only needs one."
 echo
 echo
 echo "OPTIONS:" 
 echo
 echo " -b <nblocks>   Only read the first <nblocks> blocks"
 echo "                ... default: all blocks"
 echo
 echo " -c <fraction>  Clip <fraction> off the edges of the samples histogram"
 echo "                ... note: eliminates spiky RFI -- but may also clip bright pulsar signals!"
 echo
 echo " -a             Normalise data based on the entire data set"
 echo "                ... note: if not used, the scaling is updated after every data block"
 echo
 echo " -s <tsub>      Set the length of each subintegration in the archive to <tsub>"
 echo "                ... default: 5 sec"
 echo
 echo " -k <nchan>     Create an additional filterbank with <nchan> channels"
 echo "                ... default: no channels"
 echo
 echo " -t             Write out ascii files (.rv) containing the complex values"
 echo
 echo " -v             Verbose"
 echo

 exit;
endif

set alltopt = ""
set verbopt = ""
set asciopt = ""
set PSR = ""
set cutoff = ""
set nblocks = ""
set subints = "-A -L 5"
set fchannels = ""

set temp=(`getopt -s tcsh -o a,v,t,f:,p:,c:,b:,s:,k:  -- $argv:q`)
if ($? != 0) then 
  echo "Terminating..." >/dev/stderr
  exit 1
endif

eval set argv=\($temp:q\)

while (1)
	switch($1:q)
	case -a:
		set alltopt = "-all_times" ; shift 
		breaksw;
	case -v:
		set verbopt = "-v" ; shift 
		breaksw;
	case -t:
		set asciopt = "-t" ; shift 
		breaksw;
	case -f:
		set Name = $2:q ; shift ; shift
		breaksw
	case -p:
		set PSR = $2:q ; shift ; shift
		breaksw
	case -c:
                set cutoff = "-hist_cutoff $2:q" ; shift ; shift
		breaksw
	case -b:
		set nblocks = "-b $2:q" ; shift ; shift
		breaksw
	case -s:
		set subints = "-A -L $2:q" ; shift ; shift
		breaksw
	case -k:
		set fchannels = "-F ${2:q}:D" ; shift ; shift
		breaksw
	case --:
		shift
		break
	default:
		echo "Internal error!" ; exit 1
        endsw
end


while ($#argv > 0)
	echo '--> '\`$1:q\'
	shift
end

if ( "$PSR" == "" ) then
    echo "You must supply a pulsar name!"
    exit 1
endif


echo "FILE: $Name"
set Base = `echo $Name | awk -F_ '{print $1}'`

set psrlen = `echo $PSR | awk '{print length($1)}'`

if ( $psrlen == 7 ) then
  echo "PULSAR: B${PSR}"
else
  echo "PULSAR: J${PSR}"
endif

if ( !( -e {$PSR}.eph ) ) then
  psrcat -e $PSR > {$PSR}.eph
endif


set narch = `ls -1 {$Base}_SAP000_B000_SB*_CH*.ar | wc -l`
if ( !( $narch == 0 ) ) then
  rm -rf {$Base}_SAP000_B000_SB*_CH*.ar
endif

cp /globalhome/lofarsystem/log/${Base}/${Base}.parset .
cp $LOFARSOFT/src/Pulsar/scripts/header.puma2 .

### Run the bf2puma2 converter ###
echo "Running: bf2puma2 -f ${Base}_SAP000_B000 -h header.puma2 -p ${Base}.parset -BG $verbopt $cutoff $nblocks $alltopt $asciopt"

#./bf2puma2 -f {$Base}_SAP000_B000 -h header.puma2 -p {$Base}.parset -BG $verbopt $cutoff $nblocks $alltopt $asciopt


#set SubSa = `grep Observation.subbandList ${Base}.parset | awk -F[ '{print $2}' | awk -F. '{print $1}'`
#set SubSo = `grep Observation.subbandList ${Base}.parset | awk -F] '{print $1}' | awk -F. '{print $4}'`
#set Nsub = `echo $SubSa $SubSo | awk '{print $2 - $1+1}'`

#set Nchan = `grep channelsPerSubband {$Base}.parset | head -1 | awk -F= '{print $2}'`

set Nsub = `ls -1 {$Base}_SAP000_B000_SB* | awk -F_CH  '{print substr($1,22)}' | sort -n | tail -1`
@ Nsub = $Nsub + 1

set Nchan = `ls -1 {$Base}_SAP000_B000_SB* | awk  '{print substr($1,26)}' | sort -n | tail -1`
@ Nchan = $Nchan + 1



@ sub = 0
while( $sub < $Nsub )
    @ chan = 0
    while( $chan < $Nchan )
        ### Set the MJD from the puma2 headers ###
        set MJD = `head -25 {$Base}_SAP000_B000_SB{$sub}_CH{$chan} | grep MJD | awk '{print $2}'`
        ### Dedisperse every channel of every subband ###
        dspsr $subints $fchannels -E {$PSR}.eph -m $MJD -O {$Base}_SAP000_B000_SB{$sub}_CH{$chan} {$Base}_SAP000_B000_SB${sub}_CH${chan}
        @ chan = $chan + 1
    end
    @ sub = $sub + 1
end

### Sum up all the dedispersed channels and save a PSRFITS archive ###
psradd -R -o {$Base}.ar {$Base}_SAP000_B000_SB*_CH*.ar

### Delete every 16th channel ###
set nchn = `vap -c NCHAN ${Base}.ar | tail -1 | awk '{print $2}'`

seq 0 16 $nchn > list.k

paz -r -k list.k -e fits ${Base}.ar >& /dev/null

rm list.k

rm -rf ${Base}_SAP000_B000_SB*_CH*
#################################

set after = `date +%s`

set tdiff = `echo | awk '{print ('$after'-'$before')/60.0}'`

echo ; echo
echo "### FINISHED ${Base}_SAP000_B000_S0/S1_P000_bf.raw ###"
echo 
echo "Elapsed time: $tdiff minutes"
echo
echo "Output: ${Base}.fits"
echo
echo "######################################################"

touch bf2puma2.log

set mydate = `date`

echo "----------------------------------------------------------------" >> bf2puma2.log
echo "    ${mydate} " >> bf2puma2.log
echo "----------------------------------------------------------------" >> bf2puma2.log
echo "    Processed observation: ${Base}_SAP000_B000_S0/S1_P000_bf.raw" >> bf2puma2.log
echo "    Process duration: $tdiff minutes" >> bf2puma2.log
echo "    Output file: ${Base}.fits" >> bf2puma2.log
echo "----------------------------------------------------------------" >> bf2puma2.log
echo >> bf2puma2.log
echo >> bf2puma2.log
