#!/bin/ksh 
#Convert raw LOFAR data
#Workes on incoherent, coherent and fly's eye data.
# N core defaul is = 8 (cores)

#PLEASE increment the version number when you edit this file!!!
VERSION=3.02

#####################################################################
# Usage #
#####################################################################
#Check the usage
USAGE1="\nusage : make_subs_SAS_Ncore_Mmodes.sh -id OBS_ID -p Pulsar_names -o Output_Processing_Location [-raw input_raw_data_location] [-par parset_location] [-core N] [-all] [-all_pproc] [-rfi] [-rfi_ppoc] [-C] [-del] [-incoh_only] [-coh_only] [-incoh_redo] [-coh_redo] [-transpose] [-nofold] [-help] [-test] [-debug] [-subs]\n\n"\
"      -id OBS_ID  ==> Specify the Observation ID (i.e. L2010_06296) \n"\
"      -p Pulsar_names ==> Specify the Pulsar Name or comma-separated list of Pulsars for folding (w/o spaces) or\n"\
"         specify the word 'position' (lower case) find associated known Pulsars in the FOV of observation or\n"\
"         specify the word 'NONE' (upper case) when you want to skip the folding step of the processing\n"\
"         (i.e. single Pulsar: B2111+46) (i.e. multiple pulsars to fold:  B2111+46,B2106+44) \n"\
"         (i.e. up to 3 brights pulsars to fold at location of FOV: position \n"\
"      -o Output_Processing_Location ==> Specify the Output Processing Location \n"\
"         (i.e. /net/sub5/lse013/data4/LOFAR_PULSAR_ARCHIVE_lse013/L2010_06296_red) \n"\
"      [-raw input_raw_data_location] ==> when pipeline is not run on CEPI, input raw data locations can be specified;\n"\
"            directory structure assumed as: input_raw_data_location/L2011_OBSID/\n"\
"            Wildcards can be used for multiple input locations, but MUST be quoted:\n"\
"            (i.e. -raw \"/state/partition2/lofarpwg/RAW?\" ) \n"\
"      [-par parset_location] ==> when pipeline is not run on CEPI, input parameter file location can be specified;\n"\
"            directory structure assumed as: parset_location/<any_path>/LOBSID.parset\n"\
"            (i.e. -par /state/partition2/lofarpwg/PARSET ) \n"\
"      [-all] ==> optional parameter perform folding on entire subband set in addition to N-splits (takes 11 extra min)\n"\
"      [-all_pproc] ==> Post-Processing optional parameter to ONLY perform folding on entire subband set based on already-processed N-splits\n"\
"      [-rfi] ==> optional parameter perform Vlad's RFI checker and only use clean results (takes 7 extra min)\n"
USAGE2="      [-rfi_pproc] ==> Post-Processing optional parameter to perform Vlad's RFI checker on already-processed N-splits\n"\
"      [-C | -c] ==> optional parameter to switch on bf2presto COLLAPSE (Collapse all channels in MS to a single .sub file)\n"\
"      [-del] ==> optional parameter to delete the previous ENTIRE Output_Processing_Location if it exists (override previous results!)\n"\
"      [-core N] ==> optional parameter to change the number of cores (splits) used for processing (default = 8)\n"\
"      [-incoh_only] ==> optional parameter to process ONLY Incoherentstokes (even though coherentstokes data exist)\n"\
"      [-coh_only] ==> optional parameter to process ONLY Coherentstokes  (even though incoherentstokes data exist)\n"\
"      [-incoh_redo] ==> optional parameter to redo processing for Incoherentstokes (deletes previous incoh results!)\n"\
"      [-coh_redo] ==> optional parameter to redo processing for Coherentstokes (deletes previous coh results!)\n"\
"      [-transpose] ==> optional parameter to indicate the input data were run through the TAB 2nd transpose\n"\
"      [-nofold] ==> optional parameter to turn off folding of data (prepfold is not run);  multiple pulsar names are not possible\n"\
"      [-subs] ==> optional parameter to process raw files into presto .subXXXX files instead of the default psrfits\n"\
"      [-help] ==> optional parameter which prints the usage and examples of how to run the pipeline\n"\
"      [-test] ==> optional for testing: runs bf2presto and bypasses prepfold and rfi processing but echo's all commands\n"\
"      [-debug] ==> optional for testing: turns on debugging in ksh (tons of STDOUT messages)\n"


if [ $# -lt 1 ]                    # this script needs at least 6 args, including -switches
then
   print "$USAGE1"    
   print "$USAGE2"    
   exit 1
fi

#alias bf2presto8=/home/hassall/lofarsoft/release/share/pulsar/bin/bf2presto8

#####################################################################
#Get the input arguments
#####################################################################
OBSID=""
PULSAR=""
PULSAR_LIST=""
location=""
COLLAPSE=""
delete=0
all=0
all_orig=0
all_pproc=0
all_pproc_orig=0
rfi=0
rfi_pproc=0
core=8
incoh_only=0
coh_only=0
incoh_redo=0
coh_redo=0
transpose=0
help=0
test=0
nofold=0
debug=0
input_location=""
user_input_location=0
parset_location=""
user_parset_location=0
input_string=$*
proc=0
subsformat=0
H5_exist=0

while [ $# -gt 0 ]
do
    case "$1" in
    -id)   OBSID=$2; shift;;
	-p)    PULSAR="$2"; PULSAR_LIST=$2; shift;;
	-o)    location="$2"; shift;;
	-C)    COLLAPSE="$1";;
	-c)    COLLAPSE="-C";;
	-del)  delete=1;;
	-rfi)  rfi=1;;
	-rfi_pproc)  rfi_pproc=1;;
	-all)  all=1; all_orig=1;;
	-all_pproc)  all_pproc=1; all_pproc_orig=1;;
	-core) core=$2; shift;;
	-incoh_only)  incoh_only=1;;
	-coh_only)    coh_only=1;;
	-incoh_redo)  incoh_redo=1;;
	-coh_redo)    coh_redo=1;;
	-transpose)   transpose=1;;
	-test)   test=1;;
	-nofold)   nofold=1;;
	-debug)   debug=1;;
	-subs)    subsformat=1;;
	-raw)     input_location="$2"; user_input_location=1; shift;;
	-par)     parset_location="$2"; user_parset_location=1; shift;;
	-help)   help=1 
	         cat $LOFARSOFT/release/share/pulsar/data/pulp_help.txt
	         exit 1;;
	-*)
	    echo >&2 \
	    "$USAGE1" "$USAGE2"
	    exit 1;;
	*)  break;;	
    esac
    shift
done

if [[ $help == 1 ]]
then 
   cat $LOFARSOFT/release/share/pulsar/data/pulp_help.txt
   exit 1
fi

if [[ $debug == 1 ]]
then
   echo "====================Turning on ksh DEBUGGING (to STDOUT)===================="
   echo ""
   set -x
fi

#####################################################################
#check for minimum required input
#####################################################################
if [[ $PULSAR == "" ]]
then 
   echo ""
   echo "Pulsar name is required as input with -p flag."
   print "$USAGE1" 
   print "$USAGE2" 
   exit 1
fi
if [[ $OBSID == "" ]]
then 
   echo ""
   echo "OBSID is required as input with -id flag."
   print "$USAGE1" 
   print "$USAGE2" 
   exit 1
fi
if [[ $location == "" ]]
then 
   echo ""
   echo "Output is required as input with -o flag."
   print "$USAGE1" 
   print "$USAGE2" 
   exit 1
fi

#####################################################################
# Print the basic information about input parameters to STDOUT at start of pipeline run
#####################################################################
echo "Running make_subs_SAS_Ncore_Mmodes.sh with the following input arguments:"
echo "    OBSID = $OBSID"
echo "    PULSAR = $PULSAR"
echo "    Output Processing Location = $location"
if [[ "$COLLAPSE" != "" ]]
then 
   echo "    COLLAPSE = $COLLAPSE"
else
   echo "    COLLAPSE is not set"
fi
if [ $all -eq 1 ]
then 
   echo "    Performing additional processing on ALL the subbands"
else
   echo "    No additional processing requested on ALL the subbands"
fi
if [ $all_pproc -eq 1 ]
then 
   echo "    Performing only post-processing of ALL the subbands"
fi
if [ $rfi -eq 1 ]
then 
   echo "    Performing additional RFI processing"
else
   echo "    No additional RFI processing requested"
fi
if [ $rfi_pproc -eq 1 ]
then 
   echo "    Performing only rfi-checking as post-processing"
fi
echo "    Using N=$core cores for processing (change with '-core N' command line option)"

if [ $core -lt 1 ] || [ $core -gt 128 ]
then
   echo "ERROR: Number of cores must be 1 >= N <= 128 ;  currently requested $core cores."
   exit 1
fi

if [ $incoh_redo -eq 1 ]
then
   echo "    Performing redo of Incoherentstokes processing" 
fi

if [ $coh_redo -eq 1 ]
then
   echo "    Performing redo of Coherentstokes processing" 
fi

if [ $incoh_only -eq 1 ]
then
   echo "    Performing processing of Incoherentstokes data ONLY" 
fi

if [ $coh_only -eq 1 ]
then
   echo "    Performing processing of Coherentstokes data ONLY" 
fi

if [ $coh_only -eq 1 ] && [ $incoh_only -eq 1 ]
then
   echo "ERROR: Mutually exclusive parameters set coh_only=yes and incoh_only=yes; only one allowed to be turned on"
   exit 1
fi

if [ $transpose -eq 1 ]
then
   echo "    Performing processing on BG/P TAB output data which were run through the 2nd transpose" 
fi

if [ $nofold -eq 1 ]
then
   echo "    Folding has been turned OFF for this pipeline run;  prepfold will be skipped." 
fi

if [ $user_input_location == 1 ]
then
   echo "    Will user user-specified raw data input location: $input_location." 
fi

if [ $user_parset_location == 1 ]
then
   echo "    Will user user-specified parset input location: $parset_location." 
fi

if [ $subsformat == 1 ]
then
   echo "    Will process using presto .subXXXX files instead of default psrfits format." 
fi

#####################################################################
#Check whether Output Processing Location already exists
#####################################################################
if [ $all_pproc == 1 ] || [ $rfi_pproc == 1 ]
then
	if [ ! -d $location ] 
	then
	   echo "ERROR: Output Pre-Processed Location $location does not exist; "
	   echo "       it is required when using the -all_pproc and/or -rfi_pproc flags."
	   exit 1
	fi
    cd $location
