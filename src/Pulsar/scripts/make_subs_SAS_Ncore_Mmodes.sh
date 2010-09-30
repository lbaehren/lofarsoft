#!/bin/ksh 
#Convert raw LOFAR data
#Workes on incoherent, coherent and fly's eye data.
# N core defaul is = 8 (cores)

#PLEASE increment the version number when you edit this file!!!
VERSION=1.26

#Check the usage
USAGE="\nusage : make_subs_SAS_Ncore_Mmodes.sh -id OBS_ID -p Pulsar_names -o Output_Processing_Location [-core N] [-all] [-all_pproc] [-rfi] [-rfi_ppoc] [-C] [-del] [-incoh_only] [-coh_only] [-incoh_redo] [-coh_redo] [-transpose] [-help]\n\n"\
"      -id OBS_ID  ==> Specify the Observation ID (i.e. L2010_06296) \n"\
"      -p Pulsar_names ==> Specify the Pulsar Name or comma-separated list of Pulsars for folding (w/o spaces) or\n"\
"         specify the word 'position' (lower case) find associated known Pulsars in the FOV of observation\n"\
"         (i.e. single Pulsar: B2111+46) (i.e. multiple pulsars to fold:  B2111+46,B2106+44) \n"\
"         (i.e. up to 3 brights pulsars to fold at location of FOV: position \n"\
"      -o Output_Processing_Location ==> Specify the Output Processing Location \n"\
"         (i.e. /net/sub5/lse013/data4/LOFAR_PULSAR_ARCHIVE_lse013/L2010_06296_red) \n"\
"      [-all] ==> optional parameter perform folding on entire subband set in addition to N-splits (takes 11 extra min)\n"\
"      [-all_pproc] ==> Post-Processing optional parameter to ONLY perform folding on entire subband set based on already-processed N-splits\n"\
"      [-rfi] ==> optional parameter perform Vlad's RFI checker and only use clean results (takes 7 extra min)\n"\
"      [-rfi_pproc] ==> Post-Processing optional parameter to perform Vlad's RFI checker on already-processed N-splits\n"\
"      [-C | -c] ==> optional parameter to switch on bf2presto COLLAPSE (Collapse all channels in MS to a single .sub file)\n"\
"      [-del] ==> optional parameter to delete the previous ENTIRE Output_Processing_Location if it exists (override previous results!)\n"\
"      [-core N] ==> optional parameter to change the number of cores (splits) used for processing (default = 8)\n"\
"      [-incoh_only] ==> optional parameter to process ONLY Incoherentstokes (even though coherentstokes data exist)\n"\
"      [-coh_only] ==> optional parameter to process ONLY Coherentstokes  (even though incoherentstokes data exist)\n"\
"      [-incoh_redo] ==> optional parameter to redo processing for Incoherentstokes (deletes previous incoh results!)\n"\
"      [-coh_redo] ==> optional parameter to redo processing for Coherentstokes (deletes previous coh results!)\n"\
"      [-transpose] ==> optional parameter to indicate the input data were run through the TAB 2nd transpose\n"\
"      [-help] ==> optional parameter which prints the usage and examples of how to run the pipeline\n"

if [ $# -lt 1 ]                    # this script needs at least 6 args, including -switches
then
   print "$USAGE"    
   exit 1
fi

#Get the input arguments
OBSID=""
PULSAR=""
PULSAR_LIST=""
location=""
COLLAPSE=""
delete=0
all=0
all_pproc=0
rfi=0
rfi_pproc=0
core=8
incoh_only=0
coh_only=0
incoh_redo=0
coh_redo=0
transpose=0
help=0
input_string=$*
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
	-all)  all=1;;
	-all_pproc)  all_pproc=1;;
	-core) core=$2; shift;;
	-incoh_only)  incoh_only=1;;
	-coh_only)    coh_only=1;;
	-incoh_redo)  incoh_redo=1;;
	-coh_redo)    coh_redo=1;;
	-transpose)   transpose=1;;
	-help)   help=1 
	         cat $LOFARSOFT/release/share/pulsar/data/pulp_help.txt
	         exit 1;;
	-*)
	    echo >&2 \
	    "$USAGE"
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

# Print the basic information about input parameters to STDOUT at start of pipeline run
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

if [ $core -lt 1 ] || [ $core -gt 8 ]
then
   echo "ERROR: Number of cores must be 1 >= N <= 8 ;  currently requested $core cores."
   exit 1
fi

if [ $incoh_redo -eq 1 ]
then
   echo "   Performing redo of Incoherentstokes processing" 
fi

if [ $coh_redo -eq 1 ]
then
   echo "   Performing redo of Coherentstokes processing" 
fi

if [ $incoh_only -eq 1 ]
then
   echo "   Performing processing of Incoherentstokes data ONLY" 
fi

if [ $coh_only -eq 1 ]
then
   echo "   Performing processing of Coherentstokes data ONLY" 
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

#Check whether Output Processing Location already exists
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

#Set these parameters by hand
###OBSID=L2010_06296

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
	PARSET=/globalhome/lofarsystem/log/${OBSID}/RTCP.parset.0
	short_id=`echo $OBSID | sed 's/L.....//g'`
	
	if [ ! -f $PARSET ] 
	then
	   new_parset=`find /globalhome/lofarsystem/log/ -name RTCP-${short_id}.parset -print`
	   if [[ $new_parset == "" ]]
	   then
	      new_parset=`find /globalhome/lofarsystem/production/lofar-trunk/bgfen/log/ -name RTCP-${short_id}.parset -print`
   	      if [[ $new_parset == "" ]]
	      then
	          # Sept 23, 2010 another parset location added
	          new_parset=/globalhome/lofarsystem/log/L${short_id}/L${short_id}.parset
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

#alias bf2presto=/home/hassall/bin/bf2presto-working

######################
# DON'T CHANGE BELOW #
######################
date_start=`date`

#Set up generic pipeline version log file
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
echo "$0 $input_string" >> $log
echo "Start date: $date_start" >> $log
echo "PARSET:" $PARSET
echo "PARSET:" $PARSET >> $log

NBEAMS=`cat $PARSET | grep "OLAP.storageStationNames" | grep -v Control | awk -F '[' '{print $2}' | awk -F ']' '{print $1}'| awk -F\, '{print NF}'`
ARRAY=`cat $PARSET | grep "Observation.bandFilter" | awk -F "= " '{print $2}' | awk -F "_" '{print $1}'`
CHAN=`cat $PARSET | grep "Observation.channelsPerSubband" | awk -F "= " '{print $2}'`
DOWN=`cat $PARSET | grep "OLAP.Stokes.integrationSteps" | grep -v ObservationControl | awk -F "= " '{print $2}'`
MAGIC_NUM=`cat $PARSET | grep "OLAP.CNProc.integrationSteps" | awk -F "= " '{print $2}'`
SAMPLES=`echo ${MAGIC_NUM}/${DOWN}| bc`
FLYSEYE=`cat $PARSET | grep "OLAP.PencilInfo.flysEye" | head -1 | awk -F "= " '{print $2}'`
INCOHERENTSTOKES=`cat $PARSET | grep "OLAP.outputIncoherentStokes"  | head -1 | awk -F "= " '{print $2}'`
COHERENTSTOKES=`cat $PARSET | grep "OLAP.outputCoherentStokes"  | head -1 | awk -F "= " '{print $2}'`
CHANPFRAME=`cat $PARSET | grep "OLAP.nrSubbandsPerFrame"  | head -1 | awk -F "= " '{print $2}'`
SUBSPPSET=`cat $PARSET | grep "OLAP.subbandsPerPset"  | head -1 | awk -F "= " '{print $2}'`
nrBeams=`cat $PARSET | grep "Observation.nrBeams"  | head -1 | awk -F "= " '{print $2}'`

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

