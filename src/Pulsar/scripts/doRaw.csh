#!/bin/tcsh

if ( "$1" == "" ) then

  echo
  echo
  echo "Usage(example): csh doRaw.sh L00000_SAP000_B000_S0_P000_bf.raw 0329+54 " 
  echo
  echo " Notes: "
  echo "         (1) This script requires a bf2puma2 system-wide installation " 
  echo "         (2) PSRCHIVE, DSPSR, PSRCAT and TEMPO need to be installed  "
  echo
  echo


  exit

endif


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

### Run the bf2puma2 converter ###
bf2puma2 -f {$Base}_SAP000_B000 -h header.puma2 -p {$Base}.parset -BG -v


set SubSa = `grep Observation.subbandList ${Base}.parset | awk -F[ '{print $2}' | awk -F. '{print $1}'`
set SubSo = `grep Observation.subbandList ${Base}.parset | awk -F] '{print $1}' | awk -F. '{print $4}'`
set Nsub = `echo $SubSa $SubSo | awk '{print $2 - $1+1}'`

set Nchan = `grep channelsPerSubband {$Base}.parset | head -1 | awk -F= '{print $2}'`

@ sub = 0
while( $sub < $Nsub )
    ### Skip the first channel of every subband ###
    @ chan = 1
    while( $chan < $Nchan )
        ### Set the MJD from the puma2 headers ###
        set MJD = `head -25 {$Base}_SAP000_B000_SB{$sub}_CH{$chan} | grep MJD | awk '{print $2}'`
        ### Dedisperse every channel of every subband ###
        dspsr -F8:D -A -L 10 -E {$PSR}.eph -m $MJD -O {$Base}_SAP000_B000_SB{$sub}_CH{$chan} {$Base}_SAP000_B000_SB${sub}_CH${chan}
        @ chan = $chan + 1
    end
    @ sub = $sub + 1
end

### Sum up all the dedispersed channels and save a PSRFITS archive ###
psradd -R -o {$Base}.ar {$Base}_SAP000_B000_SB*_CH*.ar


set after = `date +%s`

set tdiff = `echo | awk '{print ('$after'-'$before')/60.0}'`

echo ; echo
echo "### FINISHED BF-2-PUMA2 ###"
echo 
echo "Elapsed time: $tdiff minutes"
echo
echo "###########################"