else
	if [ -d $location ] && [ $delete -ne 1 ] && [ $incoh_redo -ne 1 ] && [ $coh_redo -ne 1 ] 
	then
	   echo "ERROR: Output Processing Location $location already exists;  "
	   echo "       please specify different output location or try again with delete option (-del)."
	   exit 1
	elif [ -d $location ] && [ $delete -eq 1 ] && [ $incoh_redo -ne 1 ] && [ $coh_redo -ne 1 ]
	then
	   echo "Deleting previous processing run in $location/*"
	   rm -rf $location/*
	   cd $location
	elif [ -d $location/incoherentstokes ] && [ $delete -ne 1 ] && [ $incoh_redo -eq 1 ] 
	then
	   echo "Deleting previous Incoherentstokes processing run in $location/incoherentstokes"
	   rm -rf $location/incoherentstokes
	   cd $location
	elif [ -d $location/stokes ] && [ $delete -ne 1 ] && [ $coh_redo -eq 1 ] 
	then
	   echo "Deleting previous Coherentstokes processing run in $location/stokes"
	   rm -rf $location/stokes
	   cd $location
	else
	   mkdir -p $location 
	   cd $location
	fi
fi # end if [ $all_pproc == 1 ] || [ $rfi_pproc == 1 ]


#In the event that location is a specified relative path, put the absolute path as location
location=`pwd`

#Check that there is disk space in the location to be able to write
touch test
status=$?

if [ $status -ne 0 ]
then
   echo "ERROR: Unable to write to designated location (permissions?);  check if there is enough disk space on device?"
   echo "       Running 'df'..."
   df .
   exit 1
else
   rm test
fi

#Set these parameters by hand
###OBSID=L2010_06296

#####################################################################
#Set up the parset location:
#####################################################################
if [ $all_pproc == 1 ] || [ $rfi_pproc == 1 ]
then
    PARSET=$location/${OBSID}.parset
    if [ ! -f $PARSET ]
    then
	   echo "ERROR: Pre-Processed parset file $PARSET is missing; "
	   echo "       perhaps these data have not yet been processed by the pipeline?"
	   echo "       Previous processing is required when using the -all_pproc and/or -rfi_pproc flags."
	   exit 1
    fi
else
	#Set up the parset location:
	# (1) OLD parset was here: /globalhome/lofarsystem/log/${OBSID}/RTCP.parset.0
	# (2) NEW parset as of May 10, 2010 is here: /globalhome/lofarsystem/log/L2010-MM-DD-DATE/RTCP-ID.parset
	# (3) 2nd transpose parset as of Aug 20, 2010 is here: /globalhome/lofarsystem/production/lofar-trunk/bgfen/log/L2010-MM-DD-DATE/RTCP-ID.parset
	
	#Check if case 1; else case 2
    if [ $user_parset_location == 0 ]
    then
	    PARSET=/globalhome/lofarsystem/log/${OBSID}/RTCP.parset.0
	else
	    PARSET=$parset_location/${OBSID}/RTCP.parset.0
	fi
	has_underscore=`echo $OBSID | grep "_"`
	if [[ $has_underscore != "" ]]
	then
	    short_id=`echo $OBSID | sed 's/L.....//g'`
	else
	    short_id=`echo $OBSID | sed 's/L//g'`
	fi
	
	if [ ! -f $PARSET ] 
	then
	   if [ $user_parset_location == 0 ]
       then
	      new_parset=`find /globalhome/lofarsystem/log/ -name RTCP-${short_id}.parset -print`
	   else
	      new_parset=`find $parset_location/ -name RTCP-${short_id}.parset -print`
	   fi
	   if [[ $new_parset == "" ]]
	   then
	      if [ $user_parset_location == 0 ]
          then
	         new_parset=`find /globalhome/lofarsystem/production/lofar-trunk/bgfen/log/ -name RTCP-${short_id}.parset -print`
	      else
	         new_parset=`find $parset_location/ -name RTCP-${short_id}.parset -print`
	      fi
   	      if [[ $new_parset == "" ]]
	      then
	          # Sept 23, 2010 another parset location added
	          if [ $user_parset_location == 0 ]
	          then
   	             new_parset=/globalhome/lofarsystem/log/L${short_id}/L${short_id}.parset
	          else
	             new_parset=`find $parset_location/ -name L${short_id}.parset -print`
	          fi
			  if [ ! -f $new_parset ] 
			  then
	             echo "ERROR: Unable to find parset for L$short_id in /globalhome/lofarsystem/production/lofar-trunk/bgfen/log/L${short_id} directory"
	             exit 1
	          fi
	      else
		      found_nof_parsets=`echo $new_parset | wc -l | awk '{print $1}'`
		      if (( $found_nof_parsets !=  1 ))
		      then
		         echo "ERROR: Found more than one parset for $short_id in /globalhome/lofarsystem/production/lofar-trunk/bgfen/log/ directory; unable to resolve problem"
		         exit 1
		      fi
	      fi
	   else
	      found_nof_parsets=`echo $new_parset | wc -l | awk '{print $1}'`
	      if (( $found_nof_parsets !=  1 ))
	      then
	         echo "ERROR: Found more than one parset for $short_id in /globalhome/lofarsystem/log/ directory; unable to resolve problem"
	         exit 1
	      fi
	   fi
	   PARSET=$new_parset
	fi
fi # end if [ $all_pproc == 1 ] || [ $rfi_pproc == 1 ]

###PULSAR=B2111+46
#STOKES=incoherentstokes
#STOKES=stokes
###COLLAPSE=              #To collapse put "-C" here

######################
# DON'T CHANGE BELOW #
######################
date_start=`date`

#####################################################################
#Set up generic pipeline version log file
#####################################################################
log=${location}/make_subs_SAS_Ncore_Mmodes.log
if [ $all_pproc == 1 ] || [ $rfi_pproc == 1 ]
then
   echo "-------------------------------------------------------------" >> $log
   echo "Pulsar Pipeline additional-run with: $0" >> $log
   echo "-------------------------------------------------------------" >> $log
else
   echo "Pulsar Pipeline run with: $0" > $log
fi
echo "Pipeline V$VERSION" >> $log
echo "Pipeline V$VERSION" 
echo "$0 $input_string" >> $log
echo "$0 $input_string" 
echo "Start date: $date_start" >> $log
echo "Start date: $date_start" 
echo "PARSET:" $PARSET >> $log
echo "PARSET:" $PARSET


#####################################################################
#Append to the parset some additional relevant info and calc TA beam locations
#####################################################################
hold_pythonpath=`echo $PYTHONPATH`

if [ $all_pproc == 0 ] && [ $rfi_pproc == 0 ]
then
   #unsetenv PYTHONPATH
   export PYTHONPATH=""
   echo "python /home/alexov/LOFAR/RTCP/Run/src/LOFAR/Parset.py $PARSET > ${location}/${OBSID}.parset" >> $log 
   python /home/alexov/LOFAR/RTCP/Run/src/LOFAR/Parset.py $PARSET > ${location}/${OBSID}.parset
   PARSET=${location}/${OBSID}.parset
   #setenv PYTHONPATH $hold_pythonpath
   export PYTHONPATH="$hold_pythonpath"
   
   #set up the beam ra,dec positions based on TA beam offsets, if TA raings > 0
   nrTArings=-1
   nrTArings=`cat $PARSET | grep -i "nrTabRings" | head -1 | awk -F "= " '{print $2}'`
   if [[ $nrTArings > 0 ]]
   then
      nrTiedArrayBeams=`cat $PARSET | grep -i "nrTiedArrayBeams" | head -1 | awk -F "= " '{print $2}'`
      ra_center=`cat $PARSET | grep -i "Observation.Beam\[0\].angle1" | head -1 | awk -F "= " '{print $2}'`
      dec_center=`cat $PARSET | grep -i "Observation.Beam\[0\].angle2" | head -1 | awk -F "= " '{print $2}'`
      ii=1
      while (( $ii < $nrTiedArrayBeams ))
      do
         #get the offsets
         ra_offset=`cat $PARSET | grep -i "Observation.Beam\[0\].TiedArrayBeam\[$ii\].angle1" | head -1 | awk -F "= " '{print $2}'`
         dec_offset=`cat $PARSET | grep -i "Observation.Beam\[0\].TiedArrayBeam\[$ii\].angle2" | head -1 | awk -F "= " '{print $2}'`
         ra_beam=`echo $ra_center $ra_offset | awk '{printf("%17.16f\n",$1 + $2)}'`
         dec_beam=`echo $dec_center $dec_offset | awk '{printf("%18.17f\n",$1 + $2)}'`
         echo "Observation.Beam[$ii].angle1 = $ra_beam" >> $PARSET
         echo "Observation.Beam[$ii].angle2 = $dec_beam" >> $PARSET
         echo "RA (rad) of TiedArrayBeam[$ii] == $ra_beam" >> $log
         echo "Dec (rad) of TiedArrayBeam[$ii] == $dec_beam" >> $log
         echo "RA (rad) of TiedArrayBeam[$ii] == $ra_beam" 
         echo "Dec (rad) of TiedArrayBeam[$ii] == $dec_beam" 
         ii=$(( $ii + 1 ))
      done
   else 
      nrTArings=0
      nrTiedArrayBeams=0
   fi
else
   nrTArings=`cat $PARSET | grep -i "nrTabRings" | head -1 | awk -F "= " '{print $2}'`
   if [[ $nrTArings > 0 ]]
   then
      nrTiedArrayBeams=`cat $PARSET | grep -i "nrTiedArrayBeams" | head -1 | awk -F "= " '{print $2}'`
   else
	   nrTArings=0
	   nrTiedArrayBeams=0
   fi
fi  #end if [ $all_pproc == 0 ] && [ $rfi_pproc == 0 ]


#####################################################################
#Some Parset names changed depending on the observation date;  read parset
#####################################################################
date_obs=`grep Observation.startTime *parset | head -1 | awk -F "= " '{print $2}' | sed "s/'//g"`
date_seconds=`date -d "$date_obs"  "+%s"` 

date1_Apr26=`date -d "2011-04-26 00:00:00" "+%s"`

#name of some parameters changed on Apr 26, 2011
if (( $date_seconds >= $date1_Apr26 ))
then
	INCOHERENTSTOKES=`cat $PARSET | grep -i "Observation.DataProducts.Output_IncoherentStokes.enabled"  | head -1 | awk -F "= " '{print $2}'`
	COHERENTSTOKES=`cat $PARSET | grep -i "Observation.DataProducts.Output_CoherentStokes.enabled"  | head -1 | awk -F "= " '{print $2}'`
else 
	INCOHERENTSTOKES=`cat $PARSET | grep "OLAP.outputIncoherentStokes"  | head -1 | awk -F "= " '{print $2}'`
	COHERENTSTOKES=`cat $PARSET | grep "OLAP.outputCoherentStokes"  | head -1 | awk -F "= " '{print $2}'`
fi

# number of incoherent beams
nrBeams=`cat $PARSET | grep "Observation.nrBeams"  | head -1 | awk -F "= " '{print $2}'`
# number of coherent beams/stations
NBEAMS=`cat $PARSET | grep "OLAP.storageStationNames" | grep -v Control | awk -F '[' '{print $2}' | awk -F ']' '{print $1}'| awk -F\, '{print NF}'`
ARRAY=`cat $PARSET | grep "Observation.bandFilter" | awk -F "= " '{print $2}' | awk -F "_" '{print $1}'`
CHAN=`cat $PARSET | grep "Observation.channelsPerSubband" | awk -F "= " '{print $2}'`
DOWN=`cat $PARSET | grep "OLAP.Stokes.integrationSteps" | grep -v ObservationControl | awk -F "= " '{print $2}'`
MAGIC_NUM=`cat $PARSET | grep "OLAP.CNProc.integrationSteps" | awk -F "= " '{print $2}'`
SAMPLES=`echo ${MAGIC_NUM}/${DOWN}| bc`
FLYSEYE=`cat $PARSET | grep "OLAP.PencilInfo.flysEye" | head -1 | awk -F "= " '{print $2}'`
CHANPFRAME=`cat $PARSET | grep "OLAP.nrSubbandsPerFrame"  | head -1 | awk -F "= " '{print $2}'`
SUBSPPSET=`cat $PARSET | grep "OLAP.subbandsPerPset"  | head -1 | awk -F "= " '{print $2}'`
nSubbands=`cat $PARSET | grep "Observation.subbandList"  | head -1 | awk -F "= " '{print $2}' | sed 's/\[//g' | sed 's/\]//g' | expand_sblist.py |awk -F"," '{print NF}'`
ANT_SHORT=`cat $PARSET | grep "Observation.antennaArray"  | head -1 | awk -F "= " '{print $2}'`
whichStokes=`grep OLAP.Stokes.which *parset | head -1 | awk -F "= " '{print $2}' | sed "s/'//g"`

if [[ $whichStokes != "I" ]]
then
   echo ""
   echo "ERROR: Pipeline can only process Stokes-I data;  parset shows OLAP.Stokes.which = '$whichStokes'; unable to proceed."
   echo "ERROR: Pipeline can only process Stokes-I data;  parset shows OLAP.Stokes.which = '$whichStokes'; unable to proceed." >> $log
   exit 1
fi

if (( $transpose == 0 ))
then
   transpose_par=`cat $PARSET | grep "OLAP.BeamsAreTransposed"  | head -1 | awk -F "= " '{print $2}'`
   if [[ $transpose_par != "True" ]] && [[ $transpose_par != "False" ]]
   then
      echo "There is no 2nd transpose parameter in the parset;  therefore assuming no transpose"
   else
      if [[ $transpose_par == "True" ]]
      then 
         transpose=1
      fi
   fi
fi

## change the nrBeams to start at 1 instead of zero
#nrBeams=$(( $nrBeams + 1 ))
#NBEAMS=$(( $NBEAMS + 1 ))

#if [[ $PULSAR == "position" ]]
#then
#   if [ $nrBeams > 1 ] || [ $NBEAMS > 1 ]
#   then
#      echo "Unable to run pipeline based on input position of multiple beams;  single beam only"
#      echo "Unable to run pipeline based on input position of multiple beams;  single beam only" >> $log
#      exit 1
#   fi
#fi

#####################################################################
#Figure out pulsars in FOV if the -p position input switch was used
#####################################################################
# Pulsar = "position" indicates that the observation is based on a position setting;
# figure out what the brightest/closest N Pulsars to the position (N=3)
if [ $all_pproc == 1 ] || [ $rfi_pproc == 1 ]
then
    # find the pulsar list file
    if [[ -f ${location}/pulsars.list ]]
    then
       num_pulsars=`wc -l ${location}/pulsars.list | awk '{print $1}'`
	   beam_counter=0
	   PULSAR_ARRAY=""
	   while (( $beam_counter < $num_pulsars ))
	   do
		   beam_counter_plus1=$(( $beam_counter + 1 ))
		   
		   line=`sed -n "$beam_counter_plus1,$beam_counter_plus1 p" ${location}/pulsars.list`
		   PULSAR_ARRAY[$beam_counter]=`echo $line`
		   PULSAR_ARRAY_PRIMARY[$beam_counter]=`echo $line | awk '{print $1}'`
	       
		   beam_counter=$(( $beam_counter + 1 ))
        done # end while  (( $beam_counter < $nrBeams ))
    else
        echo "WARNING: Pulsar list file ${location}/pulsars.list not found;  needed for post processing." 
        echo "WARNING: Pulsar list file ${location}/pulsars.list not found;  needed for post processing." >> $log
        echo "         Maybe an older pipeline run, trying to compensate for missing file." 
        echo "         Maybe an older pipeline run, trying to compensate for missing file." >> $log
        PULSAR=`find $location/. -name "*tar.gz" -print | sed 's/.*\///g' | sed 's/_.*//g'`
        PULSAR_LIST=$PULSAR
	    multi_fold=0
	    array_multi_fold[0]=0
	    if [[ $nofold == 1 ]]
	    then 
	       nfolds=0
	    else
	       nfolds=1
        fi
	    array_nfolds[0]=$nfolds
	    PULSAR_ARRAY[0]=$PULSAR_LIST
	    PULSAR_ARRAY_PRIMARY[0]=$PULSAR
    fi
else # [ $all_pproc == 1 ] || [ $rfi_pproc == 1 ]
	beam_counter=0
	PULSAR_ARRAY=""
	PULSAR_input=$PULSAR
	while (( $beam_counter < $nrBeams ))
	do
		is_3c=`echo $PULSAR_input | grep -i 3C`
		if [[ $PULSAR_input == "position" ]]
		then
		    pi=$(echo "scale=10; 4*a(1)" | bc -l)
			RRA=`cat $PARSET | grep "Observation.Beam\[$beam_counter\].angle1"  | head -1 | awk -F "= " '{print $2}'`
			RDEC=`cat $PARSET | grep "Observation.Beam\[$beam_counter\].angle2"  | head -1 | awk -F "= " '{print $2}'`
		    echo "Position $RRA $RDEC (radians) will be used to find a pulsar in the field # $beam_counter"
		    echo "Position $RRA $RDEC (radians) will be used to find a pulsar in the field # $beam_counter"  >> $log
		    RA_DEG=`echo "scale=10; $RRA / $pi * 180.0" | bc | awk '{printf("%3.9f\n",$1)}'`
		    DEC_DEG=`echo "scale=10; $RDEC / $pi * 180.0" | bc | awk '{printf("%3.9f\n",$1)}'`
		    echo "   Position is $RA_DEG $DEC_DEG (deg) in field # $beam_counter"
		    echo "   Position is $RA_DEG $DEC_DEG (deg) in field # $beam_counter" >> $log
		    get_list=`pulsars_at_location.sh $RA_DEG $DEC_DEG $ANT_SHORT 3 | tail -1`
		    if [[ $get_list == "ERROR" ]] || [[ $get_list == "NONE" ]]
		    then
		       echo "   WARNING: Unable to find Pulsar in FOV of pointing ($RA_DEG $DEC_DEG); partial pipeline run."
		       echo "   WARNING: Unable to find Pulsar in FOV of pointing ($RA_DEG $DEC_DEG); partial pipeline run." >> $log
		       PULSAR="NONE"
		       PULSAR_LIST="NONE"
		       PULSAR_ARRAY[$beam_counter]="NONE"
		    else
		       PULSAR=$get_list
		       PULSAR_LIST=$get_list
		    fi
		elif [[ $is_3c != "" ]]
		then
		    if [[ ! -f $LOFARSOFT/release/share/pulsar/data/3cCatalog_psr$ANT_SHORT.txt ]]
		    then
		       echo "ERROR: Unable to find cross-match catalog $LOFARSOFT/release/share/pulsar/data/3cCatalog_psr$ANT_SHORT.txt"
		       echo "       Try performing an SVN update and 'make scripts_install' in $LOFARSOFT/build/pulsar"
		       echo "ERROR: Unable to find cross-match catalog $LOFARSOFT/release/share/pulsar/data/3cCatalog_psr$ANT_SHORT.txt" >> $log
		       echo "       Try performing an SVN update and 'make scripts_install' in $LOFARSOFT/build/pulsar"  >> $log
		       exit 1
		    fi
		    matched_str=`grep "$PULSAR " $LOFARSOFT/release/share/pulsar/data/3cCatalog_psr$ANT_SHORT.txt | awk '{print $4}'`
		    if [[ $matched_str == "NONE" ]]
		    then
		       echo "WARNING: No known pulsars found in $ANT_SHORT FOV of $PULSAR;  this is a partial pipeline run."
		       echo "WARNING: No known pulsars found in $ANT_SHORT FOV of $PULSAR;  this is a partial pipeline run." >> $log
		       PULSAR="NONE"
		       PULSAR_LIST="NONE"
		       PULSAR_ARRAY[$beam_counter]="NONE"
		    else
		       PULSAR=`echo $matched_str | awk -v MAX=3 -F"," '{ for (i=1; i<=MAX; i++) printf "%s,", $i; printf "\n"; }' | sed 's/,,*$//g' | sed 's/,$//g'`
		       PULSAR_LIST=$PULSAR
		    fi
		else 
		    echo "   Assuming input is a Pulsar name: $PULSAR_input"
		    echo "   Assuming input is a Pulsar name: $PULSAR_input" >> $log
		    PULSAR=$PULSAR_input
		    PULSAR_LIST=$PULSAR_input
		    PULSAR_ARRAY[$beam_counter]=$PULSAR_input
		fi
		
		is_psr_list=`echo $PULSAR_LIST | grep ","`
		if [[ $is_psr_list != "" ]]
		then
		   multi_fold=1
		   array_multi_fold[$beam_counter]=1
		   PULSAR=`echo $PULSAR_LIST | awk -F"," '{print $1}'`
		   if [[ $nofold == 1 ]]
		   then 
		      nfolds=0
		   else
		      nfolds=1
		   fi
#		   nfolds=`echo $PULSAR_LIST | awk -F"," '{print NF}'`
		   array_nfolds[$beam_counter]=$nfolds
		   PULSAR_LIST=`echo $PULSAR_LIST | sed 's/\,/ /g'`
		   PULSAR_ARRAY[$beam_counter]=$PULSAR_LIST
		   PULSAR_ARRAY_PRIMARY[$beam_counter]=$PULSAR
		else
		   multi_fold=0
		   array_multi_fold[$beam_counter]=0
		   PULSAR=$PULSAR_LIST
		   if [[ $nofold == 1 ]]
		   then 
		      nfolds=0
		   else
		      nfolds=1
		   fi
		   array_nfolds[$beam_counter]=$nfolds
		   PULSAR_ARRAY[$beam_counter]=$PULSAR_LIST
		   PULSAR_ARRAY_PRIMARY[$beam_counter]=$PULSAR
		fi
	    
	    # get the subbands per beam
	    array_nSubbands[$beam_counter]=`cat $PARSET | grep "Observation.Beam\[$beam_counter\].subbandList"  | head -1 | awk -F "= " '{print $2}' | sed 's/\[//g' | sed 's/\]//g' | expand_sblist.py |awk -F"," '{print NF}'`
	    array_SubbandList[$beam_counter]=`cat $PARSET | grep "Observation.Beam\[$beam_counter\].subbandList"  | head -1 | awk -F "= " '{print $2}'`
	
		beam_counter=$(( $beam_counter + 1 ))
	done # end (( $beam_counter < $nrBeams ))
	
	beam_counter=0
	while (( $beam_counter < $nrBeams ))
	do
	    echo "Main PULSAR(s) in field # $beam_counter:" ${PULSAR_ARRAY[beam_counter]}
	    echo "Main PULSAR(s) in field # $beam_counter:" ${PULSAR_ARRAY[beam_counter]} >> $log
	    echo ${PULSAR_ARRAY[beam_counter]} >> ${location}/pulsars.list
		if [[ ${array_multi_fold[$beam_counter]} == 1 ]]
		then
		   echo "   Will perform multiple folding for the following ${array_nfolds[$beam_counter]} Pulsars: ${PULSAR_ARRAY[$beam_counter]}"
		fi
		beam_counter=$(( $beam_counter + 1 ))
	done
fi #end if [ $all_pproc == 1 ] || [ $rfi_pproc == 1 ]

#####################################################################
#Echo all the major parameters to stdout and log
#####################################################################
echo "ARRAY:" $ARRAY 
echo "ARRAY:" $ARRAY >> $log
echo "CHANNELS:" $CHAN 
echo "CHANNELS:" $CHAN >> $log
echo "Number of SAMPLES:" $SAMPLES
echo "Number of SAMPLES:" $SAMPLES >> $log
echo "Number of Channels per Frame:" $CHANPFRAME
echo "Number of Channels per Frame:" $CHANPFRAME >> $log
echo "Number of Subbands per Pset:" $SUBSPPSET 
echo "Number of Subbands per Pset:" $SUBSPPSET >> $log
echo "Number of Subbands:" $nSubbands 
echo "Number of Subbands:" $nSubbands >> $log
echo "Incoherentstokes set to:" $INCOHERENTSTOKES
echo "Incoherentstokes set to:" $INCOHERENTSTOKES >> $log
echo "Coherentstokes set to:" $COHERENTSTOKES
echo "Coherentstokes set to:" $COHERENTSTOKES >> $log
echo "FlysEye set to:" $FLYSEYE 
echo "FlysEye set to:" $FLYSEYE >> $log

echo "Starting Time"
echo "Starting Time" >> $log
date
date >> $log


#####################################################################
#Check the modes against what was requested by the user for processing
#####################################################################
#Default is incoherentstokes
mode_str="incoherentstokes"
nmodes=1
if [[ $INCOHERENTSTOKES == 'true' ]] && [[ $COHERENTSTOKES == 'true' ]] && [[ $incoh_only == 0 ]] && [[ $coh_only == 0 ]] && [[ $incoh_redo == 0 ]] && [[ $coh_redo == 0 ]]
then
   mode_str="stokes incoherentstokes"
   nmodes=2
   echo "Incoherentstokes and coherentstokes modes for processing."
   echo "Incoherentstokes and coherentstokes modes for processing." >> $log
elif [[ $INCOHERENTSTOKES == 'true' ]] && [[ $COHERENTSTOKES == 'true' ]] && [[ $incoh_only == 0 ]] && [[ $coh_only == 0 ]] && [[ $incoh_redo == 1 ]] && [[ $coh_redo == 0 ]]
then
   mode_str="incoherentstokes"
   nmodes=1
   echo "Incoherentstokes re-do for processing."
   echo "Incoherentstokes re-do for processing." >> $log
elif [[ $INCOHERENTSTOKES == 'true' ]] && [[ $COHERENTSTOKES == 'true' ]] && [[ $incoh_only == 0 ]] && [[ $coh_only == 0 ]] && [[ $incoh_redo == 0 ]] && [[ $coh_redo == 1 ]]
then
   mode_str="stokes"
   nmodes=1
   echo "Coherentstokes re-do for processing."
   echo "Coherentstokes re-do for processing." >> $log