if [[ $nrBeams > 1 ]] && [[ $PULSAR == "position" ]]
then
   echo "Unable to run pipeline based on input position of multiple beams;  single beam only"
   exit 1
fi

# Pulsar = "position" indicates that the observation is based on a position setting;
# figure out what the brightest/closest N Pulsars to the position (N=3)
ANT_SHORT=`cat $PARSET | grep "Observation.antennaArray"  | head -1 | awk -F "= " '{print $2}'`
is_3c=`echo $PULSAR | grep -i 3C`
is_bj=`echo $PULSAR | egrep -i "B|J"`
if [[ $PULSAR == "position" ]]
then
    pi=$(echo "scale=10; 4*a(1)" | bc -l)
	RRA=`cat $PARSET | grep "Observation.Beam\[0\].angle1"  | head -1 | awk -F "= " '{print $2}'`
	RDEC=`cat $PARSET | grep "Observation.Beam\[0\].angle2"  | head -1 | awk -F "= " '{print $2}'`
    echo "Position $RRA $RDEC (radians) will be used to find a pulsar in the field"
    RA_DEG=`echo "scale=10; $RRA / $pi * 180.0" | bc | awk '{printf("%3.9f\n",$1)}'`
    DEC_DEG=`echo "scale=10; $RDEC / $pi * 180.0" | bc | awk '{printf("%3.9f\n",$1)}'`
    echo "Position is $RA_DEG $DEC_DEG (deg)"
    get_list=`pulsars_at_location.sh $RA_DEG $DEC_DEG $ANT_SHORT 3`
    if [[ $get_list == "ERROR" ]] || [[ $get_list == "NONE" ]]
    then
       echo "ERROR: Unable to find Pulsar in FOV of pointing ($RA_DEG $DEC_DEG); unable to run the pipeline"
       exit 1
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
       exit 1
    fi
    matched_str=`grep "$PULSAR " $LOFARSOFT/release/share/pulsar/data/3cCatalog_psr$ANT_SHORT.txt | awk '{print $4}'`
    if [[ $matched_str == "NONE" ]]
    then
       echo "ERROR: No known pulsars found in $ANT_SHORT FOV of $PULSAR;  unable to run the pipeline."
       exit 1
    else
       PULSAR=`echo $matched_str | awk -v MAX=3 -F"," '{ for (i=1; i<=MAX; i++) printf "%s,", $i; printf "\n"; }' | sed 's/,,*$//g' | sed 's/,$//g'`
       PULSAR_LIST=$PULSAR
    fi
elif [[ $is_bj == "" ]]
then
    echo "ERROR: Unble to process object '$PULSAR';  not a Pulsar name nor 3C object."
    echo "       Try using '-p position' to set off the pipeline using the target location."
    exit 1
fi

is_psr_list=`echo $PULSAR_LIST | grep ","`
if [[ $is_psr_list != "" ]]
then
   multi_fold=1
   PULSAR=`echo $PULSAR_LIST | awk -F"," '{print $1}'`
   nfolds=`echo $PULSAR_LIST | awk -F"," '{print NF}'`
   PULSAR_LIST=`echo $PULSAR_LIST | sed 's/\,/ /g'`
else
   multi_fold=0
   PULSAR=$PULSAR_LIST
   nfolds=1
fi

echo "Main PULSAR:" $PULSAR 
echo "Main PULSAR:" $PULSAR >> $log
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
echo "Incoherentstokes set to:" $INCOHERENTSTOKES
echo "Incoherentstokes set to:" $INCOHERENTSTOKES >> $log
echo "Coherentstokes set to:" $COHERENTSTOKES
echo "Coherentstokes set to:" $COHERENTSTOKES >> $log
echo "FlysEye set to:" $FLYSEYE 
echo "FlysEye set to:" $FLYSEYE >> $log
if [[ $multi_fold == 1 ]]
then
   echo "Will perform folding for the following $nfolds Pulsars: $PULSAR_LIST"
fi

echo "Starting Time"
echo "Starting Time" >> $log
date
date >> $log

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
   mode_str="incoherentstokes"
   nmodes=1
   echo "WARNING: Unable to determine stokes type from parset, using Incoherentstokes (only) mode for processing."
   echo "WARNING: Unable to determine stokes type from parset, using Incoherentstokes (only) mode for processing." >> $log
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

user_core=$core

