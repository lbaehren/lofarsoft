#!/bin/tcsh


set before = `date +%s`

set mydir = `pwd`

setenv PSRCAT_FILE $LOFARSOFT/release/share/pulsar/data/psrcat.db

echo " USAGE: csh doRaw.sh L00000_SAP000_B000_S0_P000_bf.raw 0329+54"
echo "-- Note: It doesn't matter which of the two polarisation files (S0 or S1) you give to the script, it only needs one."

set Name = $1
echo "PULSAR: $Name"
set Base = `echo $Name | awk -F_ '{print $1}'`

set PSR = $2

if ( !( -e {$PSR}.eph ) ) then
  psrcat -e $PSR > {$PSR}.eph
endif

cp /globalhome/lofarsystem/log/${Base}/${Base}.parset .

### Run the bf2puma2 converter ###
bf2puma2 -f {$Base}_SAP000_B000 -h header.puma2 -p {$Base}.parset -BG -v


set SubSa = `grep Observation.subbandList ${Base}.parset | awk -F[ '{print $2}' | awk -F. '{print $1}'`
set SubSo = `grep Observation.subbandList ${Base}.parset | awk -F] '{print $1}' | awk -F. '{print $4}'`
set Nsub = `echo $SubSa $SubSo | awk '{print $2 - $1+1}'`

set Nchan = `grep channelsPerSubband {$Base}.parset | head -1 | awk -F= '{print $2}'`

@ sub = 0
while( $sub < $Nsub )
    ### Skip the first channel of every subband ###
    @ chan = 0
    while( $chan < $Nchan )
        ### Set the MJD from the puma2 headers ###
        set MJD = `head -25 {$Base}_SAP000_B000_SB{$sub}_CH{$chan} | grep MJD | awk '{print $2}'`
        ### Dedisperse every channel of every subband ###
        dspsr -A -L 5 -E {$PSR}.eph -m $MJD -O {$Base}_SAP000_B000_SB{$sub}_CH{$chan} {$Base}_SAP000_B000_SB${sub}_CH${chan}
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