elif [[ $INCOHERENTSTOKES == 'true' ]] && [[ $COHERENTSTOKES == 'true' ]] && [[ $incoh_only == 0 ]] && [[ $coh_only == 0 ]] && [[ $incoh_redo == 1 ]] && [[ $coh_redo == 1 ]]
then
   mode_str="stokes incoherentstokes"
   nmodes=2
   echo "Incoherentstokes (re-do) and coherentstokes (re-do) modes for processing."
   echo "Incoherentstokes (re-do) and coherentstokes (re-do) modes for processing." >> $log
elif [[ $INCOHERENTSTOKES == 'true' ]] && [[ $COHERENTSTOKES == 'true' ]] && [[ $incoh_only == 1 ]] && [[ $coh_only == 0 ]]
then
   mode_str="incoherentstokes"
   nmodes=1
   echo "Incoherentstokes ONLY mode for processing."
   echo "Incoherentstokes ONLY mode for processing." >> $log
elif [[ $INCOHERENTSTOKES == 'true' ]] && [[ $COHERENTSTOKES == 'true' ]] && [[ $incoh_only == 0 ]] && [[ $coh_only == 1 ]]
then
   mode_str="stokes"
   nmodes=1
   echo "Coherentstokes ONLY mode for processing."
   echo "Coherentstokes ONLY mode for processing." >> $log
elif [[ $INCOHERENTSTOKES == 'false' ]] && [[ $COHERENTSTOKES == 'true' ]] && [[ $incoh_only == 1 ]] && [[ $coh_only == 0 ]]
then
   echo "ERROR: Coherentstokes = true, but user specified Incoherentstokes processing only; no IS data to process."
   proc=-2
elif [[ $INCOHERENTSTOKES == 'true' ]] && [[ $COHERENTSTOKES == 'false' ]] && [[ $incoh_only == 0 ]] && [[ $coh_only == 1 ]]
then
   echo "ERROR: Incoherentstokes = true, but user specified Coherentstokes processing only; no CS data to process."
   proc=-2
elif [[ $INCOHERENTSTOKES == 'false' ]] && [[ $COHERENTSTOKES == 'true' ]]
then
   mode_str="stokes"
   nmodes=1
   echo "Coherentstokes (only) mode for processing."
   echo "Coherentstokes (only) mode for processing." >> $log
elif [[ $INCOHERENTSTOKES == 'true' ]] && [[ $COHERENTSTOKES == 'false' ]]
then
   mode_str="incoherentstokes"
   nmodes=1
   echo "Incoherentstokes (only) mode for processing."
   echo "Incoherentstokes (only) mode for processing." >> $log
else
   echo "ERROR: Unable to determine stokes type from parset, Incoherentstokes or Coherentstokes must be True for processing."
   echo "ERROR: Unable to determine stokes type from parset, Incoherentstokes or Coherentstokes must be True for processing." >> $log
   echo "       This Pulsar Pipeline is unable to run on the requested dataset."
   echo "       This Pulsar Pipeline is unable to run on the requested dataset." >> $log
   proc=-2
fi

# clean up if exiting early
if (( $proc == -2 ))
then
    cd $location/..
    echo "ERROR: No processing performed;  cleaning up."
    if [[ -d $location ]]
    then 
       rm -rf $location
    fi
    exit 1
fi

#Non-Flys Eye (flyseye=0);  Flys Eye (flyseye=1)
flyseye=0
flyseye_tar=0
if [[ $COHERENTSTOKES == 'true' ]]
then
	if [[ $FLYSEYE == 'true' ]]
	then
	   flyseye=1
	   flyseye_tar=1
	fi
fi
TiedArray=0

user_core=$core
#nrBeams_plus1=$(( $nrBeams + 1 ))
#NBEAMS_plus1=$(( $NBEAMS + 1 ))

###############################################################################
# Main program loop over "incoherentstokes" and coherentstokes/"stokes" strings
###############################################################################
for modes in $mode_str
do

    STOKES=$modes

	if [[ $transpose == 1 ]] && [[ $STOKES == 'incoherentstokes' ]] 
	then
        date1_Sept20=`date -d "2010-09-20 00:00:00" "+%s"`
        date2_Oct26=`date -d "2010-10-26 00:00:00" "+%s"`
       
        #ignore transpose parameter for incoherentstokes outside the date range when it was actually needed
        if (( $date_seconds < $date1_Sept20 ))
        then
           transpose=0
        elif (( $date_seconds > $date2_Oct26 ))
        then 
           transpose=0
        fi
    fi

	if [[ $transpose == 1 ]] && [[ $STOKES == 'stokes' ]] && [[ $flyseye == 1 ]] && [[ $user_core != $NBEAMS ]] 
	then
	   echo "WARNING: 2nd Transpose CoherentStokes must have N cores = # of stations;  resetting user-specified core to $NBEAMS."
	   echo "WARNING: 2nd Transpose CoherentStokes must have N cores = # of stations;  resetting user-specified core to $NBEAMS." >> $log
       core=$NBEAMS
	elif [[ $transpose == 1 ]] && [[ $STOKES == 'stokes' ]] && [[ $flyseye == 0 ]] && [[ $user_core != $nrBeams ]]
	then
	   echo "WARNING: 2nd Transpose CoherentStokes must have N cores = # of beams;  resetting user-specified core to $nrBeams."
	   echo "WARNING: 2nd Transpose CoherentStokes must have N cores = # of beams;  resetting user-specified core to $nrBeams." >> $log
	   core=$nrBeams
	elif [[ $transpose == 1 ]] && [[ $STOKES == 'stokes' ]] && [[ $flyseye == 1 ]] && [[ $NBEAMS -gt 128 ]]
	then
	   echo "ERROR: Pipeline is currently unable to handle more than 128 beams FE mode;  there are $NBEAMS in this observation."
	   echo "ERROR: Pipeline is currently unable to handle more than 128 beams FE mode;  there are $NBEAMS in this observation."  >> $log
	   exit 1
	elif [[ $STOKES == 'incoherentstokes' ]] && [[ $nrBeams -gt 1 ]] && [[ $nrBeams -le 128 ]] 
	then
	   echo "WARNING: 2nd Transpose IncoherentStokes must have N cores = # of beams;  resetting user-specified core to $nrBeams."
	   echo "WARNING: 2nd Transpose IncoherentStokes must have N cores = # of beams;  resetting user-specified core to $nrBeams." >> $log
	   core=$nrBeams
	elif [[ $STOKES == 'incoherentstokes' ]] && [[ $NBEAMS -gt 1 ]] && [[ $subsformat == 0 ]]
	then
	   echo "WARNING: IS mode with psrfits, N cores must be set to 1."
	   echo "WARNING: IS mode with psrfits, N cores must be set to 1." >> $log
	   core=1
	elif [[ $STOKES == 'incoherentstokes' ]] && [[ $nrBeams -gt 128 ]]
	then
	   echo "ERROR: Pipeline is currently unable to handle more than 128 Incoherent beams;  there are $nrBeams in this observation."
	   echo "ERROR: Pipeline is currently unable to handle more than 128 Incoherent beams;  there are $nrBeams in this observation."  >> $log
	   exit 1
	else 
	   core=$user_core
	fi

    if [ $all_pproc == 0 ] && [ $rfi_pproc == 0 ]
    then
       mkdir -p ${STOKES}
    fi
    # STOKES mode="stokes" always comes first, to do fly's eye mode before incoherentstokes;
    # reset fly's eye variable to 0 when mode=incoherentstokes (after fly's eye is done)
    if [[ $STOKES == "incoherentstokes" ]]
    then
       flyseye=0
    fi 

    #due to some logic problems, the easiest way to quickly fix a bug is to reset fly's eye to 0 for ALL incoherentstokes data
#    if [[ $FLYSEYE == 'true' ]] && [[ $STOKES == 'incoherentstokes' ]]
#    then
#        flyseye=0
#    fi
    
    echo "Starting work on $modes files."
    echo "Starting work on $modes files." >> $log
    date
    date >> $log

    # Get the list of input files; and check if divisible by N cores
	master_list=${STOKES}/SB_master.list
	
	#A2 TBD: test/check this logic for post processing
	if [ $all_pproc == 1 ] || [ $rfi_pproc == 1 ]
	then
	    # find the number of RSP? directories;  this is the number of cores used previously for processing
	    if [ -f $location/${STOKES}/nof_cores.txt ]
	    then
	       core=`cat $location/${STOKES}/nof_cores.txt`
	    elif [ $flyseye == 0 ]
	    then
	       core=`ls -F $location/${STOKES} | grep -w 'RSP[0-7].' | wc -l`
	    else
	       core=`ls -F $location/${STOKES}/*/* | egrep -w 'RSP[0-7].|beam_[0-7].' | sed 's/^.*\///' | sort | uniq | wc -l`	    	    
	    fi
	    
	    if [ $core -lt 1 ]
	    then
	       echo "ERROR: cannot determine the number of cores used in previous processing in $location/${STOKES}"
	       echo "ERROR: cannot determine the number of cores used in previous processing in $location/${STOKES}" >> $log
	       exit 1
	    fi
	    echo "These data were previously processed with $core cores, based on number of RSP-directories"
	    echo "These data were previously processed with $core cores, based on number of RSP-directories" >> $log
		all_num=`wc -l $master_list | awk '{print $1}'`
	else
		#Create subband lists
		if [[ $transpose == 0 ]] 
		then	
		    if [ $user_input_location == 0 ]
		    then 	    
			   all_list=`ls /net/sub?/lse*/data?/${OBSID}/*.${STOKES} | sort -t B -g -k 2`