###############################################################################
# Main program loop over "incoherentstokes" and coherentstokes/"stokes" strings
###############################################################################
for modes in $mode_str
do

    STOKES=$modes

	if [[ $transpose == 1 ]] && [[ $STOKES == 'stokes' ]] && [[ $user_core > 1 ]]
	then
	   core=1
	   echo "WARNING: 2nd Transpose CoherentStokes must have N cores=1;  resetting user-specified core to 1."
	   echo "WARNING: 2nd Transpose data processing takes a long time since processing is not split between cores."
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
	
	if [ $all_pproc == 1 ] || [ $rfi_pproc == 1 ]
	then
	    # find the number of RSP? directories;  this is the number of cores used previously for processing
	    if [ $flyseye == 0 ]
	    then
	       core=`ls -F $location/${STOKES} | grep -w 'RSP[0-7].' | wc -l`
	    else
	       # for multiple beams / divide by the number of beams to get the splits 
	       core=`ls -F $location/${STOKES}/* | grep -w 'RSP[0-7].' | wc -l | awk -v nbeams=$NBEAMS '{print $1 / nbeams}'`	    
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
		if (( $transpose == 0 )) 
		then		    
			all_list=`ls /net/sub?/lse*/data?/${OBSID}/SB*.MS.${STOKES} | sort -t B -g -k 2`
		#XXX	all_list=`ls /net/sub[456]/lse01[35]/data?/${OBSID}/SB*.MS.${STOKES} | sort -t B -g -k 2`
			ls /net/sub?/lse*/data?/${OBSID}/SB*.MS.${STOKES} | sort -t B -g -k 2 > $master_list
		#XXX	ls /net/sub[456]/lse01[35]/data?/${OBSID}/SB*.MS.${STOKES} | sort -t B -g -k 2 > $master_list
	    else

		    if [[ $STOKES == "incoherentstokes" ]]
		    then
		       fname="SB*incoh*"
		    elif [[ $STOKES == "stokes" ]]
		    then
		       fname="B*_S*-bf.raw"
		    else
		       echo "ERROR: Unable to determine the file naming convension - not incoherent or coherent stokes" 
		       exit 1
		    fi

			all_list=`ls /net/sub?/lse*/data?/${OBSID}/L${short_id}_${fname} | sort -t B -g -k 2`
			ls /net/sub?/lse*/data?/${OBSID}/L${short_id}_${fname} | sort -t B -g -k 2 > $master_list
	    fi
	    
        all_num=`wc -l $master_list | awk '{print $1}'`
		echo "Found a total of $all_num SB ${STOKES} input datafiles to process" 
		echo "Found a total of $all_num SB ${STOKES} input datafiles to process" >> $log
		
		if [[ $transpose == 1 ]] && [[ $STOKES == "stokes" ]]
        then
           all_num=`echo "$CHANPFRAME * $SUBSPPSET" | bc`
  		   echo "2nd transpose has $all_num subbands in one file" 
  		   echo "2nd transpose has $all_num subbands in one file" >> $log
        fi
        
		if [ $all_num -lt $core ]
		then
		  echo "ERROR: Less than $core subbands found, unlikely to be a valid observation"
		  echo "ERROR: Less than $core subbands found, unlikely to be a valid observation" >> $log
		  exit 1
		fi
		
		modulo_files=`echo $all_num $core | awk '{print ($1 % $2)}'`
		# If the number of cores does not evenly divide into the # of files, then find another good value
		if (( $modulo_files != 0 ))
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
		fi
		    
		div_files=`echo $all_num $core | awk '{print $1 / $2}'`
		count=0
		
		#Create N-split sections of the file list
		echo split -a 1 -d -l $div_files $master_list ${STOKES}/$$"_split_"
		echo split -a 1 -d -l $div_files $master_list ${STOKES}/$$"_split_" >> $log
		split -a 1 -d -l $div_files $master_list ${STOKES}/$$"_split_"
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

	if [ $core -eq 1 ]
	then
	   num_dir="0"
	   last=0
	elif [ $core -eq 2 ]
	then
	   num_dir="0 1"
	   last=1
	elif [ $core -eq 3 ]
	then
	   num_dir="0 1 2"
	   last=2
	elif [ $core -eq 4 ]
	then
	   num_dir="0 1 2 3"
	   last=3
	elif [ $core -eq 5 ]
	then
	   num_dir="0 1 2 3 4"
	   last=4
	elif [ $core -eq 6 ]
	then
	   num_dir="0 1 2 3 4 5"
	   last=5
	elif [ $core -eq 7 ]
	then
	   num_dir="0 1 2 3 4 5 6"
	   last=6
	elif [ $core -eq 8 ]
	then
	   num_dir="0 1 2 3 4 5 6 7"
	   last=7
	fi
		
	#Set up the strings for the beam names when in fly's eye mode;  max 8 beams currently allowed
	beams=""
	if (( $flyseye == 1 )) 
	then
	   if (( $NBEAMS == 1 ))
	   then
	      beams="beam_0"
	      last_beam="beam_0"
	   elif (( $NBEAMS == 2 ))
	   then
	      beams="beam_0 beam_1"
	      last_beam="beam_1"
	   elif (( $NBEAMS == 3 ))
	   then
	      beams="beam_0 beam_1 beam_2"
	      last_beam="beam_2"
	   elif (( $NBEAMS == 4 ))
	   then
	      beams="beam_0 beam_1 beam_2 beam_3"
	      last_beam="beam_3"
	   elif (( $NBEAMS == 5 ))
	   then
	      beams="beam_0 beam_1 beam_2 beam_3 beam_4"
	      last_beam="beam_4"
	   elif (( $NBEAMS == 6 ))
	   then
	      beams="beam_0 beam_1 beam_2 beam_3 beam_4 beam_5"
	      last_beam="beam_5"
	   elif (( $NBEAMS == 7 ))
	   then
	      beams="beam_0 beam_1 beam_2 beam_3 beam_4 beam_5 beam_6"
	      last_beam="beam_6"
	   elif (( $NBEAMS == 8 ))
	   then
	      beams="beam_0 beam_1 beam_2 beam_3 beam_4 beam_5 beam_6 beam_7"
	      last_beam="beam_7"
	   else
	      echo "ERROR: unable to work on more than 8 beams in this pipeline"
	      echo "ERROR: unable to work on more than 8 beams in this pipeline" >> $log
	      exit 1
	   fi
	fi

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

	#Set up the list of files called "DONE" for output checking of prepfold results
	done_list=""
	if (( $flyseye == 0 ))
	then
		for ii in $num_dir
		do
		   if [ $ii -ne $last ]
		   then
		      done_list=`echo "&& -e "${STOKES}/"RSP"${ii}/"DONE" $done_list`
		   else
		      done_list=`echo ${STOKES}/"RSP"${ii}/"DONE"  $done_list`
		   fi   
		done
	else
	    for ii in $num_dir
	    do
		   for jjj in $beams
	       do
			   if [ $ii -ne $last ]
			   then
			      done_list=`echo "&& -e "${STOKES}/"RSP"${ii}/${jjj}/"DONE" $done_list`
			   else
			      done_list=`echo ${STOKES}/"RSP"${ii}/${jjj}/"DONE"  $done_list`
			   fi   
		   done	
		done
	fi # end if (( $flyseye == 0 ))
    
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
	#	       done
		   done
		fi
	
		if [ $core -eq 1 ]
		then
		   echo "Warning - turning off 'all' processing since number of cores/slipts is 1 and therefore all are done together in RSP0"
		   echo "Warning - turning off 'all' processing since number of cores/slipts is 1 and therefore all are done together in RSP0" >> $log
		   all=0
		fi
		
		chmod -R 774 . * 
		chgrp -R pulsar . *
    fi # end if [ $all_pproc == 0 ] && [ $rfi_pproc == 0 ]

	  	
    #Move the split lists to the appropriate directories
#	if (( $flyseye == 0 ))
#	then
    if [ $all_pproc == 0 ] && [ $rfi_pproc == 0 ]
    then
		for ii in $num_dir
		do
		  echo mv ${STOKES}/$$"_split_"$ii ${STOKES}/"RSP"${ii}/"RSP"${ii}".list" >> $log
		  mv ${STOKES}/$$"_split_"$ii ${STOKES}/"RSP"${ii}/"RSP"${ii}".list"
		done
	fi
#	else
#	    for ii in $num_dir
#		do
#			for jjj in $beams
#			do
#			  echo cp ${STOKES}/$$"_split_"$ii ${STOKES}/"RSP"${ii}/${jjj}/"RSP"${ii}".list" >> $log
#			  cp ${STOKES}/$$"_split_"$ii ${STOKES}/"RSP"${ii}/${jjj}/"RSP"${ii}".list"
#			done
#		done
#		echo rm ${STOKES}/$$"_split_"* >> $log
#		rm ${STOKES}/$$"_split_"*
#    fi	
	
	#Convert the subbands with bf2presto
	if [ $all_pproc == 0 ] && [ $rfi_pproc == 0 ]
	then
		echo "Starting bf2presto8 conversion for RSP-splits"
		echo "Starting bf2presto8 conversion for RSP-splits" >> $log
		date
		date >> $log
		
		if (( $flyseye == 0 ))
		then
			for ii in $num_dir
			do
			  echo 'Converting subbands: '`cat ${STOKES}/"RSP"$ii"/RSP"$ii".list"` >> ${STOKES}/RSP$ii"/bf2presto_RSP"$ii".out" 2>&1 
			  if [[ $transpose == 0 ]] || [[ $STOKES == "incoherentstokes" ]]
			  then
			     echo bf2presto8 ${COLLAPSE} -A 10 -f 0 -c ${CHAN} -n ${DOWN} -N ${SAMPLES} -o ${STOKES}/RSP$ii"/"${PULSAR}_${OBSID}"_RSP"$ii `cat ${STOKES}/"RSP"$ii"/RSP"$ii".list"` >> $log  
			     bf2presto8 ${COLLAPSE} -A 10 -f 0 -c ${CHAN} -n ${DOWN} -N ${SAMPLES} -o ${STOKES}/RSP$ii"/"${PULSAR}_${OBSID}"_RSP"$ii `cat ${STOKES}/"RSP"$ii"/RSP"$ii".list"` >> ${STOKES}"/RSP"$ii"/bf2presto_RSP"$ii".out" 2>&1 &
			     bf2presto_pid[$ii]=$!  
			  else
			     echo bf2presto8 ${COLLAPSE} -T ${all_num} -A 10 -f 0 -c ${CHAN} -n ${DOWN} -N ${SAMPLES} -o ${STOKES}/RSP$ii"/"${PULSAR}_${OBSID}"_RSP"$ii `cat ${STOKES}/"RSP"$ii"/RSP"$ii".list"` >> $log  
			     bf2presto8 ${COLLAPSE} -T ${all_num} -A 10 -f 0 -c ${CHAN} -n ${DOWN} -N ${SAMPLES} -o ${STOKES}/RSP$ii"/"${PULSAR}_${OBSID}"_RSP"$ii `cat ${STOKES}/"RSP"$ii"/RSP"$ii".list"` >> ${STOKES}"/RSP"$ii"/bf2presto_RSP"$ii".out" 2>&1 &
			     bf2presto_pid[$ii]=$!  
			  fi
			done
		else
			for ii in $num_dir
			do	
			    # note, should be in STOKES/RSP? directory because output gets "beam_N" PREFIX
			    cd ${location}/${STOKES}/"RSP"${ii}
	
		 	    echo 'Converting subbands: '`cat RSP"$ii".list"` >> "bf2presto_RSP"$ii".out" 2>&1 
		 	    if [[ $transpose == 0 ]] || [[ $STOKES == "incoherentstokes" ]]
			    then
			       echo bf2presto8 ${COLLAPSE} -b ${NBEAMS} -f 0 -c ${CHAN} -n ${DOWN} -N ${SAMPLES} -o ${PULSAR}_${OBSID}"_RSP"$ii `cat "RSP"$ii".list"` >> $log  
			       bf2presto8 ${COLLAPSE} -b ${NBEAMS} -f 0 -c ${CHAN} -n ${DOWN} -N ${SAMPLES} -o ${PULSAR}_${OBSID}"_RSP"$ii `cat "RSP"$ii".list"` >> "bf2presto_RSP"$ii".out" 2>&1 &
			       bf2presto_pid[$ii]=$!  
			    else
			       echo bf2presto8 ${COLLAPSE} -T ${all_num} -b ${NBEAMS} -f 0 -c ${CHAN} -n ${DOWN} -N ${SAMPLES} -o ${PULSAR}_${OBSID}"_RSP"$ii `cat "RSP"$ii".list"` >> $log  
			       bf2presto8 ${COLLAPSE} -T ${all_num} -b ${NBEAMS} -f 0 -c ${CHAN} -n ${DOWN} -N ${SAMPLES} -o ${PULSAR}_${OBSID}"_RSP"$ii `cat "RSP"$ii".list"` >> "bf2presto_RSP"$ii".out" 2>&1 &
			       bf2presto_pid[$ii]=$!  
			    fi
	#			echo 'Converting subbands: '`cat SB_master.list` >> bf2presto.out 2>&1 
	#		    echo bf2presto ${COLLAPSE} -b ${NBEAMS} -f 0 -c ${CHAN} -n ${DOWN} -N ${SAMPLES} -o ${PULSAR}_${OBSID} `cat SB_master.list` >> $log
	#		    bf2presto ${COLLAPSE} -b ${NBEAMS} -f 0 -c ${CHAN} -n ${DOWN} -N ${SAMPLES} -o ${PULSAR}_${OBSID} `cat SB_master.list` >> bf2presto.out 2>&1 &
	#			set bf2presto_pid=$!  
			done
			cd ${location}
	    fi
	
		echo "Running bf2presto8 in the background for RSP-splits..." 

#	if (( $flyseye == 0 ))
#	then
		for ii in $num_dir
		do
		   echo "Waiting for RSP$ii bf2presto to finish"
		   wait ${bf2presto_pid[ii]}
		done