#A2test			all_list=`cat /data4/2nd_transpose/L2010_21488_red_freqwrong/incoherentstokes/SB_master.list`
		#XXX	all_list=`ls /net/sub[456]/lse01[35]/data?/${OBSID}/SB*.MS.${STOKES} | sort -t B -g -k 2`
			   ls /net/sub?/lse*/data?/${OBSID}/*.${STOKES} | sort -t B -g -k 2 > $master_list
#A2test			cp /data4/2nd_transpose/L2010_21488_red_freqwrong/incoherentstokes/SB_master.list $master_list
		#XXX	ls /net/sub[456]/lse01[35]/data?/${OBSID}/SB*.MS.${STOKES} | sort -t B -g -k 2 > $master_list
		    else
			   all_list=`ls $input_location/${OBSID}/*.${STOKES} | sort -t B -g -k 2`
			   ls $input_location/${OBSID}/*.${STOKES} | sort -t B -g -k 2 > $master_list
		    fi
	    else

		    if [[ $STOKES == "incoherentstokes" ]]
		    then
		       fname="SB*incoh*"
		    elif [[ $STOKES == "stokes" ]]
		    then
		       fname="*bf.raw"
		    else
		       echo "ERROR: Unable to determine the file naming convension - not incoherent or coherent stokes" 
		       echo "ERROR: Unable to determine the file naming convension - not incoherent or coherent stokes" >> $log
		       exit 1
		    fi

		    if [ $user_input_location == 0 ]
		    then 	    
			   all_list=`ls /net/sub?/lse*/data?/${OBSID}/L${short_id}_${fname} | sort -t B -g -k 2`
			   ls /net/sub?/lse*/data?/${OBSID}/L${short_id}_${fname} | sort -t B -g -k 2 > $master_list
			   ls /net/sub?/lse*/data?/${OBSID}/L${short_id}_*h5 > $location/h5.list
			else
			   all_list=`ls $input_location/${OBSID}/L${short_id}_${fname} | sort -t B -g -k 2`
			   ls $input_location/${OBSID}/L${short_id}_${fname} | sort -t B -g -k 2 > $master_list
			   ls $input_location/${OBSID}/L${short_id}_*h5 > $location/h5.list
			fi
			
			#check that CS raw data files are non-zero length
		    if [[ $STOKES == "stokes" ]]
		    then
		       while read rawname
		       do
		          filesize=`/bin/ls -l $rawname | awk '{print $5}'`
		          if (( $filesize == 0 ))
		          then
		             echo "ERROR: Input raw data ($rawname) are zero length;  unable to continue." >> $log
		             echo "ERROR: Input raw data ($rawname) are zero length;  unable to continue." 
		             echo "Changing permissions of files"

					 echo "Changing permissions of files" >> $log
					 date
					 date >> $log
					 echo chmod 775 -R . * >> $log
					 chmod 775 -R . * 
#					 echo chgrp -R pulsar . * >> $log
#					 chgrp -R pulsar . * 

		             exit 1
		          fi
		       done < $master_list
		    fi
		    #check for H5 file
		    count_h5=0
		    count_h5=`wc -l $location/h5.list | awk '{print $1}'`
		    if (( $count_h5 > 0 ))
		    then
		       H5_exist=1
		       echo "Found H5 file; therefore, using new CS format logic"
		       echo "Found H5 file; therefore, using new CS format logic" >> $log
		    fi 
	    fi  #end if [ $all_pproc == 1 ] || [ $rfi_pproc == 1 ]
	    
	    nodename=`uname -n`
	    is_cep2=`echo $nodename | grep locus`
	    create_dummies=0
   		if [[ $STOKES == "incoherentstokes" ]] && [[ $is_cep2 != "" ]]
   		then
              # on cep2, there are missing subbands;  create dummy files for missing numbers
              nfiles=`grep Observation.beamList $PARSET | head -1 | awk -F "= " '{print $2}' | sed -e 's/\[//g' -e 's/\*.*//g'`
              master_list_wc=`wc -l $master_list | awk '{print $1}`
              if (( $master_list_wc < $nfiles ))
              then
                  echo "WARNING: number of IS files is less than number requested in observation;  creating dummy files to compensate"
                  echo "WARNING: number of IS files is less than number requested in observation;  creating dummy files to compensate"  >> $log
                  ii=0
                  if [[ -d /data/$OBSID ]]
                  then 
                     rm -rf /data/$OBSID
                  fi
                  mkdir /data/$OBSID
	              rootname=`head -1 $master_list | sed -e "s/\// /g" -e "s/.* //g" -e "s/_SB.*//g"`
	              while (( $ii < $nfiles ))
	              do
	                 create_dummies=1
	                 sb=`echo $ii | awk '{printf("%03d",$1)}'`
	                 filename=`grep _SB$sb $master_list`
	                 #sb=`echo $filename | sed -e "s/^.*_SB/ /g" -e "s/_.*//g"`
	                 if [[ $filename == "" ]]
	                 then
	                     echo "WARNING: missing subband #$ii; creating dummy."
	                     echo "WARNING: missing subband #$ii; creating dummy." >> $log
	                     filename=`echo $rootname $ii | awk '{printf("%s_SB%03d_bf.incoherentstokes\n", $1, $2)}'`
	                     touch /data/$OBSID/$filename
	                 else 
	                     ln -s $filename /data/$OBSID/
	                 fi  
	                 ii=$(( $ii + 1 )) 
	              done 
	           chmod -R 777 /data/$OBSID
		       ls /data/$OBSID/*incoherentstokes | sort -t B -g -k 2 > $master_list
		       echo "Created new master list" 
		       echo "Created new master list" >> $log
		    fi
        fi

        all_num=`wc -l $master_list | awk '{print $1}'`
		echo "Found a total of $all_num ${STOKES} input datafiles to process" 
		echo "Found a total of $all_num ${STOKES} input datafiles to process" >> $log

		if [[ $all_num == 0 ]]
		then
		   echo "ERROR: no input files found, for this mode."
		   echo "ERROR: no input files found, for this mode." >> $log
		   continue
		else
		   proc=1
		fi
		
        if [[ $transpose == 1 ]] && [[ $STOKES == "stokes" ]] && [[ $flyseye == 1 ]]
        then
           if (( $all_num != $NBEAMS ))
           then
              echo "WARNING: cannot find all beams as noted in the parset;  continuing with number of files found."
              echo "WARNING: cannot find all beams as noted in the parset;  continuing with number of files found." >> $log
              NBEAMS=$all_num
              echo "NOTE: core was set to $core, being reset to $all_num to match number of input raw files found."
              core=$all_num
           else
              all_num=$NBEAMS
              echo "Able to find correct number of raw files for CS FE as noted in the parset."
              echo "Able to find correct number of raw files for CS FE as noted in the parset." >> $log
           fi
  		   echo "2nd transpose FE data has $nSubbands subbands each in $all_num files" 
  		   echo "2nd transpose FE data has $nSubbands subbands each in $all_num files" >> $log
		elif [[ $transpose == 1 ]] && [[ $STOKES == "stokes" ]] && [[ $flyseye == 0 ]]
        then
           if (( $nrBeams == 1 )) && (( $all_num > 1 ))
           then
               TiedArray=1
               flyseye=1
               flyseye_tar=1
               NBEAMS=$all_num
	  		   echo "TiedArray Beams: 2nd transpose has $nSubbands subbands in $all_num files" 
	  		   echo "TiedArray Beams: 2nd transpose has $nSubbands subbands in $all_num files" >> $log
           else
	           all_num=$nrBeams
	  		   echo "2nd transpose has $nSubbands subbands in $all_num files" 
	  		   echo "2nd transpose has $nSubbands subbands in $all_num files" >> $log
	  	   fi
        fi

		if [ $all_num -lt $core ]
		then
		  echo "ERROR: Less than $core subbands found, unlikely to be a valid observation"
		  echo "ERROR: Less than $core subbands found, unlikely to be a valid observation" >> $log
		  exit 1
		fi
        
		modulo_files=`echo $all_num $core | awk '{print ($1 % $2)}'`
		# If the number of cores does not evenly divide into the # of files, then find another good value
		if [[ $modulo_files -ne 0 ]] && [[ $nrBeams -gt 1 ]] && [[ $STOKES == "incoherentstokes" ]]
           then 
		   echo "ERROR: For multi-beam IS mode, the number of files ($all_num) must divide by the number of beams ($core)!"
		   echo "       There seems to be missing data or an error in the parset or pipeline;  cannot continue."
		   echo "ERROR: For multi-beam IS mode, the number of files ($all_num) must divide by the number of beams ($core)!" >> $log
		   echo "       There seems to be missing data or an error in the parset or pipeline;  cannot continue."  >> $log
           exit            
        elif (( $modulo_files != 0 )) 
		then
		   echo "WARNING: User requested $core cores; this does not evently divide into $all_num number of files"
		   echo "WARNING: User requested $core cores; this does not evently divide into $all_num number of files" >> $log
		   echo "         Searching for alternative N core value..."
		   echo "         Searching for alternative N core value..." >> $log
		   ii=`expr $core - 1`
		   while (( $ii > 0 ))
		   do 
		       modulo_files=`echo $all_num $ii | awk '{print ($1 % $2)}'`
		       if (( $modulo_files == 0 ))
		       then
		          echo "Success: $ii cores divides into $all_num subbands"
		          break
		       else
		          echo "Tried $ii cores, but still not divisble into $all_num" >> $log
		          echo "Tried $ii cores, but still not divisble into $all_num"
		       fi
		       ii=`expr $ii - 1`
		   done
		   echo "WARNING: Resetting user requested number of cores from $core to $ii for processing of $all_num subbands"
		   echo "WARNING: Resetting user requested number of cores from $core to $ii for processing of $all_num subbands" >> $log
		   core=$ii

		fi # [[ $nrBeams == 1 ]]
        		    
		div_files=`echo $all_num $core | awk '{print $1 / $2}'`
		count=0
		
		#Create N-split sections of the file list
#		echo split -a 1 -d -l $div_files $master_list ${STOKES}/$$"_split_"
#		echo split -a 1 -d -l $div_files $master_list ${STOKES}/$$"_split_" >> $log
#		split -a 1 -d -l $div_files $master_list ${STOKES}/$$"_split_"

        if (( $TiedArray == 1 )) 
        then 
			echo split -a 3 -d -l 1 $master_list ${STOKES}/$$"_split_"
			echo split -a 3 -d -l 1 $master_list ${STOKES}/$$"_split_" >> $log
			split -a 3 -d -l 1 $master_list ${STOKES}/$$"_split_"
        else
			echo split -a 3 -d -l $div_files $master_list ${STOKES}/$$"_split_"
			echo split -a 3 -d -l $div_files $master_list ${STOKES}/$$"_split_" >> $log
			split -a 3 -d -l $div_files $master_list ${STOKES}/$$"_split_"
        fi
        
		status=$?
		
		if [ $status -ne 0 ]
		then
		   echo "ERROR: 'split' command unable to split ($all_num files/$core cores) into $div_files chunks each (not integer number);"
		   echo "       you may need to run with a different number of cores which divide $all_num files evenly"
		   echo "ERROR: 'split' command unable to split ($all_num files/$core cores) into $div_files chunks each (not integer number);" >> $log
		   echo "       you may need to run with a different number of cores which divide $all_num files evenly"  >> $log
		
		   exit 1
		fi
    fi # end if [ $all_pproc == 1 ] || [ $rfi_pproc == 1 ]

    # Set the number of working directories (string), based on the number of cores set
    num_dir=""
    min=0
    max=$core
    ii=0
    while (( $ii < $max ))
    do
       num_dir=`echo $num_dir " " $ii`
       last=$ii
       ii=$(( $ii + 1 )) 
    done
		
	#Set up the strings for the beam names when in fly's eye mode;  max NBEAMS beams currently allowed
    beams=""
	beams_init="beam_0"
	if (( $flyseye == 1 )) 
	then
		min=0
	    max=$NBEAMS
	    ii=0
	    while (( $ii < $max ))
	    do
	       beams=`echo $beams " beam_"$ii`
	       last_beam="beam_"$ii
	       ii=$(( $ii + 1 )) 
	    done
    fi

    # if this is post processing FE, then the beams were renamed to the station names;  get the station names
	if (( $flyseye == 1 )) && (( (( $all_pproc == 1 )) || (( $rfi_pproc == 1 )) ))
	then
       beams_tmp=""
	   for jjj in $beams
		  do
			N=`echo $jjj | awk -F "_" '{print $2}'`
			N=`echo "$N+1" | bc`
			NAME=`cat $PARSET| grep "OLAP.storageStationNames" | awk -F '[' '{print $2}' | awk -F ']' '{print $1}'| awk -F "," '{print $'$N'}'`
			beams_tmp="$beams_tmp $NAME"
			last_beam=$NAME
	   done
	   beams=$beams_tmp
    fi
    
	#Create directories with appropriate permissions (beams are ignored/blank when not needed)
    if [ $all_pproc == 0 ] && [ $rfi_pproc == 0 ]
    then
		if (( $flyseye == 0 ))
		then
		   for ii in $num_dir
	       do
	          mkdir -p ${STOKES}/RSP$ii
	       done	
		else
	       for ii in $num_dir
		   do
	#	       for jjj in $beams
	#	       do
	#	          mkdir -p ${STOKES}/RSP$ii/${jjj}
		          mkdir -p ${STOKES}/RSP$ii
		          if (( $TiedArray == 1 ))
		          then
		              for jjj in $beams
		              do 
		                  mkdir -p ${STOKES}/RSP${ii}/${jjj}
		              done
		          fi
	#	       done
		   done
		fi
	
		if [[ $core == 1 ]] && [[ $STOKES == "stokes" ]]
		then
		   if [[ $all == 1 ]]
		   then
  		      echo "WARNING: turning off 'all' processing since number of cores/slipts is 1 and therefore all are done together in RSP0"
		      echo "WARNING: turning off 'all' processing since number of cores/slipts is 1 and therefore all are done together in RSP0" >> $log
		      all=0
		   fi
		   if [[ $all_pproc == 1 ]]
		   then 
  		      echo "WARNING: turning off 'all' processing since number of cores/slipts is 1 and therefore all are done together in RSP0"
		      echo "WARNING: turning off 'all' processing since number of cores/slipts is 1 and therefore all are done together in RSP0" >> $log
		      all_pproc=0
		   fi
		elif [[ $all == 0 ]] && [[ $core != 1 ]] && [[ $all_orig == 1 ]] && [[ $STOKES == "incoherentstokes" ]]
		then
		   # turn all processing back on if it was turned off for stokes processing
  		   echo "Turning ON 'all' processing for mode $STOKES."
		   echo "Turning ON 'all' processing for mode $STOKES." >> $log
		   all=1
		elif [[ $all_pproc == 0 ]] && [[ $core != 1 ]] && [[ $all_pproc_orig == 1 ]] && [[ $STOKES == "incoherentstokes" ]]
		then
		   # turn all_pproc processing back on if it was turned off for stokes processing
  		   echo "Turning ON 'all' processing for mode $STOKES."
		   echo "Turning ON 'all' processing for mode $STOKES." >> $log
		   all_pproc=1
		fi
		
		chmod -R 775 . * 
		#chgrp -R pulsar . *
    fi # end if [ $all_pproc == 0 ] && [ $rfi_pproc == 0 ]
 	
    #Move the split lists to the appropriate directories
	if (( $TiedArray == 0 ))
	then
	    if [ $all_pproc == 0 ] && [ $rfi_pproc == 0 ]
	    then
			for ii in $num_dir
			do
			  if (( $ii < 10 ))
			  then 
			     echo mv ${STOKES}/$$"_split_00"$ii ${STOKES}/"RSP"${ii}/"RSP"${ii}".list" >> $log
			     mv ${STOKES}/$$"_split_00"$ii ${STOKES}/"RSP"${ii}/"RSP"${ii}".list"
			  elif (( $ii < 100 ))
			  then
			     echo mv ${STOKES}/$$"_split_0"$ii ${STOKES}/"RSP"${ii}/"RSP"${ii}".list" >> $log
			     mv ${STOKES}/$$"_split_0"$ii ${STOKES}/"RSP"${ii}/"RSP"${ii}".list"
			  else
			     echo mv ${STOKES}/$$"_split_"$ii ${STOKES}/"RSP"${ii}/"RSP"${ii}".list" >> $log
			     mv ${STOKES}/$$"_split_"$ii ${STOKES}/"RSP"${ii}/"RSP"${ii}".list"
			  fi
			done
		fi
	else 
	    for ii in $num_dir
		do
	        counter=0
			for jjj in $beams
			do
			  if (( $counter < 10 ))
			  then 
				  echo cp ${STOKES}/$$"_split_00"$counter ${STOKES}/"RSP"${ii}/${jjj}/"RSP"${ii}".list" >> $log
				  cp ${STOKES}/$$"_split_00"$counter ${STOKES}/"RSP"${ii}/${jjj}/"RSP"${ii}".list"
			  elif (( $counter < 100 ))
			  then 
				  echo cp ${STOKES}/$$"_split_0"$counter ${STOKES}/"RSP"${ii}/${jjj}/"RSP"${ii}".list" >> $log
				  cp ${STOKES}/$$"_split_0"$counter ${STOKES}/"RSP"${ii}/${jjj}/"RSP"${ii}".list"
			  else
				  echo cp ${STOKES}/$$"_split_"$counter ${STOKES}/"RSP"${ii}/${jjj}/"RSP"${ii}".list" >> $log
				  cp ${STOKES}/$$"_split_"$counter ${STOKES}/"RSP"${ii}/${jjj}/"RSP"${ii}".list"			  
			  fi
		      counter=$(( $counter + 1 )) 
			done
		done
		echo rm ${STOKES}/$$"_split_"* >> $log
		rm ${STOKES}/$$"_split_"*
    fi	
         
	#Convert the subbands with bf2presto or 2bf2fits
	if [ $all_pproc == 0 ] && [ $rfi_pproc == 0 ]
	then
	    if [[ $subsformat == 0 ]] && [[ $STOKES == "stokes" ]] && [[ $H5_exist == 0 ]]
	    then
	       converter_exe="2bf2fits"
	       extra_flags="-CS -parset $PARSET -nbits 8 -A 600 -v"
	    elif [[ $subsformat == 0 ]] && [[ $STOKES == "stokes" ]] && [[ $H5_exist == 1 ]]
	    then
	       converter_exe="2bf2fits"
	       extra_flags="-CS -H -parset $PARSET -append -nbits 8 -A 600 -v"
	    elif [[ $subsformat == 0 ]] && [[ $STOKES == "incoherentstokes" ]]
	    then
	       converter_exe="2bf2fits"
	       extra_flags="-append -parset $PARSET -nbits 8 -A 600 -v"
	    elif [[ $subsformat == 1 ]] && [[ $STOKES == "stokes" ]] && [[ $H5_exist == 0 ]]
	    then
	       converter_exe="bf2presto8"
	       extra_flags="-f 0 -c ${CHAN} -n ${DOWN} -N ${SAMPLES} ${COLLAPSE} -A 10"
	    elif [[ $subsformat == 1 ]] && [[ $STOKES == "stokes" ]] && [[ $H5_exist == 1 ]]
	    then
	       converter_exe="bf2presto8"
	       extra_flags="-H -f 0 -c ${CHAN} -n ${DOWN} -N ${SAMPLES} ${COLLAPSE} -A 10"
	    else # [[ $subsformat == 1 ]] && [[ $STOKES == "incoherentstokes" ]] 
	       converter_exe="bf2presto8"
	       extra_flags="-f 0 -c ${CHAN} -n ${DOWN} -N ${SAMPLES} ${COLLAPSE} -A 10"
	    fi
		echo "Starting $converter_exe conversion with flags=$extra_flags for RSP-splits"
		echo "Starting $converter_exe conversion with flags=$extra_flags for RSP-splits" >> $log
        
		date
		date >> $log
		
		if (( $flyseye == 0 ))
		then
			for ii in $num_dir
			do
			  echo 'Converting subbands: '`cat ${STOKES}"/RSP"${ii}"/RSP"${ii}".list"` >> ${STOKES}"/RSP"${ii}"/"${converter_exe}"_RSP"${ii}".out" 2>&1 
              if [[ $nrBeams == 1 ]]
              then 
                 pulsar_name=${PULSAR_ARRAY_PRIMARY[0]}
              else
                 pulsar_name=${PULSAR_ARRAY_PRIMARY[$ii]}
              fi
              
			  if [[ $transpose == 0 ]] 
			  then
			     echo ${converter_exe} ${extra_flags} -o ${STOKES}"/RSP"${ii}"/"${pulsar_name}"_"${OBSID}"_RSP"${ii} `cat ${STOKES}"/RSP"${ii}"/RSP"${ii}".list"` >> $log  
			     ${converter_exe} ${extra_flags} -A 10 -o ${STOKES}"/RSP"${ii}"/"${pulsar_name}"_"${OBSID}"_RSP"${ii} `cat ${STOKES}"/RSP"${ii}"/RSP"${ii}".list"` >> ${STOKES}"/RSP"${ii}"/"${converter_exe}"_RSP"${ii}".out" 2>&1 &
#A2test
#                 cp /net/sub6/lse016/data4/2nd_transpose/L2010_21144_red_BusyWeek/incoherentstokes/RSP$ii/*sub0000 ${STOKES}/RSP$ii/
#                 ln -s /net/sub6/lse016/data4/2nd_transpose/L2010_21144_red_BusyWeek/incoherentstokes/RSP$ii/*sub[0-9]* ${STOKES}/RSP$ii/
			     bf2presto_pid[$ii]=$!  
			  elif [[ $transpose == 1 ]] && [[ $STOKES == "incoherentstokes" ]]
              then
                 if (( $subsformat == 1 ))
                 then
                    extra_flags="$extra_flags -t"
                 fi
			     echo ${converter_exe} ${extra_flags} -A 10 -o ${STOKES}"/RSP"${ii}"/"${pulsar_name}"_"${OBSID}"_RSP"${ii} `cat ${STOKES}"/RSP"${ii}"/RSP"${ii}".list"` >> $log  
			     ${converter_exe} ${extra_flags} -A 10 -o ${STOKES}"/RSP"${ii}"/"${pulsar_name}"_"${OBSID}"_RSP"${ii} `cat ${STOKES}"/RSP"${ii}"/RSP"${ii}".list"` >> ${STOKES}"/RSP"${ii}"/"${converter_exe}"_RSP"${ii}".out" 2>&1 &
#A2test
#                 cp /net/sub6/lse016/data4/2nd_transpose/L2010_21144_red_test/incoherentstokes/RSP$ii/*sub[0-9]* ${location}/${STOKES}/RSP$ii/
			     bf2presto_pid[$ii]=$!  
              else # [[ $transpose == 1 ]] && [[ $STOKES == "stokes" ]]
                 if (( $TiedArray == 0 ))
                 then
                    cd ${location}/${STOKES}/RSP$ii/

	                if (( $subsformat == 1 ))
	                then
	                   extra_flags="$extra_flags -M -T ${nSubbands}"
	                fi

			        echo ${converter_exe} ${extra_flags} -A 10 -o ${pulsar_name}"_"${OBSID}"_RSP"${ii} `cat "RSP"${ii}".list"` >> $log  
			        ${converter_exe} ${extra_flags} -A 10 -o ${pulsar_name}"_"${OBSID}"_RSP"${ii} `cat "RSP"${ii}".list"` >> ${converter_exe}"_RSP"${ii}".out" 2>&1 &
#A2test
#                    cp /net/sub6/lse016/data4/2nd_transpose/L2010_21144_red_test/incoherentstokes/RSP$ii/*sub[0-9]* ${location}/${STOKES}/RSP$ii/
			        bf2presto_pid[$ii]=$!  
			     else # (( $TiedArray == 1 ))
			        counter=0
			        for jjj in $beams
			        do
			            ## -- not sure about this line:    ${STOKES}/"RSP"${ii}/${jjj}
                        cd ${location}/${STOKES}/RSP$ii/${jjj}

		                if (( $subsformat == 1 ))
		                then
		                   extra_flags="$extra_flags -M -T ${nSubbands}"
		                fi

			            echo ${converter_exe} ${extra_flags} -A 10 -o ${pulsar_name}"_"${OBSID}"_RSP"${ii} `cat "RSP"${ii}".list"` >> $log  
			            ${converter_exe} ${extra_flags} -A 10 -o ${pulsar_name}"_"${OBSID}"_RSP"${ii} `cat "RSP"${ii}".list"` >> ${converter_exe}"_RSP"${ii}".out" 2>&1 &
			           bf2presto_pid[$ii][$counter]=$!  
				       counter=$(( $counter + 1 )) 
			        done
			     
			     fi
			  fi
			done
		else # if (( $flyseye != 0 ))
			for ii in $num_dir
			do	
			    # note, should be in STOKES/RSP? directory because output gets "beam_N" PREFIX
			    cd ${location}/${STOKES}/"RSP"${ii}

                if [[ $nrBeams == 1 ]]
                then 
                   pulsar_name=${PULSAR_ARRAY_PRIMARY[0]}
                else
                   pulsar_name=${PULSAR_ARRAY_PRIMARY[$ii]}
                fi
	
#		 	    echo 'Converting subbands: '`cat RSP"${ii}".list"` >> ${converter_exe}"_RSP"${ii}".out" 2>&1 
		 	    if [[ $transpose == 0 ]] 
			    then

	               if (( $subsformat == 1 ))
	               then
	                  extra_flags="$extra_flags -b ${NBEAMS}"
	               fi

			       echo ${converter_exe} ${extra_flags} -A 10 -o ${pulsar_name}"_"${OBSID}"_RSP"${ii} `cat "RSP"${ii}".list"` >> $log  
			       ${converter_exe} ${extra_flags} -A 10 -o ${pulsar_name}"_"${OBSID}"_RSP"${ii} `cat "RSP"${ii}".list"` >> ${converter_exe}"_RSP"${ii}".out" 2>&1 &
			       bf2presto_pid[$ii]=$!  
			    elif [[ $transpose == 1 ]] && [[ $STOKES == "incoherentstokes" ]]
                then

	               if (( $subsformat == 1 ))
	               then
	                  extra_flags="$extra_flags -t -b ${NBEAMS}"
	               fi

			       echo ${converter_exe} ${extra_flags} -A 10 -o ${pulsar_name}"_"${OBSID}"_RSP"${ii} `cat "RSP"${ii}".list"` >> $log  
			       ${converter_exe} ${extra_flags} -A 10 -o ${pulsar_name}"_"${OBSID}"_RSP"${ii} `cat "RSP"${ii}".list"` >> ${converter_exe}"_RSP"${ii}".out" 2>&1 &
			       bf2presto_pid[$ii]=$!
                else #    # [[ $transpose == 1 ]] && [[ $STOKES == "stokes" ]]
                   if (( $TiedArray == 0 ))
                   then

		               if (( $subsformat == 1 ))
		               then
		                  extra_flags="$extra_flags -T ${nSubbands} -M -b 1"
		               fi

				       echo ${converter_exe} ${extra_flags} -A 10 -o ${pulsar_name}"_"${OBSID}"_RSP"${ii} `cat "RSP"${ii}".list"` >> $log  
				       ${converter_exe} ${extra_flags} -A 10 -o ${pulsar_name}"_"${OBSID}"_RSP"${ii} `cat "RSP"${ii}".list"` >> ${converter_exe}"_RSP"${ii}".out" 2>&1 &
			       
			           bf2presto_pid[$ii]=$!
			           
				   else # (( $TiedArray == 1 ))
				        counter=0
				        for jjj in $beams
				        do
	                        cd ${location}/${STOKES}/RSP$ii/${jjj}

			                if (( $subsformat == 1 ))
			                then
			                   extra_flags="$extra_flags -T ${nSubbands} -M -b 1"
			                fi

				            echo ${converter_exe} ${extra_flags} -A 10 -o ${pulsar_name}"_"${OBSID}"_RSP"${ii} `cat "RSP"${ii}".list"` >> $log  
				            ${converter_exe} ${extra_flags} -A 10 -o ${pulsar_name}"_"${OBSID}"_RSP"${ii} `cat "RSP"${ii}".list"` >> ${converter_exe}"_RSP"${ii}".out" 2>&1 &
				            bf2presto_pid[$ii][$counter]=$!
				            counter=$(( $counter + 1 )) 
				        done
				     
				   fi #end  (( $TiedArray == 1 ))
			    fi  #end [[ $transpose == 1 ]] && [[ $STOKES == "stokes" ]]
			done
			cd ${location}
	    fi
	
		echo "Running $converter_exe in the background for RSP-splits..." 

#	if (( $flyseye == 0 ))
#	then

		for ii in $num_dir
		do
		   echo "Waiting for RSP$ii $converter_exe to finish"
		   if (( $TiedArray == 0 ))
		   then
		       wait ${bf2presto_pid[ii]}
		   else
		       counter=0
		       for jjj in $beams
		       do
		           wait ${bf2presto_pid[ii][counter]}
				   counter=$(( $counter + 1 )) 
		       done
		   fi
		done

#    else
#		echo "Waiting for bf2presto to finish"
#		wait $bf2presto_pid
#    fi
	
		echo "Done $converter_exe (splits)" 
		echo "Done $converter_exe (splits)" >> $log
		date
		date >> $log
		
	    if [[ $transpose == 1 ]] && [[ $STOKES == "stokes" ]] && [[ $nrBeams == 1 ]] && [[ $subsformat == 1 ]]
	    then
	       if [[ $flyseye == 0 ]] 
	       then
		       # move the beam_0 data out one directory
		       cd ${location}/${STOKES}/RSP0/beam_0
		       mv * ../
		       cd ../
		       rmdir beam_0
	       elif [[ $TiedArray == 1 ]]
	       then
		       # move the beam_0 data out one directory
		       cd ${location}/${STOKES}/RSP0/

		       counter=0
		       for jjj in $beams
		       do
		           cd $jjj/beam_0
			       mv * ../
			       cd ../
			       rmdir beam_0
			       cd ../
				   counter=$(( $counter + 1 )) 
		       done

	       fi 

	    fi

       # clean up dummy files
       if [[ $create_dummies == 1 ]] && [[ $STOKES == "incoherentstokes" ]]
       then
          if [[ -d /data/$OBSID ]]
          then
             rm -rf /data/$OBSID
          fi
       fi

	fi # end if [ $all_pproc == 0 ] && [ $rfi_pproc == 0 ]

	# Calculating the number of samples to be passed to inf-file
	split_files=`echo $all_num $CHAN | awk '{print $1 * $2}'`
	if (( $flyseye == 0 )) && (( $rfi_pproc == 0 )) && (( $subsformat == 1 ))
	then
	    # check the file sizes to be sure all sub files are the same
	    file_size_check=`ls -l ${location}/${STOKES}/RSP0/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP0.sub???? | grep -v .inf | awk '{print $5}' | sort | uniq -c | head -1 | awk '{print $1}'`
	    num_files_check=`ls -l ${location}/${STOKES}/RSP0/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP0.sub???? | grep -v .inf | wc -l | awk '{print $1}`
	    if (( $file_size_check != $num_files_check ))
	    then
	       echo "WARNING: RSP0/SubXXXX files are not all the same size;  raw data and processing is suspect to problems."
	       echo "WARNING: RSP0/SubXXXX files are not all the same size;  raw data and processing is suspect to problems." >> $log
	    fi
		NSAMPL=`ls -l ${location}/${STOKES}/RSP0/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP0.sub???? | grep -v .inf | awk '{print $5}' | sort -n | uniq -c | sort -n -r | head -1 | awk '{print $2 / 2 }' -`
	elif (( $flyseye == 1 )) && (( $rfi_pproc == 0 ))
	then
	    file_size_check=`ls -l ${location}/${STOKES}/RSP0/beam_0/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP0.sub???? | grep -v .inf | awk '{print $5}' | sort | uniq -c | head -1 | awk '{print $1}'`
	    num_files_check=`ls -l ${location}/${STOKES}/RSP0/beam_0/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP0.sub???? | grep -v .inf | wc -l | awk '{print $1}`
	    if (( $file_size_check != $num_files_check ))
	    then
	       echo "WARNING: RSP0/SubXXXX files are not all the same size;  raw data and processing is suspect to problems."
	       echo "WARNING: RSP0/SubXXXX files are not all the same size;  raw data and processing is suspect to problems." >> $log
	    fi
		NSAMPL=`ls -l ${location}/${STOKES}/RSP0/beam_0/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP0.sub???? | grep -v .inf | awk '{print $5}' | sort -n | uniq -c | sort -n -r | head -1 | awk '{print $2 / 2 }' -`
	fi

##############################################################################################
# Excise the channels (subband data -- create blank dummy files;  psrfits -- user rfifind to create mask)
##############################################################################################
     # clean up data and excise every 0th channel from each subband (make blank .subXXXX files)
     # A2 to-do: check for the number of channels and is 1, don't excise data
	if [ $all_pproc == 0 ] && [ $rfi_pproc == 0 ] && [ $CHAN != 1 ] 
	then
	    if (( $subsformat == 1 ))
	    then
		   echo "Excising blank channels from .subXXX files"
		   echo "Excising blank channels from .subXXX files" >> $log
        else
		   echo "Creating rfi mask for to blank channels from psrfits files"
		   echo "Creating rfi mask for to blank channels from psrfits files" >> $log
        fi # end if (( $subsformat == 1 ))

		if (( $flyseye == 0 ))
		then
			for ii in $num_dir
			do
			    cd ${location}/${STOKES}/RSP${ii}
			    if (( $subsformat == 1 ))
			    then
				    /bin/ls *sub[0-9]??? | grep -v sub0000 > $$.list
				    while read file_name
				    do
				       subband_number=`echo $file_name | sed 's/.*\.sub//g' | awk '{printf("%d\n"),$1}'`
				       modulo_files=`echo $subband_number $CHAN | awk '{print ($1 % $2)}'`
				       if (( $modulo_files == 0 ))
				       then
				             echo "   Excising RSP$ii sub`echo $file_name | sed 's/.*\.sub//g'`"
				             echo "   Excising RSP$ii sub`echo $file_name | sed 's/.*\.sub//g'`" >> $log
				             mv $file_name $file_name"_ORIG"
				             touch $file_name
				       fi
				    done < $$.list
				    rm $$.list
				else 
				    if [ $rfi == 1 ] || [ $rfi_pproc == 1 ]
				    then
					    # prepare for psrfits channel excision via the rfifind mask file
					    max=`echo "$nSubbands * $CHAN" | bc` 
					    echo rfifind -o ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii} -psrfits -noclip -blocks 16 -zapchan 0:$max:$CHAN ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.fits
					    echo rfifind -o ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii} -psrfits -noclip -blocks 16 -zapchan 0:$max:$CHAN ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.fits >> $log
					    rfifind -o ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii} -psrfits -noclip -blocks 16 -zapchan 0:$max:$CHAN ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.fits >> ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.rfiout 2>&1 &
	  			        rfifind_pid[$ii]=$!  
                    fi
				fi # end if (( $subsformat == 1 ))
			done
		else
		    if (( $TiedArray == 0 ))
		    then
		       loop_beams=$beams_init
		    else
		       loop_beams=$beams
		    fi
		    counter=0
		    for jjj in $loop_beams
		    do
		        for ii in $num_dir
		        do
				    cd ${location}/${STOKES}/RSP${ii}/${jjj}
			        if (( $subsformat == 1 ))
			        then
				        /bin/ls *sub[0-9]??? | grep -v sub0000 > $$.list
					    while read file_name
					    do
					       subband_number=`echo $file_name | sed 's/.*\.sub//g' | awk '{printf("%d\n"),$1}'`
					       modulo_files=`echo $subband_number $CHAN | awk '{print ($1 % $2)}'`
					       if (( $modulo_files == 0 ))
					       then
					          echo "   Excising $jjj RSP$ii sub`echo $file_name | sed 's/.*\.sub//g'`"
					          echo "   Excising $jjj RSP$ii sub`echo $file_name | sed 's/.*\.sub//g'`" >> $log
					          mv $file_name $file_name"_ORIG"
					          touch $file_name
					       fi
					    done < $$.list
					    rm $$.list
					else
					    if [ $rfi == 1 ] || [ $rfi_pproc == 1 ]
					    then
						    # prepare for psrfits channel excision via the rfifind mask file
						    max=`echo "$nSubbands * $CHAN" | bc` 
						    echo rfifind -o ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii} -psrfits -noclip -blocks 16 -zapchan 0:$max:$CHAN ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.fits
						    echo rfifind -o ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii} -psrfits -noclip -blocks 16 -zapchan 0:$max:$CHAN ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.fits >> $log
						    rfifind -o ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii} -psrfits -noclip -blocks 16 -zapchan 0:$max:$CHAN ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.fits >> ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.rfiout 2>&1 &
	  			            rfifind_pid[$ii][$counter]=$!  
                        fi
					fi # end if (( $subsformat == 1 ))
		        done
		        counter=$(( $counter + 1 ))
	        done
		
		fi # end if (( $flyseye == 0 ))
       
	fi # end if [ $all_pproc == 0 ] && [ $rfi_pproc == 0 ] && [ $CHAN != 1 ]
	cd ${location}

	#RFI-Report
	if [ $rfi == 1 ] || [ $rfi_pproc == 1 ]
	then 
		   echo "Producing rfi report"
		   echo "Producing rfi report" >> $log
		   date
		   date >> $log
		   for ii in $num_dir
		   do
		      if (( $flyseye == 0 ))
		      then
		         echo cd ${location}/${STOKES}/RSP${ii} >> $log
		         cd ${location}/${STOKES}/RSP${ii}
		         if (( $subsformat == 1 ))
		         then
		            echo python ${LOFARSOFT}/release/share/pulsar/bin/subdyn.py --saveonly -n `echo ${SAMPLES}*10 | bc` *.sub[0-9]???  >> $log
		         else
		            echo python ${LOFARSOFT}/release/share/pulsar/bin/subdyn.py --psrfits --saveonly -n `echo ${SAMPLES}*10 | bc` *.fits  >> $log
		         fi
			     if [ $test == 0 ]
			     then
				    sleep 5
		            if (( $subsformat == 1 ))
		            then
		               python ${LOFARSOFT}/release/share/pulsar/bin/subdyn.py --saveonly -n `echo ${SAMPLES}*10 | bc` *.sub[0-9]??? &
		               subdyn_pid[$ii]=$!
		            else
		               python ${LOFARSOFT}/release/share/pulsar/bin/subdyn.py --psrfits --saveonly -n `echo ${SAMPLES}*10 | bc` *.fits &
		               subdyn_pid[$ii]=$!
		            fi
		         fi
		      else
	
	             if [ $rfi_pproc == 1 ]
	             then
	                loop_beams=$beams
	             else
	                loop_beams=$beams_init
	             fi
	             
	             counter=0
				 for jjj in $loop_beams
				 do
				     if [ $rfi_pproc == 0 ]
				     then
	   		            echo cd ${location}/${STOKES}/RSP${ii}/${jjj} >> $log
			            cd ${location}/${STOKES}/RSP${ii}/${jjj}
			         else
			            echo cd ${location}/${STOKES}/${jjj}/beam_${ii}/ >> $log
			            cd ${location}/${STOKES}/${jjj}/beam_${ii}/
	                 fi
	                 
		             if (( $subsformat == 1 ))
		             then
			            echo python ${LOFARSOFT}/release/share/pulsar/bin/subdyn.py --saveonly -n `echo ${SAMPLES}*10 | bc` *.sub[0-9]???  >> $log
			         else
			            echo python ${LOFARSOFT}/release/share/pulsar/bin/subdyn.py --psrfits --saveonly -n `echo ${SAMPLES}*10 | bc` *.fits  >> $log
			         fi
				     if [ $test == 0 ]
				     then
				        sleep 5
		                if (( $subsformat == 1 ))
		                then
			               python ${LOFARSOFT}/release/share/pulsar/bin/subdyn.py --saveonly -n `echo ${SAMPLES}*10 | bc` *.sub[0-9]??? &
			               subdyn_pid[$ii][$counter]=$!	
			            else
			               python ${LOFARSOFT}/release/share/pulsar/bin/subdyn.py --psrfits --saveonly -n `echo ${SAMPLES}*10 | bc` *.fits &
			               subdyn_pid[$ii][$counter]=$!	
			            fi
			         fi
				     counter=$(( $counter + 1 ))
		         done      
	          fi
		   done
		
		   echo cd ${location} >> $log
		   cd ${location}

	fi # end if [ $rfi == 1 ] || [ $rfi_pproc == 1 ] 
		
	# wait for rfifind to finish
	if [ $rfi == 1 ] || [ $rfi_pproc == 1 ]
	then
	    if (( $subsformat == 0 ))
	    then
			for ii in $num_dir
			do
			   echo "Waiting for RSP$ii rfifind_pid to finish"
			   echo "Waiting for RSP$ii rfifind_pid to finish" >> $log
			   if (( $flyseye == 0 ))
			   then
			       wait ${rfifind_pid[ii]}
			   else
		  		    if (( $TiedArray == 0 ))
				    then
				       loop_beams=$beams_init
				    else
				       loop_beams=$beams
				    fi
		
			        counter=0
			        for jjj in $loop_beams
			        do
			           echo "Waiting for RSP$ii beam_$counter rfifind_pid to finish"
			           echo "Waiting for RSP$ii beam_$counter rfifind_pid to finish" >> $log
			           wait ${rfifind_pid[ii][counter]}
					   counter=$(( $counter + 1 )) 
			        done
			   fi
			done					
	    fi # end if (( $subsformat == 0 ))
	fi # end if [ $rfi == 1 ] || [ $rfi_pproc == 1 ]
	
	if [ $all_pproc == 0 ] && [ $rfi_pproc == 0 ]
	then
	   if (( $subsformat == 1 ))
	   then
			#Create .sub.inf files with par2inf.py
		    if (( $flyseye == 0 )) || (( $transpose == 0 ))
		    then
		        beam_counter=0
				while (( $beam_counter < $nrBeams ))
				do
					echo cp ${LOFARSOFT}/release/share/pulsar/data/lofar_default.inf default.inf >> $log
					cp ${LOFARSOFT}/release/share/pulsar/data/lofar_default.inf default.inf
					#python ${LOFARSOFT}/release/share/pulsar/bin/par2inf.py -S ${PULSAR} -o test -N ${NSAMPL} -n `echo $all_num 248 | awk '{print $1 / $2}'` -r $core ./${OBSID}.parset
					echo "Running par2inf 1" 
					echo "Running par2inf 1" >> $log
					
					if (( $nrBeams == 1 ))
					then
	                   if [[ $STOKES != "stokes" ]]
	                   then
					        echo python ${LOFARSOFT}/release/share/pulsar/bin/par2inf.py -S ${PULSAR_ARRAY_PRIMARY[0]} -o test -N ${NSAMPL} -n `echo $all_num $core | awk '{print $1 / $2}'` -r $core ./${OBSID}.parset >> $log
					        python ${LOFARSOFT}/release/share/pulsar/bin/par2inf.py -S ${PULSAR_ARRAY_PRIMARY[0]} -o test -N ${NSAMPL} -n `echo $all_num $core | awk '{print $1 / $2}'` -r $core ./${OBSID}.parset
					        status=$?	
		                else 
		                    if [[ $nrTArings == 0 ]]
		                    then
							    echo python ${LOFARSOFT}/release/share/pulsar/bin/par2inf.py -S ${PULSAR_ARRAY_PRIMARY[0]} -o test -N ${NSAMPL} -n $nSubbands -r 1 ${location}/${OBSID}.parset >> $log
							    python ${LOFARSOFT}/release/share/pulsar/bin/par2inf.py -S ${PULSAR_ARRAY_PRIMARY[0]} -o test -N ${NSAMPL} -n $nSubbands -r 1 ${location}/${OBSID}.parset
						        status=$?	
						     else
						        # get the beam_num from the file name of the TA beam (assume single file TA beams per machine)
						        beam_num=`head -1 ${location}/${STOKES}/SB_master.list | sed 's/^.*\///g' | sed 's/.*_B//g' | sed 's/_.*raw//g' | sed 's/^0//g' | sed 's/^0//g'`
							    echo python ${LOFARSOFT}/release/share/pulsar/bin/par2inf.py -S ${PULSAR_ARRAY_PRIMARY[0]} -o test -N ${NSAMPL} -n $nSubbands -r 1 -B ${beam_num} ${location}/${OBSID}.parset >> $log
							    python ${LOFARSOFT}/release/share/pulsar/bin/par2inf.py -S ${PULSAR_ARRAY_PRIMARY[0]} -o test -N ${NSAMPL} -n $nSubbands -r 1 -B ${beam_num} ${location}/${OBSID}.parset
						        status=$?	
						     fi
	                    fi
				        
						if [ $status -ne 0 ]
						then
						   echo "ERROR: Unable to successfully run par2inf task"
						   echo "ERROR: Unable to successfully run par2inf task" >> $log
						   exit 1
						fi
						
						# move results to the correct location					
	            		jj=0    
						for ii in `ls -1 test*.inf | awk -F\. '{print $0,substr($1,5,10)}' | sort -k 2 -n | awk '{print $1}'`
						do
	                       if (( $TiedArray == 0 ))
	                       then
	                           INFNAME=${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${jj}
	                           echo "sed -e 's/Data file name without suffix.*/Data file name without suffix          =  $INFNAME/' ${ii} > ${STOKES}/RSP${jj}/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${jj}.sub.inf" >> $log
	                           sed -e "s/Data file name without suffix.*/Data file name without suffix          =  $INFNAME/" ${ii} > ${STOKES}/RSP${jj}/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${jj}.sub.inf 
	                           rm ${ii}
	#						   echo mv ${ii} ${STOKES}/RSP${jj}/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${jj}.sub.inf >> $log
	#						   mv ${ii} ${STOKES}/RSP${jj}/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${jj}.sub.inf
							   jj=`expr $jj + 1`
						   else
						       for jjj in $beams
						       do 
		                           INFNAME=${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${jj}
		                           echo "sed -e 's/Data file name without suffix.*/Data file name without suffix          =  $INFNAME/' ${ii} > ${STOKES}/RSP${jj}/${jjj}/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${jj}.sub.inf" >> $log
		                           sed -e "s/Data file name without suffix.*/Data file name without suffix          =  $INFNAME/" ${ii} > ${STOKES}/RSP${jj}/${jjj}/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${jj}.sub.inf 
	                               rm ${ii}
	#							   echo cp ${ii} ${STOKES}/RSP${jj}/${jjj}/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${jj}.sub.inf >> $log
	#							   cp ${ii} ${STOKES}/RSP${jj}/${jjj}/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${jj}.sub.inf
								   jj=`expr $jj + 1`
							   done 
						   fi
						done
					else # (( $nrBeams > 1 ))
				        echo python ${LOFARSOFT}/release/share/pulsar/bin/par2inf.py -S ${PULSAR_ARRAY_PRIMARY[$beam_counter]} -o test -N ${NSAMPL} -n `echo $all_num $core | awk '{print $1 / $2}'` -r 1 -B $beam_counter ./${OBSID}.parset >> $log
				        python ${LOFARSOFT}/release/share/pulsar/bin/par2inf.py -S ${PULSAR_ARRAY_PRIMARY[$beam_counter]} -o test -N ${NSAMPL} -n `echo $all_num $core | awk '{print $1 / $2}'` -r 1 -B $beam_counter ./${OBSID}.parset
				        status=$?	
				        
						if [ $status -ne 0 ]
						then
						   echo "ERROR: Unable to successfully run par2inf task"
						   echo "ERROR: Unable to successfully run par2inf task" >> $log
						   exit 1
						fi
						
						# move result (single file) to the correct location
						result=`ls -1 test*.inf`
	
	                    INFNAME=${PULSAR_ARRAY_PRIMARY[$beam_counter]}_${OBSID}_RSP${beam_counter}
	                    echo "sed -e 's/Data file name without suffix.*/Data file name without suffix          =  $INFNAME/' ${result} > ${STOKES}/RSP${beam_counter}/${PULSAR_ARRAY_PRIMARY[$beam_counter]}_${OBSID}_RSP${beam_counter}.sub.inf" >> $log
	                    sed -e "s/Data file name without suffix.*/Data file name without suffix          =  $INFNAME/" ${result} > ${STOKES}/RSP${beam_counter}/${PULSAR_ARRAY_PRIMARY[$beam_counter]}_${OBSID}_RSP${beam_counter}.sub.inf 
	                    rm ${result}
	#					echo mv ${result} ${STOKES}/RSP${beam_counter}/${PULSAR_ARRAY_PRIMARY[$beam_counter]}_${OBSID}_RSP${beam_counter}.sub.inf >> $log
	#					mv ${result} ${STOKES}/RSP${beam_counter}/${PULSAR_ARRAY_PRIMARY[$beam_counter]}_${OBSID}_RSP${beam_counter}.sub.inf
				     fi # end if (( $nrBeams == 1 ))
	
					beam_counter=$(( $beam_counter + 1 ))
				done # end while (( $beam_counter < $nrBeams ))
	        elif (( $flyseye == 1 )) && (( $transpose == 1 ))
	        then
				for ii in $num_dir
				do	        
					echo cp ${LOFARSOFT}/release/share/pulsar/data/lofar_default.inf ${location}/${STOKES}/default.inf >> $log
					cp ${LOFARSOFT}/release/share/pulsar/data/lofar_default.inf ${location}/${STOKES}/default.inf
					#python ${LOFARSOFT}/release/share/pulsar/bin/par2inf.py -S ${PULSAR} -o test -N ${NSAMPL} -n `echo $all_num 248 | awk '{print $1 / $2}'` -r $core ./${OBSID}.parset
					echo "Running par2inf for dir $ii" 
					echo "Running par2inf for dir $ii" >> $log
					
				    echo python ${LOFARSOFT}/release/share/pulsar/bin/par2inf.py -S ${PULSAR_ARRAY_PRIMARY[0]} -o test -N ${NSAMPL} -n $nSubbands -r 1 ${location}/${OBSID}.parset >> $log
				    python ${LOFARSOFT}/release/share/pulsar/bin/par2inf.py -S ${PULSAR_ARRAY_PRIMARY[0]} -o test -N ${NSAMPL} -n $nSubbands -r 1 ${location}/${OBSID}.parset
				    status=$?	
				
					if [ $status -ne 0 ]
					then
					   echo "ERROR: Unable to successfully run par2inf task"
					   echo "ERROR: Unable to successfully run par2inf task" >> $log
					   exit 1
					fi
	
	                if (( $TiedArray == 0 ))
	                then
		                result=`ls -1 test*.inf`
	                    INFNAME=${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}
	                    echo "sed -e 's/Data file name without suffix.*/Data file name without suffix          =  $INFNAME/' $result > ${STOKES}/RSP${ii}/beam_0/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.sub.inf" >> $log
	                    sed -e "s/Data file name without suffix.*/Data file name without suffix          =  $INFNAME/" $result > ${STOKES}/RSP${ii}/beam_0/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.sub.inf 
	                    rm ${result}
	#	                echo mv $result ${STOKES}/RSP${ii}/beam_0/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.sub.inf >> $log
	#	                mv $result ${STOKES}/RSP${ii}/beam_0/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.sub.inf
			         else
			             for jjj in $beams
			             do
			                result=`ls -1 test*.inf`
		                    INFNAME=${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}
		                    echo "sed -e 's/Data file name without suffix.*/Data file name without suffix          =  $INFNAME/' $result > ${STOKES}/RSP${ii}/${jjj}/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.sub.inf" >> $log
		                    sed -e "s/Data file name without suffix.*/Data file name without suffix          =  $INFNAME/" $result > ${STOKES}/RSP${ii}/${jjj}/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.sub.inf 
	#                        rm ${result}
	
	#		                echo cp $result ${STOKES}/RSP${ii}/${jjj}/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.sub.inf >> $log
	#		                cp $result ${STOKES}/RSP${ii}/${jjj}/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.sub.inf
			             done		         
		             fi
	            done
	        fi
			
			# A2 check to see if this is still valid/needed mode of operation
			jj=0    
	        if (( $flyseye == 1 )) && (( $transpose == 0 ))
			then
			    for jjj in $beams
			    do
			        jj=0    
				    for ii in `ls -1 test*.inf | awk -F\. '{print $0,substr($1,5,10)}' | sort -k 2 -n | awk '{print $1}'`
				    do
				       #cp ${ii} ${STOKES}/${jjj}/RSP${jj}/${PULSAR}_${OBSID}_RSP${jj}.sub.inf
	                   INFNAME=${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${jj}
	                   echo "sed -e 's/Data file name without suffix.*/Data file name without suffix          =  $INFNAME/' $ii > ${STOKES}/RSP${jj}/${jjj}/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${jj}.sub.inf" >> $log
	                   sed -e "s/Data file name without suffix.*/Data file name without suffix          =  $INFNAME/" $ii > ${STOKES}/RSP${jj}/${jjj}/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${jj}.sub.inf 
	                   #rm ${result}
	                   
	#			       echo cp ${ii} ${STOKES}/RSP${jj}/${jjj}/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${jj}.sub.inf >> $log
	#			       cp ${ii} ${STOKES}/RSP${jj}/${jjj}/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${jj}.sub.inf
				       jj=`expr $jj + 1`
				    done
			    done
			fi 
			
		    if (( $flyseye == 1 )) && (( $transpose == 0 ))
		    then
		       echo rm test*.inf >> $log
		       rm test*.inf
		    fi
		 else # if (( $subsformat == 1 ))
	        if [ $rfi == 1 ] || [ $rfi_pproc == 1 ]
	        then
		        # copy the rfifind .inf file as the .inf file instead of creating it
		        echo "Note, skipping creating .inf file for psrfits mode" 
		        echo "Note, skipping creating .inf file for psrfits mode"  >> $log	        
	        else
		        echo "Note, skipping creating .inf file for psrfits mode" 
		        echo "Note, skipping creating .inf file for psrfits mode"  >> $log	        
	        fi
		 fi
	fi # end if [ $all_pproc == 0 ] && [ $rfi_pproc == 0 ]

    # Create the RSPA (all) directory when the all option is requested
	if [ $all == 1 ] || [ $all_pproc == 1 ]
	then 
	  if (( $flyseye == 0 ))
	  then
	      if [ -d ${STOKES}/RSPA ]
	      then 
	         echo "${STOKES}/RSPA already exist;  deleting contents in order to process all subbands correctly"
	         echo "${STOKES}/RSPA already exist;  deleting contents in order to process all subbands correctly" >> $log
	         rm -rf ${STOKES}/RSPA/*
	      else
	         if (( $nrBeams > 1 ))
	         then
		         echo "WARNING: For multi-beam observations -all option should NOT be used;  ignoring RSPA processing request."
		         echo "WARNING: For multi-beam observations -all option should NOT be used;  ignoring RSPA processing request." >> $log
		         all=0
		         all_pproc=0
		     else
		         mkdir -p ${STOKES}/"RSPA"
	         fi
		  fi
	  fi 
    fi # end if [ $all == 1 ] || [ $all_pproc == 1 ]
		
	#Create the .sub.inf file for the entire set (in background, as there is plenty of time to finish before file is needed
	if (( (( $all == 1 )) || (( $all_pproc == 1 )) )) && (( $flyseye == 0 ))
	then 
	     echo python ${LOFARSOFT}/release/share/pulsar/bin/par2inf.py -S ${PULSAR_ARRAY_PRIMARY[0]} -o test -N ${NSAMPL} -n $all_num -r 1 ./${OBSID}.parset >> $log
	     python ${LOFARSOFT}/release/share/pulsar/bin/par2inf.py -S ${PULSAR_ARRAY_PRIMARY[0]} -o test -N ${NSAMPL} -n $all_num -r 1 ./${OBSID}.parset

         INFNAME=${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSPA
         echo "sed -e 's/Data file name without suffix.*/Data file name without suffix          =  $INFNAME/' `ls test*.inf` > ${STOKES}/RSPA/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSPA.sub.inf" >> $log
         sed -e "s/Data file name without suffix.*/Data file name without suffix          =  $INFNAME/" `ls test*.inf` > ${STOKES}/RSPA/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSPA.sub.inf 
         rm `ls test*.inf`
	fi

	if [ $all == 1 ] || [ $all_pproc == 1 ]
	then 
	     #master_counter=0
	     offset=$(( $all_num / $core * $CHAN ))
	
	     if (( $flyseye == 0 ))
	     then
	        echo cd ${location}/${STOKES}/"RSPA" >> $log
	        cd ${location}/${STOKES}/"RSPA"
  	        echo "#!/bin/sh" > run.sh
	        ls ../RSP[0-7]/*sub[0-9]??? | sed 's/\// /g' | awk '{print $3}' | sed 's/RSP/ RSP /' | sed 's/ RSP/RSP/g' | sed 's/\.sub/ /' | awk -v offset=$offset '{printf("ln -s ../RSP%d/%s%d.sub%04d %sA.sub%04d\n"),$2,$1,$2,$3,$1,$2*offset+$3}' >> run.sh
		    echo "Performing subband linking for all RPSs in one location"
		    echo "Performing subband linking for all RPSs in one location" >> $log
		    echo chmod 777 run.sh >> $log
		    chmod 777 run.sh
		    check_number=`wc -l run.sh | awk '{print $1}'`
		    total=$(( $all_num * $CHAN + 1 ))
		    if [ $check_number -ne $total ]
		    then
		        all=0
		        echo "WARNING: possible problem running on ALL subbands in $STOKES/$jjj/RSPA;  master list is too short (is $check_number but should be $total rows)"
		        echo "WARNING: possible problem running on ALL subbands in $STOKES/$jjj/RSPA;  master list is too short (is $check_number but should be $total rows)" >> $log
		    else
		        echo ./run.sh >> $log
		        if [ $all_pproc == 0 ]
		        then
		           ./run.sh &
		        else
		           ./run.sh 
                fi
		        echo "Done subband linking for all RPSs in one location for $STOKES/$jjj/RSPA"
		        echo "Done subband linking for all RPSs in one location for $STOKES/$jjj/RSPA" >> $log
		    fi
		    echo cd $location >> $log
		    cd $location
	     fi
	     	
	fi # end if [ $all == 1 ] || [ $all_pproc == 1 ]
#    fi # end if (( $flyseye == 0 ))

    if (( $subsformat == 0 ))
    then
         total_channels=`echo "$nSubbands * $CHAN" | bc`
         if (( $total_channels <= 6000 ))
         then 
            prepfold_nsubs=256 
         else
            prepfold_nsubs=512 
         fi        
         # if total_channels is not divisible by prepfold_nsubs (prepfold fails), find one which is divisible
		 ii=$prepfold_nsubs
		 while (( $ii <= $total_channels ))
		 do 
		       modulo_files=`echo $total_channels $ii | awk '{print ($1 % $2)}'`
		       if (( $modulo_files == 0 ))
		       then
		          echo "Success: $ii divides into $total_channels total channels;  use for -nsubs"
		          break
		       else
		          echo "Tried $ii nsubs, but still not divisble into $total_channels" >> $log
		          echo "Tried $ii nsubs, but still not divisble into $total_channels"
		       fi
		       ii=`expr $ii + 1`
		 done
         echo "Successfully found $ii to use as nsubs for prepfold" >> $log
         echo "Successfully found $ii to use as nsubs for prepfold"
		 prepfold_nsubs=$ii
    fi

	if [[ $all_pproc == 0 ]] && [[ $rfi_pproc == 0 ]] #&& [[ $PULSAR_ARRAY_PRIMARY[0] != "NONE" ]]
	then
        
        # check for .par file, else create one
		for fold_pulsar in $PULSAR_LIST
		do
	        fold_pulsar_cut=`echo $fold_pulsar | sed 's/PSR//g' | sed 's/[BJ]//g'`
		    if [[ $fold_pulsar_cut != "NONE" ]] 
		    then 
		        if [[ -f $LOFARSOFT/release/share/pulsar/data/parfile/$fold_pulsar_cut.par ]]
		        then 
		           echo cp $LOFARSOFT/release/share/pulsar/data/parfile/$fold_pulsar_cut.par ${location}/
		           echo cp $LOFARSOFT/release/share/pulsar/data/parfile/$fold_pulsar_cut.par ${location}/  >> $log
		           cp $LOFARSOFT/release/share/pulsar/data/parfile/$fold_pulsar_cut.par ${location}/
		        else
		           echo "psrcat -db_file $LOFARSOFT/release/share/pulsar/data/psrcat.db -e $fold_pulsar_cut  > ${location}/$fold_pulsar_cut.par"
		           echo "psrcat -db_file $LOFARSOFT/release/share/pulsar/data/psrcat.db -e $fold_pulsar_cut  > ${location}/$fold_pulsar_cut.par" >> $log
		           psrcat -db_file $LOFARSOFT/release/share/pulsar/data/psrcat.db -e $fold_pulsar_cut  > ${location}/$fold_pulsar_cut.par
		        fi
		     fi
		done
		
		# Fold data per requested Pulsar
		if [[ $nrBeams == 1 ]] 
		then 
		    if [[ $PULSAR_LIST != "NONE" ]] && [[ $nofold == 0 ]]
		    then 
				for fold_pulsar in $PULSAR_LIST
				do
					echo "Starting folding for RSP-splits for Pulsar $fold_pulsar"
					echo "Starting folding for RSP-splits for Pulsar $fold_pulsar" >> $log
					date
					date >> $log
		
		            # check for Pulsar name in catalog
		            in_catalog=`grep $fold_pulsar $LOFARSOFT/release/share/pulsar/data/PSR_catalog.txt`
		            if [[ $in_catalog == "" ]]
		            then 
					   echo "WARNING: Pulsar $fold_pulsar not found in PSR_catalog.txt;  skipping folding."
					   echo "WARNING: Pulsar $fold_pulsar not found in PSR_catalog.txt;  skipping folding." >> $log
					   nofold=1
		               continue
		            fi
		            	
				    if (( $flyseye == 0 ))
				    then
						for ii in $num_dir
					    do
						   cd ${location}/${STOKES}/RSP${ii}
						   cp ${location}/$fold_pulsar_cut.par .
						   echo cd ${location}/${STOKES}/RSP${ii} >> $log
						   
						   if (( $subsformat == 1 ))
						   then
						      echo prepfold -noxwin -psr ${fold_pulsar} -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.sub[0-9]??? >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout 
						   else
						      if [ $rfi == 1 ] || [ $rfi_pproc == 1 ]
						      then
						         echo prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -mask ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}_rfifind.mask -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.fits >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout
						         echo prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii}_nomask ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.fits >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout_nomask
						      else
						         echo prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.fits >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout
						      fi
						   fi
						   
						   if [ $test == 0 ]
						   then
							   if (( $subsformat == 1 ))
							   then
						          prepfold -noxwin -psr ${fold_pulsar} -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.sub[0-9]??? >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout 2>&1 &
  						          prepfold_pid[$ii]=$!  
 						          echo "Running: " prepfold -noxwin -psr ${fold_pulsar} -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.sub[0-9]??? >> $log
  						       else
  						          if [ $rfi == 1 ] || [ $rfi_pproc == 1 ]
  						          then
						             prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -mask ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}_rfifind.mask -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.fits >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout
						             sleep 10
						             prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii}_nomask ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.fits >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout_nomask
  						             prepfold_pid[$ii]=$!  
 						             echo "Running: " prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -mask ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}_rfifind.mask -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.fits >> $log
 						             echo "Running: " prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii}_nomask ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.fits >> $log
 						          else 
						             prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.fits >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout
  						             prepfold_pid[$ii]=$!  
 						             echo "Running: " prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.fits >> $log
 						          fi
 						       fi
						   fi
						   sleep 10
					    done
				     else
					    if (( $TiedArray == 0 ))
					    then
					       loop_beams=$beams_init
					    else
					       loop_beams=$beams
					    fi
					    counter=0
					    for jjj in $loop_beams
					    do
							for ii in $num_dir
						    do
							   cd ${location}/${STOKES}/RSP${ii}/${jjj}
							   cp ${location}/$fold_pulsar_cut.par .
							   echo cd ${location}/${STOKES}/RSP${ii}/${jjj} >> $log

							   if (( $subsformat == 1 ))
							   then
							       echo prepfold -noxwin -psr ${fold_pulsar} -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.sub[0-9]??? >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout 
							   else
							       if [ $rfi == 1 ] || [ $rfi_pproc == 1 ]
							       then
							          echo prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -mask ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}_rfifind.mask -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.fits >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout 
							          echo prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii}_nomask ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.fits >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout_nomask 
							       else
							          echo prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.fits >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout 
							       fi
							   fi
						       if [ $test == 0 ]
						       then
								   if (( $subsformat == 1 ))
								   then
								       prepfold -noxwin -psr ${fold_pulsar} -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.sub[0-9]??? >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout 2>&1 &
							           prepfold_pid[$ii][$counter]=$!  
							           echo "Running: " prepfold -noxwin -psr ${fold_pulsar} -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.sub[0-9]??? >> $log
							       else
							           if [ $rfi == 1 ] || [ $rfi_pproc == 1 ]
							           then
									       prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -mask ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}_rfifind.mask -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.fits >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout 2>&1 &
									       sleep 10
									       prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii}_nomask ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.fits >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout_nomask 2>&1 &
								           prepfold_pid[$ii][$counter]=$!  
								           echo "Running: " prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -mask ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}_rfifind.mask -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.fits >> $log
								           echo "Running: " prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii}_nomask ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.fits >> $log
							           else
									       prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.fits >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout 2>&1 &
								           prepfold_pid[$ii][$counter]=$!  
								           echo "Running: " prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}.fits >> $log
							           fi
							       fi
							   fi
							   sleep 10
						    done
						    counter=$(( $counter + 1 ))
					    done
					fi # end if (( $flyseye == 0 ))
					
					echo cd ${location} >> $log
					cd ${location}

					#Check when all DONE files are available, then all processes have exited
					for ii in $num_dir
					do
					   echo "Waiting for RSP$ii prepfold_pid to finish"
					   echo "Waiting for RSP$ii prepfold_pid to finish" >> $log
					   if (( $flyseye == 0 ))
					   then
					       wait ${prepfold_pid[ii]}
					   else
				  		    if (( $TiedArray == 0 ))
						    then
						       loop_beams=$beams_init
						    else
						       loop_beams=$beams
						    fi
			
					        counter=0
					        for jjj in $loop_beams
					        do
					           echo "Waiting for RSP$ii beam_$counter prepfold_pid to finish"
					           echo "Waiting for RSP$ii beam_$counter prepfold_pid to finish" >> $log
					           wait ${prepfold_pid[ii][counter]}
							   counter=$(( $counter + 1 )) 
					        done
					   fi
					done					
					
				done # finished loop over PULSAR_LIST
			fi # end if [[ $PULSAR_LIST != "NONE" ]] && [[ $nofold == 0 ]]
		else # nrBeams > 1
			for ii in $num_dir
		    do
                PULSAR_LIST=${PULSAR_ARRAY[$ii]}
				for fold_pulsar in $PULSAR_LIST
				do
				    if [[ $fold_pulsar == "NONE" ]] || [[ $nofold == 1 ]]
				    then
				        break
				    else 
						echo "Starting folding for RSP-split $ii for Pulsar $fold_pulsar"
						echo "Starting folding for RSP-split $ii for Pulsar $fold_pulsar" >> $log
						date
						date >> $log
	
						cd ${location}/${STOKES}/RSP${ii}
						echo cd ${location}/${STOKES}/RSP${ii} >> $log

					    if (( $subsformat == 1 ))
					    then
						    echo prepfold -noxwin -psr ${fold_pulsar} -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR_ARRAY_PRIMARY[$ii]}_${OBSID}_RSP${ii}.sub[0-9]??? >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout 
						else
						    if [ $rfi == 1 ] || [ $rfi_pproc == 1 ]
						    then
						        echo prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -mask ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}_rfifind.mask -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR_ARRAY_PRIMARY[$ii]}_${OBSID}_RSP${ii}.fits >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout 		
						        echo prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii}_nomask ${PULSAR_ARRAY_PRIMARY[$ii]}_${OBSID}_RSP${ii}.fits >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout_nomask 		
						    else
						        echo prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR_ARRAY_PRIMARY[$ii]}_${OBSID}_RSP${ii}.fits >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout 		
						    fi				
						fi

						if [ $test == 0 ]
						then
						    if (( $subsformat == 1 ))
						    then
				   	            prepfold -noxwin -psr ${fold_pulsar} -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR_ARRAY_PRIMARY[$ii]}_${OBSID}_RSP${ii}.sub[0-9]??? >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout 2>&1 &
							    prepfold_pid[$ii]=$!  
						        echo "Running: " prepfold -noxwin -psr ${fold_pulsar} -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR_ARRAY_PRIMARY[$ii]}_${OBSID}_RSP${ii}.sub[0-9]??? >> $log
							else
							    if [ $rfi == 1 ] || [ $rfi_pproc == 1 ]
							    then
				   	               prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -mask ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}_rfifind.mask -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR_ARRAY_PRIMARY[$ii]}_${OBSID}_RSP${ii}.fits >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout 2>&1 &
				   	               sleep 10
				   	               prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii}_nomask ${PULSAR_ARRAY_PRIMARY[$ii]}_${OBSID}_RSP${ii}.fits >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout_nomask 2>&1 &
							       prepfold_pid[$ii]=$!  
						           echo "Running: " prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -mask ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}_rfifind.mask -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR_ARRAY_PRIMARY[$ii]}_${OBSID}_RSP${ii}.fits >> $log
						           echo "Running: " prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii}_nomask ${PULSAR_ARRAY_PRIMARY[$ii]}_${OBSID}_RSP${ii}.fits >> $log
						        else
				   	               prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR_ARRAY_PRIMARY[$ii]}_${OBSID}_RSP${ii}.fits >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout 2>&1 &
							       prepfold_pid[$ii]=$!  
						           echo "Running: " prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR_ARRAY_PRIMARY[$ii]}_${OBSID}_RSP${ii}.fits >> $log
						        fi
							fi
			   	        fi
						sleep 5
					fi # end if [[ $fold_pulsar == "NONE" ]] || [[ $nofold == 1 ]]
				done # folding over pulsars
				for fold_pulsar in $PULSAR_LIST
				do
				    if [[ $fold_pulsar == "NONE" ]] || [[ $nofold == 1 ]]
				    then
				        break
				    else 
					    echo "Waiting for RSP$ii prepfold to finish for pulsar $fold_pulsar"
					    echo "Waiting for RSP$ii prepfold to finish for pulsar $fold_pulsar" >> $log
					    wait ${prepfold_pid[ii]}
				    fi
				 done # end for fold_pulsar in $PULSAR_LIST	
		    done # end for ii in $num_dir		
		
		fi # end if (( $nrBeams == 1 ))
    fi # end if [[ $all_pproc == 0 ]] && [[ $rfi_pproc == 0 ]]

    # fold the RSPA location in non-multi-beam mode     
    if [[ $nrBeams == 1 ]] && [[ $PULSAR_ARRAY_PRIMARY[0] != "NONE" ]] && [[ $nofold == 0 ]]
    then
	   #When the first folding task finishes, start the folding for the "all" directory
	   if [ $all == 1 ] || [ $all_pproc == 1 ]
	   then
		  echo "Starting folding for RSPA (all subbands)"
		  echo "Starting folding for RSPA (all subbands)" >> $log
		  date
		  date >> $log

		  # Fold data per requested Pulsar
		  index=1
		  beam_index=1
		  for fold_pulsar in $PULSAR_LIST
		  do
		      if (( $flyseye == 0 ))
	          then
		         cd ${location}/${STOKES}/RSPA
		         echo cd ${location}/${STOKES}/RSPA >> $log

			     if (( $subsformat == 1 ))
			     then
		             echo prepfold -noxwin -psr ${fold_pulsar} -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSPA ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSPA.sub[0-9]??? >> ${fold_pulsar}_${OBSID}_RSPA.prepout 
		         else
		             if [ $rfi == 1 ] || [ $rfi_pproc == 1 ]
		             then
		                echo prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -mask ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}_rfifind.mask -o ${fold_pulsar}_${OBSID}_RSPA ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSPA.fits >> ${fold_pulsar}_${OBSID}_RSPA.prepout 
		                echo prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSPA_nomask ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSPA.fits >> ${fold_pulsar}_${OBSID}_RSPA.prepout_nomask 
		             else
		                echo prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSPA ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSPA.fits >> ${fold_pulsar}_${OBSID}_RSPA.prepout 
		             fi
		         fi
		         index=$index
			     if [ $test == 0 ]
			     then
				     if (( $subsformat == 1 ))
				     then
			            prepfold -noxwin -psr ${fold_pulsar} -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSPA ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSPA.sub[0-9]??? >> ${fold_pulsar}_${OBSID}_RSPA.prepout 2>&1 && touch "DONE" >> ${fold_pulsar}_${OBSID}_RSPA.prepout 2>&1 &
				        prepfold_pid_all[$index]=$!  
		                echo "Running: " prepfold -noxwin -psr ${fold_pulsar} -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSPA ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSPA.sub[0-9]??? >> $log
				     else
				        if [ $rfi == 1 ] || [ $rfi_pproc == 1 ]
				        then
			               prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -mask ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}_rfifind.mask -o ${fold_pulsar}_${OBSID}_RSPA ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSPA.fits >> ${fold_pulsar}_${OBSID}_RSPA.prepout 2>&1 && touch "DONE" >> ${fold_pulsar}_${OBSID}_RSPA.prepout 2>&1 &
			               sleep 10
			               prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSPA_nomask ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSPA.fits >> ${fold_pulsar}_${OBSID}_RSPA.prepout 2>&1 && touch "DONE" >> ${fold_pulsar}_${OBSID}_RSPA.prepout_nomask 2>&1 &
				           prepfold_pid_all[$index]=$!  
		                   echo "Running: " prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -mask ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSP${ii}_rfifind.mask -o ${fold_pulsar}_${OBSID}_RSPA ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSPA.fits >> $log
		                   echo "Running: " prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSPA_nomask ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSPA.fits >> $log
		                else
			               prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSPA ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSPA.fits >> ${fold_pulsar}_${OBSID}_RSPA.prepout 2>&1 && touch "DONE" >> ${fold_pulsar}_${OBSID}_RSPA.prepout 2>&1 &
				           prepfold_pid_all[$index]=$!  
		                   echo "Running: " prepfold -noxwin -psr ${fold_pulsar} -nsub $prepfold_nsubs -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSPA ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_RSPA.fits >> $log
		                fi
				     fi
		         fi
		         cd ${location}
		         sleep 5
			  fi # end if (( $flyseye == 0 ))
			  index=$(( $index + 1 ))
		  done # end for fold_pulsar in $PULSAR_LIST
	   fi # end if [ $all == 1 ] || [ $all_pproc == 1 ]
	fi # end if [[ $nrBeams == 1 ]] && [[ $PULSAR_ARRAY_PRIMARY[0] != "NONE" ]] && [[ $nofold == 0 ]]

		
	if [[ $all_pproc == 0 ]] && [[ $rfi_pproc == 0 ]] 
	then
		#Make a cumulative plot of the profiles
		if [[ $nrBeams == 1 ]] && [[ $PULSAR_ARRAY_PRIMARY[0] != "NONE" ]] && [[ $nofold == 0 ]] && [[ $STOKES == "incoherentstokes" ]] && [[ $subsformat == 1 ]]
		then
			for fold_pulsar in $PULSAR_LIST
			do
			    if (( $flyseye == 0 ))
			    then
			        cd ${location}/${STOKES}
			        cd ${location}/${STOKES} >> $log
					echo "Running plot summary script plot_profs8multi.py in `pwd`"
					echo "Running plot summary script plot_profs8multi.py in `pwd`" >> $log
					date
					date >> $log
					echo python ${LOFARSOFT}/release/share/pulsar/bin/plot_profs8multi.py -p ${fold_pulsar} >> $log
					python ${LOFARSOFT}/release/share/pulsar/bin/plot_profs8multi.py -p ${fold_pulsar} >> $log 2>&1
					echo convert profiles.ps ${fold_pulsar}_${OBSID}_profiles.pdf >> $log
					convert profiles.ps ${fold_pulsar}_${OBSID}_profiles.pdf
					echo rm profiles.ps >> $log
					rm profiles.ps
				else
				    # A2 - this needs to be fixed since the RSPN has to proceed ${jjj} location below
				    for jjj in $beams
				    do
				        cd ${location}/${STOKES}/${jjj}
				        cd ${location}/${STOKES}/${jjj} >> $log
						echo "Running plot summary script plot_profs8multi.py in `pwd`"
						echo "Running plot summary script plot_profs8multi.py in `pwd`" >> $log
						date
						date >> $log
						echo python ${LOFARSOFT}/release/share/pulsar/bin/plot_profs8multi.py -p ${fold_pulsar} >> $log
						python ${LOFARSOFT}/release/share/pulsar/bin/plot_profs8multi.py -p ${fold_pulsar} >> $log 2>&1
						echo convert profiles.ps ${fold_pulsar}_${OBSID}_profiles.pdf >> $log
						convert profiles.ps ${fold_pulsar}_${OBSID}_profiles.pdf
						echo rm profiles.ps >> $log
						rm profiles.ps
					done	
				fi
			done # end for fold_pulsar in $PULSAR_LIST
        fi # end if [[ $nrBeams == 1 ]] && [[ $PULSAR_ARRAY_PRIMARY[0] != "NONE" ]] && [[ $nofold == 0 ]] && [[ $STOKES == "incoherentstokes" ]] && [[ $subsformat == 1 ]]
	    echo cd ${location} >> $log
	    cd ${location}
		
		if [[ $nofold == 0 ]]
		then
			#Make a .pdf and .png version of the plots
			echo "Running convert on ps to pdf and png of the plots"
			echo "Running convert on ps to pdf and png of the plots" >> $log
			date
			date >> $log
	
	        #find all the prepfold .ps files and convert them into .pdf .png and .th.png results
	        find ./ -name "*.ps" -print | grep -v rfifind | sed 's/\.ps//g' | awk '{print "convert "$1".ps "$1".pdf; convert -rotate 90 "$1".ps "$1".png; convert -rotate 90 -crop 200x140-0 "$1".ps "$1".th.png"}' > convert.sh
	        wc_convert=`wc -l convert.sh | awk '{print $1}'`
	        if [[ $wc_convert > 0 ]]
	        then
	           chmod 777 convert.sh
	           cat convert.sh >> $log
	           ./convert.sh
	        else
	           echo "No prepfold .ps files were found for conversion to pdf and png."
	           echo "No prepfold .ps files were found for conversion to pdf and png." >> $log
	        fi
        fi # end if [[ $nofold == 0 ]]
	fi # end if [[ $all_pproc == 0 ]] && [[ $rfi_pproc == 0 ]] 
		

	if [ $rfi == 1 ] || [ $rfi_pproc == 1 ] 
	then	
		   echo cd ${location} >> $log
		   cd ${location}

		   #Check when all processes to be done, then all processes have exited
		   if (( $flyseye == 0 ))
		   then
		      for ii in $num_dir
		      do
		         echo "Waiting for RSP$ii subdyn to finish non-Fly's eye mode; pid = ${subdyn_pid[ii]}"
		         wait ${subdyn_pid[ii]}
	#	         echo "Exit status of subdyn for RSP$ii is $?"
		      done
		   else
		      for ii in $num_dir
		      do
	             counter=0
				 for jjj in $loop_beams
				 do
			         echo "Waiting for RSP$ii and $jjj subdyn to finish in Fly's eye mode pid ${subdyn_pid_[ii][counter]}"
		             wait ${subdyn_pid[ii][counter]}
	#	             echo "Exit status of subdyn for RSP$ii in beam $jjj is $?"
				     counter=$(( $counter + 1 ))
		         done
		      done
	       fi	 
	fi # end if [ $rfi == 1 ] || [ $rfi_pproc == 1 ] 
	
	#Gather the RFI RSPN/*rfiprep files into one summary RFI file
    if [ $rfi == 1 ] || [ $rfi_pproc == 1 ]
	then 
#       if (( $subsformat == 1 ))
#       then
		   if [ $nrBeams == 1 ]
		   then
			   echo "Creating RFI Summary files from RSP-split results."
			   echo "Creating RFI Summary files from RSP-split results." >> $log
		
		       max_num=$(( (( $all_num * $CHAN )) - 1 ))
		
			   if (( $flyseye == 0 ))
			   then
		          rfi_file=$location/${STOKES}/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_sub0-${max_num}.rfirep
			      if [ -f $rfi_file ]
			      then
			         rm $rfi_file
			         echo "WARNING: deleting previous version of RFI summary file: $rfi_file"
			      fi
			      
			      # put the header line into the RFI summary file
			      echo "# Subband       Freq (MHz)" > $rfi_file
			      
			      for ii in $num_dir
			      do
			         offset=$(( $all_num * $CHAN / $core * $ii ))
			         echo "RFI all_num=$all_num, chan=$CHAN, core=$core, num_dir=ii=$ii ==> offset=$offset"
			         cat $location/${STOKES}/RSP${ii}/*rfirep | grep -v "#" | awk -v offset=$offset '{printf("%d \t\t %f\n"),$1+offset, $2}' >> $rfi_file
			      done
			      
			   else
	              counter=0
			   	  for jjj in $loop_beams
				  do
		              rfi_file=$location/${STOKES}/RSP${ii}/${PULSAR_ARRAY_PRIMARY[$counter]}_${OBSID}_sub0-${max_num}.rfirep
				      if [ -f $rfi_file ]
				      then
				         rm $rfi_file
				         echo "WARNING: deleting previous version of RFI summary file: $rfi_file"
				      fi
			      
			          # put the header line into the RFI summary file
			          echo "# Subband       Freq (MHz)" > $rfi_file
		
				      for ii in $num_dir
				      do
				         offset=$(( $all_num * $CHAN / $core * $ii ))
			             echo "RFI beam=$jjj, all_num=$all_num, chan=$CHAN, core=$core, num_dir=ii=$ii ==> offset=$offset"
	#			         cat $location/${STOKES}/${jjj}/RSP${ii}/*rfirep | grep -v "#" | awk -v offset=$offset '{printf("%d \t\t %f\n"),$1+offset, $2}' >> $rfi_file
				         cat $location/${STOKES}/RSP${ii}/${jjj}/*rfirep | grep -v "#" | awk -v offset=$offset '{printf("%d \t\t %f\n"),$1+offset, $2}' >> $rfi_file
				      done
		
			      done
			   fi
		   else # if [[ $nrBeams > 1 ]]
			   echo "Merging RFI Summary files from RSP-split results from different beams. (removed)"
			   echo "Merging RFI Summary files from RSP-split results from different beams. (removed)" >> $log
	
		       max_num=$all_num
	           rfi_file=$location/${STOKES}/${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_sub0-${max_num}.rfirep
		       if [ -f $rfi_file ]
		       then
		          rm $rfi_file
		          echo "WARNING: deleting previous version of RFI summary file: $rfi_file"
		       fi
		       
		       touch $location/${STOKES}/$$.rfirep	      
		       for ii in $num_dir
		       do
		           comm $location/${STOKES}/$$.rfirep $location/${STOKES}/RSP${ii}/*rfirep | sed -e 's/^[ \t]*//' > $location/${STOKES}/$$.tmp
		           mv $location/${STOKES}/$$.tmp $location/${STOKES}/$$.rfirep
		       done
	           mv $location/${STOKES}/$$.rfirep $rfi_file
	
		   fi # end if [[ $nrBeams == 1 ]]
#		fi # end if (( $subsformat == 1 ))
	fi # end if [ $rfi == 1 ] || [ $rfi_pproc == 1 ]
		
	#Wait for the all prepfold to finish
	if [[ $nrBeams == 1 ]] && [[ $PULSAR_ARRAY_PRIMARY[0] != "NONE" ]] && [[ $nofold == 0 ]]
	then
		if [ $all -eq 1 ] || [ $all_pproc == 1 ]
		then
		   ii=1
		   yy=0
		   echo "Waiting for prepfold on entire subband list to complete..." 
		   echo "Waiting for prepfold on entire subband list to complete..." >> $log
		   date 
		   date >> $log
	
		   index=1
		   beam_index=1
		   
		   for fold_pulsar in $PULSAR_LIST
		   do
			   if (( $flyseye == 0 ))
			   then
			      echo "Waiting for Pulsar #$index prepfold (all) to finish"
			      wait ${prepfold_pid_all[$index]}
			   
				   echo convert ${STOKES}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.ps ${STOKES}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.pdf >> $log
				   convert ${STOKES}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.ps ${STOKES}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.pdf
				   echo convert -rotate 90 ${STOKES}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.ps ${STOKES}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.png >> $log
				   convert -rotate 90 ${STOKES}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.ps ${STOKES}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.png
				   echo convert -rotate 90 -crop 200x140-0 ${STOKES}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.ps ${STOKES}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.th.png >> $log
				   convert -rotate 90 -crop 200x140-0 ${STOKES}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.ps ${STOKES}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.th.png
			   else
				   for jjj in $beams
				   do
			           echo "Waiting for Pulsar #$index, beam #$beam_index prepfold (all) to finish"
				       wait ${prepfold_pid_all[$beam_index]}
				       
					   echo convert ${STOKES}/${jjj}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.ps ${STOKES}/${jjj}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.pdf >> $log
					   convert ${STOKES}/${jjj}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.ps ${STOKES}/${jjj}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.pdf
					   echo convert -rotate 90 ${STOKES}/${jjj}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.ps ${STOKES}/${jjj}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.png >> $log
					   convert -rotate 90 ${STOKES}/${jjj}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.ps ${STOKES}/${jjj}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.png
					   echo convert -rotate 90 -crop 200x140-0 ${STOKES}/${jjj}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.ps ${STOKES}/${jjj}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.th.png >> $log
					   convert -rotate 90 -crop 200x140-0 ${STOKES}/${jjj}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.ps ${STOKES}/${jjj}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.th.png
					   beam_index=$(( $beam_index + 1 ))
		           done
		       fi # end if (( $flyseye == 0 ))
		       index=$(( $index + 1 ))
	       done # end for fold_pulsar in $PULSAR_LIST
		fi # end if [ $all -eq 1 ] || [ $all_pproc == 1 ]
    fi # end if [[ $nrBeams == 1 ]] && [[ $PULSAR_ARRAY_PRIMARY[0] != "NONE" ]] && [[ $nofold == 0 ]]

	#Rename the RSP?/beam_? to their actual names based on the observation parset names -> NAME/beam_?
    if [ $flyseye == 1 ] && [ $all_pproc == 0 ] && [ $rfi_pproc == 0 ] && [ $TiedArray == 0 ]
    then
        cd ${location}/${STOKES}/
		for ii in $num_dir
		do
            cd ${location}/${STOKES}/
			N=$ii
			N=`echo "$N+1" | bc`
			beam_index=`sed -n "$N"p SB_master.list | sed 's/^.*\///g' | sed 's/.*_B//g' | sed 's/_.*raw//g' | sed 's/^0//g' | sed 's/^0//g'`
			beam_index=`echo "$beam_index+1" | bc`
			NAME=`cat $PARSET| grep "OLAP.storageStationNames" | awk -F '[' '{print $2}' | awk -F ']' '{print $1}'| awk -F "," '{print $'$beam_index'}'`
			echo "mv RSP${ii} $NAME" >> $log
			echo "mv RSP${ii} $NAME"
			mv RSP${ii} $NAME
			cd ${location}/${STOKES}/$NAME

            # add to the beam_process_node.txt for fly's eye mode here, since you know the station name
            sed -n "$N"p SB_master.list | awk -v NODE_NAME=`uname -n` -v NEW_NAME=$NAME '{print NODE_NAME " " $1 "["NAME"]"}' > $location/${STOKES}/beam_process_node.txt

		    for jjj in $loop_beams
			do
			   cd ${location}/${STOKES}/$NAME/${jjj}
			   prev_name=RSP${ii}
               new_name=$NAME
			   if [[ RSP${ii} != $NAME ]]
			   then
			       echo rename "s/$prev_name/$new_name/" *
			       echo rename "s/$prev_name/$new_name/" *  >> $log
                   rename "s/$prev_name/$new_name/" *
               fi

#			   /bin/ls *_RSP${ii}* > name_change.list
#			   while read filename
#			   do
##			      new_name=`echo $filename | sed -e "s/_RSP.*_/_$NAME_/g" -e "s/_RSP.*\./_$NAME./g"`
#                  NAME1="_"${NAME}"_PSR"
#                  NAME2="_"${NAME}
#                  NAME3="_"${NAME}"_"
#			      new_name=`echo $filename | sed   -e "s/_RSP.*PSR/$NAME1/g"  -e "s/_RSP.*\./$NAME2\./g" -e "s/_RSP.*_/$NAME3/g"`
#			      mv $filename $new_name
#			   done < name_change.list

#			   inf_file=`/bin/ls *.inf`
#			   if [[ RSP${ii} != $NAME ]]
#			   then
#			      sed   -e "s/$prev_name/$new_name/g" $inf_file > $$.inf
#			   else 
#			      cp $inf_file $$.inf
#			   fi
#			   new_inf_file=`echo $inf_file | sed 's/\.inf/\.sub\.inf/'`
#			   echo mv $$.inf $new_inf_file
#			   echo mv $$.inf $new_inf_file >> $log
#			   mv $$.inf $new_inf_file
#			   rm $inf_file
			done
		done
		cd ${location}	    
	#Rename the TA beams RSP0 to RSP<beam numner>
    elif [[ $all_pproc == 0 ]] && [[ $rfi_pproc == 0 ]] && [[ $STOKES == "stokes" ]] && [ $TiedArray == 0 ]
    then
        cd ${location}/${STOKES}/
		for ii in $num_dir
		do
			N=$ii
			N=`echo "$N+1" | bc`  #actual line number is one higher than the RSP number
            beam_index=`sed -n "$N"p SB_master.list | sed 's/^.*\///g' | sed 's/.*_B//g' | sed 's/_.*raw//g' | sed 's/^0//g' | sed 's/^0//g'`
            echo "The beam_index is $beam_index" >> $log
            cd ${location}/${STOKES}/
			NAME=$beam_index
			mkdir -p tmp$$
			echo "mv RSP${ii} tmp$$/RSP$NAME" >> $log
			echo "mv RSP${ii} tmp$$/RSP$NAME"
			mv RSP${ii} tmp$$/RSP${NAME}
			cd ${location}/${STOKES}/tmp$$/RSP${NAME}
			if [[ RSP${ii} != RSP${NAME} ]]
			then
			   echo "rename 's/RSP$ii/RSP$NAME/g' *" >> $log
			   echo "rename 's/RSP$ii/RSP$NAME/g' *"
			   rename "s/RSP$ii/RSP$NAME/g" *
			fi
#		    inf_file=`/bin/ls *.inf`
#		    sed -e 's/RSP$ii/RSP$NAME/g'  $inf_file > $$.inf
#			new_inf_file=`echo $inf_file | sed 's/\.inf/\.sub\.inf/'`
#		    echo mv $$.inf $new_inf_file >> $log
#		    echo mv $$.inf $new_inf_file
#		    mv $$.inf $new_inf_file
#		    rm $inf_file
		done
		cd ${location}/${STOKES}/tmp$$/
		mv * ../
		cd ..
		rmdir tmp$$
		cd ${location}	
    elif [[ $all_pproc == 0 ]] && [[ $rfi_pproc == 0 ]] && [[ $STOKES == "stokes" ]] && [[ $TiedArray == 1 ]]
    then
        cd ${location}/${STOKES}/
		for ii in $num_dir
		do
			N=$ii
			N=`echo "$N+1" | bc`  #actual line number is one higher than the RSP number
            beam_index=`sed -n "$N"p SB_master.list | sed 's/^.*\///g' | sed 's/.*_B//g' | sed 's/_.*raw//g' | sed 's/^0//g' | sed 's/^0//g'`
            echo "The beam_index is $beam_index" >> $log
            cd ${location}/${STOKES}/
			#NAME=$beam_index
			mkdir -p tmp$$
			echo "mv RSP${ii} tmp$$/" >> $log
			echo "mv RSP${ii} tmp$$/"
			mv RSP${ii} tmp$$/
			cd ${location}/${STOKES}/tmp$$/RSP${ii}
            
			counter=0
			loop_beams=$beams
			echo loop_beams == "$loop_beams"
		    for jjj in $loop_beams
			do
			    cd ${location}/${STOKES}/tmp$$/RSP${ii}
		        M=$counter
				M=`echo "$M+1" | bc`  #actual line number is one higher than the RSP number
	            beam_index=`sed -n "$M"p ${location}/${STOKES}/SB_master.list | sed 's/^.*\///g' | sed 's/.*_B//g' | sed 's/_.*raw//g' | sed 's/^0//g' | sed 's/^0//g'`
	            echo "The M beam_index is $beam_index" >> $log
                cd ${location}/${STOKES}/tmp$$/RSP${ii}/${jjj}
                prev_name=RSP${ii}
                new_name=RSP${beam_index}
			    if (( $beam_index != $ii ))
			    then
			       echo rename "s/$prev_name/$new_name/" *
			       echo rename "s/$prev_name/$new_name/" *  >> $log
                   rename "s/$prev_name/$new_name/" *
                fi
#			    /bin/ls * > name_change.list
#			    NAME=RSP${beam_index}
#			    if (( $beam_index != $ii ))
#			    then
#			       #rename files
#				   while read filename
#				   do
#				      # A2 fix the ORIG files which get changed to files without subband numbers
#	#			      new_name=`echo $filename | sed -e "s/_RSP.*_/_$NAME_/g" -e "s/_RSP.*\./_$NAME./g"`
#	                  NAME1="_"${NAME}"_PSR"
#	                  NAME2="_"${NAME}
#	                  NAME3="_"${NAME}"_"
#				      new_name=`echo $filename | sed   -e "s/_RSP.*PSR/$NAME1/g"  -e "s/_RSP.*\./$NAME2\./g" -e "s/_RSP.*_/$NAME3/g"`
#				      echo mv $filename $new_name
#				      mv $filename $new_name
#				   done < name_change.list
#			   fi 

#			   inf_file=`/bin/ls *.inf`
#			   if (( $beam_index != $ii ))
#			   then
#			      sed   -e "s/$prev_name/$new_name/g" $inf_file > $$.inf
#			   else 
#			      cp $inf_file $$.inf
#			   fi
#			   new_inf_file=`echo $inf_file | sed 's/\.inf/\.sub\.inf/'`
#			   echo mv $$.inf $new_inf_file
#			   echo mv $$.inf $new_inf_file >> $log
#			   mv $$.inf $new_inf_file
#			   rm $inf_file
               cd ..
               echo mv ${jjj} ../../RSP${beam_index}
               echo mv ${jjj} ../../RSP${beam_index} >> $log
               mv ${jjj} ../../RSP${beam_index}
		       counter=$(( $counter + 1 ))

			done
			cd ${location}/${STOKES}/tmp$$/RSP${ii}
            echo mv * ../../
            mv * ../../
		done
		cd ${location}/${STOKES}/tmp$$/
		ls
		rmdir RSP${ii}
		cd ..
		rmdir -p tmp$$
		cd ${location}	
    fi

	cd ${location}	

    echo $core > $location/${STOKES}/nof_cores.txt
    if [[ $STOKES == "stokes" ]] 
    then 
       if (( $flyseye == 0 ))
       then
          cat $master_list | awk -v NODE_NAME=`uname -n` '{print NODE_NAME " " $1}' > $location/${STOKES}/beam_process_node.txt
       fi
    fi

	#create a delete list of subband files for future clean up
	if [[ $STOKES == "incoherentstokes" ]] 
	then
	   find `pwd`/incoherentstokes -name "*sub????*" -print | egrep -v "RSPA|inf|ps|pdf|png|rfirep" | awk '{print "if [ -f "$1" ]\nthen  \n   rm -f "$1 "\n echo rm -f "$1 "\nfi"}' > IS_delete_sub.list
	else
       if (( $flyseye == 0 )) || (( $TiedArray == 1 ))
       then
	      find `pwd`/stokes -name "*sub????*" -print | egrep -v "RSPA|inf|ps|pdf|png|rfirep" | awk '{print "if [ -f "$1" ]\nthen  \n   rm -f "$1 "\n echo rm -f "$1 "\nfi"}' > CS_delete_sub.list
       else
	      find `pwd`/stokes -name "*sub????*" -print | egrep -v "RSPA|inf|ps|pdf|png|rfirep" | awk '{print "if [ -f "$1" ]\nthen  \n   rm -f "$1 "\n echo rm -f "$1 "\nfi"}' > FE_delete_sub.list
       fi	
	fi
	
	#run the heat map for multiple TA beams
	cd $location
    if (( $nrTArings > 0 ))
    then
       echo "plot_LOFAR_TA_multibeam.py --chi chi-squared.txt --parset ${OBSID}.parset --out_logscale ${OBSID}_TA_heatmap_log.png --out_linscale ${OBSID}_TA_heatmap_linear.png --target ${PULSAR_ARRAY_PRIMARY[0]}" > TA_heatmap.sh
       chmod 777 TA_heatmap.sh
       echo "Created TA_heatmap.sh which will be run after hoovering step"
       echo "Created TA_heatmap.sh which will be run after hoovering step" >> $log
    fi

done # for loop over modes in $mode_str 

if [[ $proc != 0 ]]
then
	#Make a combined png of all the th.png files
	echo "Combining all th.png files into one"
	echo "Combining all th.png files into one" >> $log
	date
	date >> $log
	#run the combine thumbnail script;  needs just the log file as input arg
	echo "running:  thumbnail_combine.sh $log"
	echo "running:  thumbnail_combine.sh $log" >> $log
	thumbnail_combine.sh $log
		
	cd ${location}

	#Make a tarball of all the plots
	echo "Creating tar file of plots"
	echo "Creating tar file of plots" >> $log
	date
	date >> $log
	#tar_list="*/*profiles.pdf */RSP*/*pfd.ps */RSP*/*pfd.pdf */RSP*/*pfd.png */RSP*/*pfd.th.png */RSP*/*pfd.bestprof */RSP*/*.sub.inf */*.rfirep"
	tar_list=`find ./ -type f \( -name "*.pdf" -o -name "*.ps" -o -name "*.pfd" -o -name "*.inf" -o -name "*.rfirep" -o -name "*png" -o -name "*out" -o -name "*parset" \)`
	echo "tar cvzf ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_plots.tar.gz  $tar_list" >> $log
	tar cvzf ${PULSAR_ARRAY_PRIMARY[0]}_${OBSID}_plots.tar.gz $tar_list
	
	#echo gzip ${PULSAR}_${OBSID}_plots.tar >> $log
	#gzip ${PULSAR}_${OBSID}_plots.tar
	
	#Change permissions and move files
	echo "Changing permissions of files"
	echo "Changing permissions of files" >> $log
	date
	date >> $log
	echo chmod 775 -R . * >> $log
	chmod 775 -R . * 
	#echo chgrp -R pulsar . * >> $log
	#chgrp -R pulsar . * 
		
	date_end=`date`
	echo "Start Time: " $date_start
	echo "Start Time: " $date_start >> $log
	echo "End Time: " $date_end
	echo "End Time: " $date_end >> $log
	
	echo "Results output location:" $location
else  # end if [[ $proc != 0 ]]
    #no processing done, clean up
    cd $location/..
    echo "ERROR: No processing was performed due to no input data found"
    if [[ -d $location ]]
    then
       rm -rf $location
    fi
    exit 1
fi

exit 0