#    else
#		echo "Waiting for bf2presto to finish"
#		wait $bf2presto_pid
#    fi
	
		echo "Done bf2presto8 (splits)" 
		echo "Done bf2presto8 (splits)" >> $log
		date
		date >> $log
	
	fi # end if [ $all_pproc == 0 ] && [ $rfi_pproc == 0 ]

	# Calculating the number of samples to be passed to inf-file
	if (( $flyseye == 0 ))
	then
		NSAMPL=`ls -l ${location}/${STOKES}/RSP0/${PULSAR}_${OBSID}_RSP0.sub0000 | awk '{print $5 / 2}' -`
	else
		NSAMPL=`ls -l ${location}/${STOKES}/RSP0/beam_0/${PULSAR}_${OBSID}_RSP0.sub0000 | awk '{print $5 / 2}' -`
	fi
	
	if [ $all_pproc == 0 ] && [ $rfi_pproc == 0 ]
	then
		#Create .sub.inf files with par2inf.py
		echo cp $PARSET ./${OBSID}.parset >> $log
		cp $PARSET ./${OBSID}.parset
		echo cp ${LOFARSOFT}/release/share/pulsar/data/lofar_default.inf default.inf >> $log
		cp ${LOFARSOFT}/release/share/pulsar/data/lofar_default.inf default.inf
		#python ${LOFARSOFT}/release/share/pulsar/bin/par2inf.py -S ${PULSAR} -o test -N ${NSAMPL} -n `echo $all_num 248 | awk '{print $1 / $2}'` -r $core ./${OBSID}.parset
		echo "Running par2inf" 
		echo "Running par2inf" >> $log
		
	    echo python ${LOFARSOFT}/release/share/pulsar/bin/par2inf.py -S ${PULSAR} -o test -N ${NSAMPL} -n `echo $all_num $core | awk '{print $1 / $2}'` -r $core ./${OBSID}.parset >> $log
	    python ${LOFARSOFT}/release/share/pulsar/bin/par2inf.py -S ${PULSAR} -o test -N ${NSAMPL} -n `echo $all_num $core | awk '{print $1 / $2}'` -r $core ./${OBSID}.parset
	    status=$?
	
		if [ $status -ne 0 ]
		then
		   echo "ERROR: Unable to successfully run par2inf task"
		   echo "ERROR: Unable to successfully run par2inf task" >> $log
		   exit 1
		fi
		
		jj=0    
	    if (( $flyseye == 0 ))
	    then
			for ii in `ls -1 test*.inf | awk -F\. '{print $0,substr($1,5,10)}' | sort -k 2 -n | awk '{print $1}'`
			do
			   echo mv ${ii} ${STOKES}/RSP${jj}/${PULSAR}_${OBSID}_RSP${jj}.sub.inf >> $log
			   mv ${ii} ${STOKES}/RSP${jj}/${PULSAR}_${OBSID}_RSP${jj}.sub.inf
			   jj=`expr $jj + 1`
			done
		else
		    for jjj in $beams
		    do
		        jj=0    
			    for ii in `ls -1 test*.inf | awk -F\. '{print $0,substr($1,5,10)}' | sort -k 2 -n | awk '{print $1}'`
			    do
			       #cp ${ii} ${STOKES}/${jjj}/RSP${jj}/${PULSAR}_${OBSID}_RSP${jj}.sub.inf
			       echo cp ${ii} ${STOKES}/RSP${jj}/${jjj}/${PULSAR}_${OBSID}_RSP${jj}.sub.inf >> $log
			       cp ${ii} ${STOKES}/RSP${jj}/${jjj}/${PULSAR}_${OBSID}_RSP${jj}.sub.inf
			       jj=`expr $jj + 1`
			    done
		    done
		fi 
		
	    if (( $flyseye == 1 ))
	    then
	       echo rm test*.inf >> $log
	       rm test*.inf
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
		     mkdir -p ${STOKES}/"RSPA"
		  fi
	  else
		  for jjj in $beams
		  do
  	        if [ -d ${STOKES}/$jjj/RSPA ]
	        then 
	           echo "${STOKES}/$jjj/RSPA already exist;  deleting contents in order to process all subbands correctly"
	           echo "${STOKES}/$jjj/RSPA already exist;  deleting contents in order to process all subbands correctly" >> $log
	           rm -rf ${STOKES}/$jjj/RSPA/*
	        else
		       mkdir -p ${STOKES}/$jjj/RSPA
		    fi
		  done
	  fi
    fi # end if [ $all == 1 ] || [ $all_pproc == 1 ]
		
	#Create the .sub.inf file for the entire set (in background, as there is plenty of time to finish before file is needed
	if (( (( $all == 1 )) || (( $all_pproc == 1 )) )) && (( $flyseye == 0 ))
	then 
	     echo python ${LOFARSOFT}/release/share/pulsar/bin/par2inf.py -S ${PULSAR} -o test -N ${NSAMPL} -n $all_num -r 1 ./${OBSID}.parset >> $log
	     python ${LOFARSOFT}/release/share/pulsar/bin/par2inf.py -S ${PULSAR} -o test -N ${NSAMPL} -n $all_num -r 1 ./${OBSID}.parset
	     echo mv `ls test*.inf` ${STOKES}/RSPA/${PULSAR}_${OBSID}_RSPA.sub.inf >> $log
	     mv `ls test*.inf` ${STOKES}/RSPA/${PULSAR}_${OBSID}_RSPA.sub.inf
	elif (( (( $all == 1 )) || (( $all_pproc == 1 )) )) && (( $flyseye == 1 ))
    then
    	for jjj in $beams
	    do
	       echo python ${LOFARSOFT}/release/share/pulsar/bin/par2inf.py -S ${PULSAR} -o test -N ${NSAMPL} -n $all_num -r 1 ./${OBSID}.parset >> $log
	       python ${LOFARSOFT}/release/share/pulsar/bin/par2inf.py -S ${PULSAR} -o test -N ${NSAMPL} -n $all_num -r 1 ./${OBSID}.parset  
	       echo mv `ls test*.inf` ${STOKES}/${jjj}/RSPA/${PULSAR}_${OBSID}_RSPA.sub.inf >> $log
	       mv `ls test*.inf` ${STOKES}/${jjj}/RSPA/${PULSAR}_${OBSID}_RSPA.sub.inf
        done
	fi
	
	#Check when all 8 DONE files are available, then all processes have exited
	#ii=1
	#yy=0
	#while [ $ii -ne $yy ]
	#do
	#  if [ -e $done_list ]
	#  then
	#     echo "All bf2presto tasks have completed!" 
	#     yy=1
	#     sleep 5
	#  fi
	#  sleep 10
	#done

	#Split the bf2presto results within the beams into RSP in Fly's Eye Mode
#	if (( $flyseye == 1 ))
#	then	
#	    echo cd ${location}/${STOKES} >> $log
#	    cd ${location}/${STOKES}
#		for ii in `ls -d beam_?`
#		do
#			A=`ls $ii/${PULSAR}_${OBSID}.sub???? | grep "sub" -c`
#			B=`echo "($A/$core)-1" | bc`
#			for iii in $num_dir
#			do
				#mkdir -p ${ii}/RSP${iii}
#				echo mv `ls $ii/${PULSAR}_${OBSID}.sub???? | head -1` ${ii}/RSP${iii}/${PULSAR}_${OBSID}.sub0000 >> $log
#				mv `ls $ii/${PULSAR}_${OBSID}.sub???? | head -1` ${ii}/RSP${iii}/${PULSAR}_${OBSID}.sub0000
#				echo mv `ls $ii/${PULSAR}_${OBSID}.sub???? | head -$B` ${ii}/RSP${iii} >> $log
#				mv `ls $ii/${PULSAR}_${OBSID}.sub???? | head -$B` ${ii}/RSP${iii}
#			done
#		done
#	    echo cd ${location} >> $log
#	    cd ${location}	
#	fi # end if (( $flyseye == 1 ))
	
	#Make a master subband location with all subbands (run in the background, while other tasks are being done)
	#This is done using links to the bf2presto output files, in the RSPA directory, changing the subband order
	#so that the total order is from 0 to total number of subbands
#	if (( $flyseye == 0 ))
#	then


    if [ $all_pproc == 0 ] && [ $rfi_pproc == 0 ]
    then
	    # Move the RSP?/beam_? directory structure output from bf2presto into beam_?/RSP? structure
		if (( $flyseye == 1 ))
		then
		    for jjj in $beams
		    do
		        mkdir -p ${STOKES}/${jjj}
		        echo mkdir -p ${STOKES}/${jjj} >> $log
		        for ii in $num_dir
		        do
		           mkdir ${STOKES}/${jjj}/RSP${ii}
		           echo mkdir ${STOKES}/${jjj}/RSP${ii} >> $log
		           mv ${STOKES}/RSP${ii}/${jjj}/* ${STOKES}/${jjj}/RSP${ii}/
		           echo mv ${STOKES}/RSP${ii}/${jjj}/* ${STOKES}/${jjj}/RSP${ii}/ >> $log
		           cp ${STOKES}/RSP${ii}/bf2presto* ${STOKES}/RSP${ii}/*list ${STOKES}/${jjj}/
		           echo cp ${STOKES}/RSP${ii}/bf2presto* ${STOKES}/RSP${ii}/*list ${STOKES}/${jjj}/ >> $log
		        done
	        done
	        rm -rf ${STOKES}/RSP[0-7]
	    fi
    fi # end if [ $all_pproc == 0 ] && [ $rfi_pproc == 0 ]

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
		        echo "Warning - possible problem running on ALL subbands in $STOKES/$jjj/RSPA;  master list is too short (is $check_number but should be $total rows)"
		        echo "Warning - possible problem running on ALL subbands in $STOKES/$jjj/RSPA;  master list is too short (is $check_number but should be $total rows)" >> $log
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
	     else
	        for jjj in $beams
	        do
	           echo cd ${location}/${STOKES}/${jjj}/"RSPA" >> $log
	           cd ${location}/${STOKES}/${jjj}/"RSPA"
	           echo "#!/bin/sh" > run.sh
	           ls ../RSP[0-7]/*sub[0-9]??? | sed 's/\// /g' | awk '{print $3}' | sed 's/RSP/ RSP /' | sed 's/ RSP/RSP/g' | sed 's/\.sub/ /' | awk -v offset=$offset '{printf("ln -s ../RSP%d/%s%d.sub%04d %sA.sub%04d\n"),$2,$1,$2,$3,$1,$2*offset+$3}' >> run.sh
			   echo "Performing subband linking for all RPSs in one location for $STOKES/$jjj/RSPA"
			   echo "Performing subband linking for all RPSs in one location for $STOKES/$jjj/RSPA" >> $log
			   echo chmod 777 run.sh >> $log
			   chmod 777 run.sh
	  	       check_number=`wc -l run.sh | awk '{print $1}'`
	           total=$(( $all_num * $CHAN + 1 ))
			   if [ $check_number -ne $total ]
			   then
			        all=0
			        echo "Warning - possible problem running on ALL subbands in $STOKES/$jjj/RSPA;  master list is too short (is $check_number but should be $total rows)"
			        echo "Warning - possible problem running on ALL subbands in $STOKES/$jjj/RSPA;  master list is too short (is $check_number but should be $total rows)" >> $log
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
	        done
	     fi
#	      if [ $status -ne 0 ]
#	      then
#	         echo "WARNING: Unable to successfully run creation of link file list for ALL subbands"
#	         echo "         Skipping the ALL processing"
#	         echo "WARNING: Unable to successfully run creation of link file list for ALL subbands"  >> $log
#	         echo "         Skipping the ALL processing" >> $log
#	         all=0
#	         break
#	     fi
	
	fi # end if [ $all == 1 ] || [ $all_pproc == 1 ]
#    fi # end if (( $flyseye == 0 ))
	
	if [ $all_pproc == 0 ] && [ $rfi_pproc == 0 ]
	then
		
		# Fold data per requested Pulsar
		for fold_pulsar in $PULSAR_LIST
		do
			echo "Starting folding for RSP-splits for Pulsar $fold_pulsar"
			echo "Starting folding for RSP-splits for Pulsar $fold_pulsar" >> $log
			date
			date >> $log

		    if (( $flyseye == 0 ))
		    then
				for ii in $num_dir
			    do
				   cd ${location}/${STOKES}/RSP${ii}
				   echo cd ${location}/${STOKES}/RSP${ii} >> $log
				   echo prepfold -noxwin -psr ${fold_pulsar} -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR}_${OBSID}_RSP${ii}.sub[0-9]* >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout 
		#		   prepfold -noxwin -psr ${fold_pulsar} -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR}_${OBSID}_RSP${ii}.sub[0-9]* >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout 2>&1 && touch "DONE" >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout 2>&1 &
				   prepfold -noxwin -psr ${fold_pulsar} -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR}_${OBSID}_RSP${ii}.sub[0-9]* >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout 2>&1 &
				   prepfold_pid[$ii]=$!  
				   echo "Running: " prepfold -noxwin -psr ${fold_pulsar} -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR}_${OBSID}_RSP${ii}.sub[0-9]* >> $log
				   sleep 5
			    done
			else
			    for jjj in $beams
			    do
					for ii in $num_dir
				    do
					   cd ${location}/${STOKES}/${jjj}/RSP${ii}
					   echo cd ${location}/${STOKES}/${jjj}/RSP${ii} >> $log
					   echo prepfold -noxwin -psr ${fold_pulsar} -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR}_${OBSID}_RSP${ii}.sub[0-9]* >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout 
		#			   prepfold -noxwin -psr ${fold_pulsar} -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR}_${OBSID}.sub[0-9]* >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout 2>&1 && touch "DONE" >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout 2>&1 &
					   prepfold -noxwin -psr ${fold_pulsar} -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR}_${OBSID}_RSP${ii}.sub[0-9]* >> ${fold_pulsar}_${OBSID}_RSP${ii}.prepout 2>&1 &
					   kk=`echo "$ii * $jjj" | bc`
				       prepfold_pid[$kk]=$!  
					   echo "Running: " prepfold -noxwin -psr ${fold_pulsar} -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSP${ii} ${PULSAR}_${OBSID}_RSP${ii}.sub[0-9]* >> $log
					   sleep 5
				    done
			    done
			fi # end if (( $flyseye == 0 ))
			
			echo cd ${location} >> $log
			cd ${location}
			
			#Check when all DONE files are available, then all processes have exited
		    if (( $flyseye == 0 ))
		    then
			   for ii in $num_dir
			   do
			      echo "Waiting for RSP$ii prepfold to finish"
			      wait ${prepfold_pid[ii]}
			   done
			else
			    for jjj in $beams
			    do
					for ii in $num_dir
				    do
					   kk=`echo "$ii * $jjj" | bc`
			           echo "Waiting for RSP$ii $jjj prepfold to finish"
			           wait ${prepfold_pid[kk]}
		            done	
			    done
			fi
		done # finished loop over PULSAR_LIST
    fi # end if [ $all_pproc == 0 ] && [ $rfi_pproc == 0 ]
    
#    if (( $flyseye == 0 ))
#    then
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
		         echo prepfold -noxwin -psr ${fold_pulsar} -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSPA ${PULSAR}_${OBSID}_RSPA.sub[0-9]* >> ${fold_pulsar}_${OBSID}_RSPA.prepout 
		         prepfold -noxwin -psr ${fold_pulsar} -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSPA ${PULSAR}_${OBSID}_RSPA.sub[0-9]* >> ${fold_pulsar}_${OBSID}_RSPA.prepout 2>&1 && touch "DONE" >> ${fold_pulsar}_${OBSID}_RSPA.prepout 2>&1 &
		         index=$index
			     prepfold_pid_all[$index]=$!  
		         echo "Running: " prepfold -noxwin -psr ${fold_pulsar} -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSPA ${PULSAR}_${OBSID}_RSPA.sub[0-9]* >> $log
		         cd ${location}
		         sleep 5
		      else
			     for jjj in $beams
			     do
			         cd ${location}/${STOKES}/${jjj}/RSPA
			         echo cd ${location}/${STOKES}/${jjj}/RSPA >> $log
			         echo prepfold -noxwin -psr ${fold_pulsar} -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSPA ${PULSAR}_${OBSID}_RSPA.sub[0-9]* >> ${fold_pulsar}_${OBSID}_RSPA.prepout 
			         prepfold -noxwin -psr ${fold_pulsar} -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSPA ${PULSAR}_${OBSID}_RSPA.sub[0-9]* >> ${fold_pulsar}_${OBSID}_RSPA.prepout 2>&1 && touch "DONE" >> ${fold_pulsar}_${OBSID}_RSPA.prepout 2>&1 &
				     prepfold_pid_all[$beam_index]=$!  
			         (( beam_index = $beam_index + 1 )) 
			         echo "Running: " prepfold -noxwin -psr ${fold_pulsar} -n 256 -fine -nopdsearch -o ${fold_pulsar}_${OBSID}_RSPA ${PULSAR}_${OBSID}_RSPA.sub[0-9]* >> $log
			         cd ${location}
			         sleep 5
			     done
			  fi # end if (( $flyseye == 0 ))
			  (( index = $index + 1 ))
		  done # end for fold_pulsar in $PULSAR_LIST
	   fi # end if [ $all == 1 ] || [ $all_pproc == 1 ]
#	fi 

		
	if [ $all_pproc == 0 ] && [ $rfi_pproc == 0 ]
	then
		#Make a cumulative plot of the profiles
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
	    echo cd ${location} >> $log
	    cd ${location}
		
		#Make a .pdf and .png version of the plots
		echo "Running convert on ps to pdf and png of the plots"
		echo "Running convert on ps to pdf and png of the plots" >> $log
		date
		date >> $log

	    index=1
		beam_index=1
		for fold_pulsar in $PULSAR_LIST
		do	
		    if (( $flyseye == 0 ))
		    then
				for ii in $num_dir
				do
				   echo convert ${STOKES}/RSP${ii}/${fold_pulsar}_${OBSID}_RSP${ii}_PSR_${fold_pulsar}.pfd.ps ${STOKES}/RSP${ii}/${fold_pulsar}_${OBSID}_RSP${ii}_PSR_${fold_pulsar}.pfd.pdf >> $log
				   convert ${STOKES}/RSP${ii}/${fold_pulsar}_${OBSID}_RSP${ii}_PSR_${fold_pulsar}.pfd.ps ${STOKES}/RSP${ii}/${fold_pulsar}_${OBSID}_RSP${ii}_PSR_${fold_pulsar}.pfd.pdf
				   echo convert -rotate 90 ${STOKES}/RSP${ii}/${fold_pulsar}_${OBSID}_RSP${ii}_PSR_${fold_pulsar}.pfd.ps ${STOKES}/RSP${ii}/${fold_pulsar}_${OBSID}_RSP${ii}_PSR_${fold_pulsar}.pfd.png >> $log
				   convert -rotate 90 ${STOKES}/RSP${ii}/${fold_pulsar}_${OBSID}_RSP${ii}_PSR_${fold_pulsar}.pfd.ps ${STOKES}/RSP${ii}/${fold_pulsar}_${OBSID}_RSP${ii}_PSR_${fold_pulsar}.pfd.png
				   echo convert -rotate 90 -crop 200x140-0 ${STOKES}/RSP${ii}/${fold_pulsar}_${OBSID}_RSP${ii}_PSR_${fold_pulsar}.pfd.ps ${STOKES}/RSP${ii}/${fold_pulsar}_${OBSID}_RSP${ii}_PSR_${fold_pulsar}.pfd.th.png >> $log
				   convert -rotate 90 -crop 200x140-0 ${STOKES}/RSP${ii}/${fold_pulsar}_${OBSID}_RSP${ii}_PSR_${fold_pulsar}.pfd.ps ${STOKES}/RSP${ii}/${fold_pulsar}_${OBSID}_RSP${ii}_PSR_${fold_pulsar}.pfd.th.png
				done
			else
			    for jjj in $beams
			    do
					for ii in $num_dir
					do
					   echo convert ${STOKES}/${jjj}/RSP${ii}/${fold_pulsar}_${OBSID}_RSP${ii}_PSR_${fold_pulsar}.pfd.ps ${STOKES}/${jjj}/RSP${ii}/${fold_pulsar}_${OBSID}_RSP${ii}_PSR_${fold_pulsar}.pfd.pdf >> $log
					   convert ${STOKES}/${jjj}/RSP${ii}/${fold_pulsar}_${OBSID}_RSP${ii}_PSR_${fold_pulsar}.pfd.ps ${STOKES}/${jjj}/RSP${ii}/${fold_pulsar}_${OBSID}_RSP${ii}_PSR_${fold_pulsar}.pfd.pdf
					   echo convert -rotate 90 ${STOKES}/${jjj}/RSP${ii}/${fold_pulsar}_${OBSID}_RSP${ii}_PSR_${fold_pulsar}.pfd.ps ${STOKES}/${jjj}/RSP${ii}/${fold_pulsar}_${OBSID}_RSP${ii}_PSR_${fold_pulsar}.pfd.png >> $log
					   convert -rotate 90 ${STOKES}/${jjj}/RSP${ii}/${fold_pulsar}_${OBSID}_RSP${ii}_PSR_${fold_pulsar}.pfd.ps ${STOKES}/${jjj}/RSP${ii}/${fold_pulsar}_${OBSID}_RSP${ii}_PSR_${fold_pulsar}.pfd.png
					   echo convert -rotate 90 -crop 200x140-0 ${STOKES}/${jjj}/RSP${ii}/${fold_pulsar}_${OBSID}_RSP${ii}_PSR_${fold_pulsar}.pfd.ps ${STOKES}/${jjj}/RSP${ii}/${fold_pulsar}_${OBSID}_RSP${ii}_PSR_${fold_pulsar}.pfd.th.png >> $log
					   convert -rotate 90 -crop 200x140-0 ${STOKES}/${jjj}/RSP${ii}/${fold_pulsar}_${OBSID}_RSP${ii}_PSR_${fold_pulsar}.pfd.ps ${STOKES}/${jjj}/RSP${ii}/${fold_pulsar}_${OBSID}_RSP${ii}_PSR_${fold_pulsar}.pfd.th.png
					done
				done
			fi
	    done # end for fold_pulsar in $PULSAR_LIST
	fi # end if [ $all_pproc == 0 ] && [ $rfi_pproc == 0 ]
		
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
	         echo python ${LOFARSOFT}/release/share/pulsar/bin/subdyn.py --saveonly -n `echo ${SAMPLES}*10 | bc` *.sub0???  >> $log
	         python ${LOFARSOFT}/release/share/pulsar/bin/subdyn.py --saveonly -n `echo ${SAMPLES}*10 | bc` *.sub0??? &
	         subdyn_pid[$ii]=$!
	      else
			 for jjj in $beams
			 do
		         echo cd ${location}/${STOKES}/${jjj}/RSP${ii} >> $log
		         cd ${location}/${STOKES}/${jjj}/RSP${ii}
		         echo python ${LOFARSOFT}/release/share/pulsar/bin/subdyn.py --saveonly -n `echo ${SAMPLES}*10 | bc` *.sub0???  >> $log
		         python ${LOFARSOFT}/release/share/pulsar/bin/subdyn.py --saveonly -n `echo ${SAMPLES}*10 | bc` *.sub0??? &
		         subdyn_pid[$ii][$jjj]=$!	
	         done      
          fi
	   done
	
	   echo cd ${location} >> $log
	   cd ${location}
	
	   #Check when all 8 DONE files are available, then all processes have exited
	   if (( $flyseye == 0 ))
	   then
	      for ii in $num_dir
	      do
	         echo "Waiting for RSP$ii subdyn to finish"
	         wait ${subdyn_pid[ii]}
	      done
	   else
	      for ii in $num_dir
	      do
			 for jjj in $beams
			 do
		         echo "Waiting for RSP$ii and $jjj subdyn to finish"
	             wait ${subdyn_pid_[ii][jjj]}
	         done
	      done
       fi	   
	fi # end if [ $rfi == 1 ] || [ $rfi_pproc == 1 ] 
	
	#Gather the RFI RSPN/*rfiprep files into one summary RFI file
    if [ $rfi == 1 ] || [ $rfi_pproc == 1 ]
	then 
	   echo "Creating RFI Summary files from RSP-split results."
	   echo "Creating RFI Summary files from RSP-split results." >> $log

       max_num=$(( (( $all_num * $CHAN )) - 1 ))

	   if (( $flyseye == 0 ))
	   then
          rfi_file=$location/${STOKES}/${PULSAR}_${OBSID}_sub0-${max_num}.rfirep
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
	   	  for jjj in $beams
		  do
              rfi_file=$location/${STOKES}/${jjj}/${PULSAR}_${OBSID}_sub0-${max_num}.rfirep
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
		         cat $location/${STOKES}/${jjj}/RSP${ii}/*rfirep | grep -v "#" | awk -v offset=$offset '{printf("%d \t\t %f\n"),$1+offset, $2}' >> $rfi_file
		      done

	      done
	   fi
	fi # end if [ $rfi == 1 ] || [ $rfi_pproc == 1 ]
		
	#Wait for the all prepfold to finish
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
		   
#			   while [ $ii -ne $yy ]
#			   do
#			      if [ -e ${STOKES}/RSPA/DONE ]
#			      then
#			         echo "prepfold on the total list has completed!" 
#			         echo "prepfold on the total list has completed!" >> $log
#			         date
#			         date >> $log
#			         yy=1
#			      fi
#			      sleep 15
#			   done
			   echo convert ${STOKES}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.ps ${STOKES}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.pdf >> $log
			   convert ${STOKES}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.ps ${STOKES}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.pdf
			   echo convert -rotate 90 ${STOKES}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.ps ${STOKES}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.png >> $log
			   convert -rotate 90 ${STOKES}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.ps ${STOKES}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.png
			   echo convert -rotate 90 -crop 200x140-0 ${STOKES}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.ps ${STOKES}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.th.png >> $log
			   convert -rotate 90 -crop 200x140-0 ${STOKES}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.ps ${STOKES}/RSPA/${fold_pulsar}_${OBSID}_RSPA_PSR_${fold_pulsar}.pfd.th.png
		   else
#			   while [ $ii -ne $yy ]
#			   do
#			      if [ -e ${STOKES}/${last_beam}/RSPA/DONE ]
#			      then
#			         echo "prepfold on the total list has completed!" 
#			         echo "prepfold on the total list has completed!" >> $log
#			         date
#			         date >> $log
#			         yy=1
#			      fi
#			      sleep 15
#			   done
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
				   (( beam_index = $beam_index + 1 ))
	           done
	       fi # end if (( $flyseye == 0 ))
	       (( index = $index + 1 ))
       done # end for fold_pulsar in $PULSAR_LIST
	fi # end if [ $all -eq 1 ] || [ $all_pproc == 1 ]

	#Rename the beam_? to their actual names based on the observation parset names
    if [ $flyseye == 1 ] && [ $all_pproc == 0 ] && [ $rfi_pproc == 0 ] 
    then
        cd ${location}/${STOKES}/
		for jj in $beams
		do
			N=`echo $jj | awk -F "_" '{print $2}'`
			N=`echo "$N+1" | bc`
			NAME=`cat $PARSET| grep "OLAP.storageStationNames" | awk -F '[' '{print $2}' | awk -F ']' '{print $1}'| awk -F "," '{print $'$N'}'`
			echo "mv $jj $NAME" >> $log
			mv $jj $NAME
		done
		cd ${location}	
    fi
 
done # for loop over modes in $mode_str 
	
#Make a tarball of all the plots
echo "Creating tar file of plots"
echo "Creating tar file of plots" >> $log
date
date >> $log
if [ $flyseye_tar == 0 ] 
then
   if [ $rfi == 1 ]  || [ $rfi_pproc == 1 ]
   then
      tar_list="*/*profiles.pdf */RSP*/*pfd.ps */RSP*/*pfd.pdf */RSP*/*pfd.png */RSP*/*pfd.th.png */RSP*/*pfd.bestprof */RSP*/*.sub.inf */*.rfirep"
   else
      tar_list="*/*profiles.pdf */RSP*/*pfd.ps */RSP*/*pfd.pdf */RSP*/*pfd.png */RSP*/*pfd.th.png */RSP*/*pfd.bestprof */RSP*/*.sub.inf"
   fi
elif [ $flyseye_tar == 1 ] 
then  
   if [ $rfi == 1 ]  || [ $rfi_pproc == 1 ]
   then
      tar_list="*/*/*profiles.pdf */*/RSP*/*pfd.ps */*/RSP*/*pfd.pdf */*/RSP*/*pfd.png */*/RSP*/*pfd.th.png */*/RSP*/*pfd.bestprof */*/RSP*/*.sub.inf */*/*.rfirep"
   else
      tar_list="*/*/*profiles.pdf */*/RSP*/*pfd.ps */*/RSP*/*pfd.pdf */*/RSP*/*pfd.png */*/RSP*/*pfd.th.png */*/RSP*/*pfd.bestprof */*/RSP*/*.sub.inf"
   fi
fi
echo 'tar cvzf ${PULSAR}_${OBSID}_plots.tar.gz  $tar_list' >> $log
tar cvzf ${PULSAR}_${OBSID}_plots.tar.gz $tar_list

#echo gzip ${PULSAR}_${OBSID}_plots.tar >> $log
#gzip ${PULSAR}_${OBSID}_plots.tar
cd ${location}

#Change permissions and move files
echo "Changing permissions of files"
echo "Changing permissions of files" >> $log
date
date >> $log
echo chmod 774 -R . * >> $log
chmod 774 -R . * 
echo chgrp -R pulsar . * >> $log
chgrp -R pulsar . * 
	
date_end=`date`
echo "Start Time: " $date_start
echo "Start Time: " $date_start >> $log
echo "End Time: " $date_end
echo "End Time: $date_end" >> $log

echo "Results output location:" $location
exit 0
