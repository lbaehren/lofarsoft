#!/bin/ksh 

# Please update the version number when you edit this file:
VERSION=2.1

# take a list of observations, and create multiple templates for MOM upload (Imaging ONLY)
# required input: list of object names or ra/dec positions
# output is an XML file which can be uploaded into MOM (if you have ADMIN privileges)

USAGE1="\nUsage for Imaging: $0 [[-help IM]] \n"\
"       -in observation_list_file -inswitch IM -intype source_or_position \n"\
"       -out template_output_file -project project_name [-stations stations_list]   \n"\
"       [-start obs_start] [-time duration] [-gap duration] [-lst|-LST] \n"\
"       [-subsHBAHigh subband_range] [-subsHBAMid subband_range] [-subsHBALow subband_range] \n"\
"       [-subsLBAHigh subband_range] [-subsLBALow subband_range]  \n"\
"       [-integHBA integration_interval_HBA] [-integLBA integration_interval_LBA]  \n"\
"       [-antenna antenna_setup]  [-modeHBA antenna_submode] [-modeLBA antenna_submode] \n"\
"       [-chansubsHBA channels_per_subband_HBA] [-chansubsLBA channels_per_subband_LBA] \n"\
"       [+multi] [+IS|+CS|+FD|+BF list_or_ALL] [-namecol] [-sexages] [-clock 200|160] [-debug] \n"\
"       [-cat user_catalog_filename] [-integstepsHBA BF_integration_steps_HBA] [-integstepsLBA BF_integration_steps_LBA] \n"

USAGE2="\nUsage for BeamFormed: $0 [[-help BF]] \n"\
"       -in observation_list_file -inswitch BF -intype source_or_position \n"\
"       -out template_output_file -project project_name [-st stations_list]   \n"\
"       [-start obs_start] [-time duration] [-gap duration] [-lst|-LST] [-IS list_or_ALL]  \n"\
"       [+IS|+CS|+FD|+BF list_or_ALL] [-subsHBA subband_range] [-subsLBA subband_range] \n"\
"       [-subsHBAHigh subband_range] [-subsHBAMid subband_range] [-subsHBALow subband_range] \n"\
"       [-subsLBAHigh subband_range] [-subsLBALow subband_range]  \n"\
"       [-integHBA integration_interval_HBA] [-integLBA integration_interval_LBA]  \n"\
"       [-antenna antenna_setup]  [-modeHBA antenna_submode] [-modeLBA antenna_submode] [+multi] \n"\
"       [+IM list_or_ALL] [-chansubsHBA channels_per_subband_HBA] [-chansubsLBA channels_per_subband_LBA] \n"\
"       [-integstepsHBA integration_steps_HBA] [-integstepsLBA integration_steps_LBA] [-namecol] [-debug]"\
"       [-sexages] [-clock 200|160] [-nof_rings num_of_TA_rings] [-ring_size TA_ring_size]\n"\
"       [-cat user_catalog_filename] \n"

USAGE3="Options: \n"\
"         -in observation_list_file ==> Specify the ascii file with observation listing (i.e. in.txt) \n"\
"         -inswitch BF_or_IM   ==>  Switch between 'BF' (Beam-Formed) or 'IM' (IMAGING) type of default obs \n"\
"         -intype source_or_position ==> Input file contains 'source'-based or 'position'-based input\n"\
"         -project project_name   ==> Name of the MOM project (usually 'Pulsars' or 'MSSS')\n"\
"         -out template_output_file ==> Specify the name of the output XML template file (i.e. out.xml) \n"         
USAGE4="         [[+multi]] ==> Turns on the multi-beam input specification;  otherwise beam separator is ignored in input file.\n"\
"         [[-LST ]]  ==> This flags the input start time (command line or file) as LST, otherwise UT is assumed.\n"\
"         [[-stations stations_list]] ==> Comma separated list of stations (i.e. CS001,CS002,RS405) \n"\
"         [[-antenna HBA_or_LBA]] ==> The antenna name: HBA, HBAHigh, HBAMid, HBALow or LBA, LBALow, LBAHigh (default = HBALow (BF); HBAHigh (IM)) \n"\
"         [[-subsHBA subband_range]] ==> The BF HBA (Low) default subband range (default = '200..443') \n"\
"         [[-subsHBALow subband_range]] ==> The HBALow subband range (default = BF:'200..443'  IM:'54..297') \n"\
"         [[-subsHBAMid subband_range]] ==> The HBAMid subband range, valid for 160MHz clock only (default = BF:'140..383'  IM:'66..309') \n"\
"         [[-subsHBAHigh subband_range]] ==> The HBAHigh subband range (default = BF:'77..320'  IM:'52..255') \n"\
"         [[-subsLBA subband_range]] ==> The BF LBA (Low), IM LBA (High) default subband range (default = BF:'100..343'  IM:'156..399') \n"\
"         [[-subsLBALow subband_range]] ==> The LBALow subband range (default = BF:'100..343'  IM:'54..297') \n"\
"         [[-subsLBAHigh subband_range]] ==> The LBAHigh subband range (default = BF:'154..397'  IM:'156..399') \n"\
"         [[-chansubsHBA channels_per_subband_HBA]] ==> The channels per subband for HBA (default = 16 (BF), 64 (IM)) \n"\
"         [[-chansubsLBA channels_per_subband_LBA]] ==> The channels per subband for LBA (default = 16 (BF), 64 (IM)) \n"\
"         [[-integstepsHBA integration_steps_HBA]] ==> The BF integration steps for HBA (default = 16) \n"\
"         [[-integstepsLBA integration_steps_LBA]] ==> The BF integration steps for LBA (default = 16) \n"\
"         [[-integHBA integration_interval_HBA]] ==> The IM-obs integration interval for HBA (default = 2) \n"\
"         [[-integLBA integration_interval_LBA]] ==> The IM-obs integration interval for LBA (default = 3) \n"\
"         [[-gap duration]] ==> The time between ALL observations in minutes (default = 3) \n"\
"         [[-IS list_or_ALL]] ==> Turn OFF incoherentStokesData [set to ON by default];  'ALL' or row-number-list '2,4,5' (rows start at #1);  must use +CS|+FD|+BF switches when IS is turned off for all rows \n"\
"         [[+IM list_or_ALL]] ==> Turn on Imaging (correlatedData) with BF observations;  'ALL' or row-number-list '2,4,5' (rows start at #1)\n"\
"         [[+IS list_or_ALL]] ==> Turn on incoherentStokesData [Standard] with Imaging observations;  'ALL' or row-number-list '2,4,5' (rows start at #1)\n"\
"         [[+CS list_or_ALL]] ==> Turn on coherentStokesData with Imaging observations;  'ALL' or row-number-list '2,4,5' (rows start at #1)\n"\
"         [[+FD list_or_ALL]] ==> Turn on filteredData with Imaging observations;  'ALL' or row-number-list '2,4,5' (rows start at #1)\n"\
"         [[+BF list_or_ALL]] ==> Turn on beamformedData with Imaging observations;  'ALL' or row-number-list '2,4,5' (rows start at #1)\n"\
"         [[-modeHBA antenna_submode]] ==> The HBA antenna sub-mode (Zero, One, Dual (default BF & IM), Joined, Dual_Inner)\n"\
"         [[-modeLBA antenna_submode]] ==> The LBA antenna sub-mode (Outer (default BF & IM), Inner, Sparse Even, Sparse Odd, X, Y)\n"\
"         [[-namecol]] ==> The first column in the input file contains an additional column which will be used as the Obs Name in MOM\n"\
"         [[-sexages]] ==> Indicator that input file contains RA & DEC columns in Sexagesimal (HH:MM:SS.SSSS [+/-]DD:MM:SS.SSSSS) format (decimal degrees assumed otherwise).\n"\
"         [[-clock 200|160]] ==> Change the clock from the default value of 200 MHz to 160 MHz;  200 MHz is assumed if clock is not specified.\n"\
"         [[-nof_rings 1 | 2]] ==> Number of Tied-Array rings (generally set to 1 [7 beams] or 2 rings [19 beams]; default=0).\n"\
"         [[-ring_size TA_ring_size]] ==> Size of Tied-Array rings in radians (default=0.00872663, which is ~0.5 deg)\n"\
"         [[-debug]] ==> Turn on KSH DEBUGGING information while running the script (tons of STDOUT messages, for testing).\n"\
"         [[-cat user_catalog_filename]] ==> User specified catalog file (format: ra dec name), overrules regular catalog positions. \n"

USAGE5="For help on Imaging input format and options, use '-help IM' switch\n"\
"For help on BF (BF+IM) input format and options, use '-help BF' switch\n"


if [ $# -lt 2 ]                    
then
   print "$USAGE1" 
   print "$USAGE2" 
   print "$USAGE3" 
   print "$USAGE4" 
   print "$USAGE5" 
   exit 1
fi

##################################################
# Input parametes and switches (set some defaults)
infile=""
user_infile=0
outfile=""
user_outfile=0
INSWITCH=1
INSWITCH_STR=""
user_inswitch=0
INTYPE=1
INTYPE_STR=""
user_intype=0
SUBBANDS_HBA="200..443"
user_subbands_hba=0
SUBBANDS_HBAHigh="77..320"
user_subbands_hbahigh=0
SUBBANDS_HBAMid="140..383"
user_subbands_hbamid=0
SUBBANDS_HBALow="200..443"
user_subbands_hbalow=0
SUBBANDS_LBA="154..397"
user_subbands_lba=0
SUBBANDS_LBALow="154..397"
user_subbands_lbalow=0
SUBBANDS_LBAHigh="100..343"
user_subbands_lbahigh=0
SUBBANDS=""
SUBBANDS_SET=0
START=""
user_start=0
previous_start=""
TIME=0
user_duration=0
STATIONS=""
user_stations=0
GAP=3
user_gap=0
ANTENNA=HBAHigh
user_antenna=0
CHAN_SUBS_HBA=16
CHAN_SUBS_HBA_IM=64
user_chan_subs_hba=0
CHAN_SUBS_LBA=16
CHAN_SUBS_LBA_IM=64
user_chan_subs_lba=0
CHAN_SUBS=0
STEPS_HBA=16
user_steps_hba=0
STEPS_LBA=16
user_steps_lba=0
INTEG_HBA=2
user_integ_hba=0
INTEG_LBA=3
user_integ_lba=0
INTERVAL=1
STEPS=0
LST=0
LST_DIFF=0
PROJECT="Pulsars"
user_project=0
IM=0
IM_LIST=""
IS=0
IS_LIST=""
ISNOT=0
ISNOT_LIST=""
CS=0
CS_LIST=""
FD=0
FD_LIST=""
BF=0
BF_LIST=""
modeHBA_IM="Dual"
#modeHBA_BF="Zero"
modeHBA_BF="Dual"
user_modeHBA=0
modeLBA="Outer"
user_modeLBA=0
HELP=""
user_help=0
MULTI=0
user_multi=0
NAMECOL=0
user_namecol=0
sexages=0
debug=0
clock=200
NOF_RINGS=0
RING_SIZE=0.0
CAT=""
user_cat=0
input_string=$*

while [ $# -gt 0 ]
do
    case "$1" in
     -in)                infile=$2; user_infile=1; shift;;
     -inswitch)          INSWITCH_STR=$2; user_inswitch=1; shift;;
     -intype)            INTYPE_STR=$2; user_intype=1; shift;;
     -out)               outfile=$2; user_outfile=1; shift;;
     -start)             START=$2; previous_start=$2; user_start=1; shift;;
     -time)              TIME=$2; user_duration=1; shift;;
     -gap)               GAP=$2; user_gap=1; shift;;
     -subsHBA)           SUBBANDS_HBA=$2; user_subbands_hba=1; shift;;
     -subsLBA)           SUBBANDS_LBA=$2; user_subbands_lba=1; shift;;
     -subsLBAHigh)       SUBBANDS_LBAHigh=$2; user_subbands_lbahigh=1; shift;;
     -subsLBALow)        SUBBANDS_LBALow=$2; user_subbands_lbalow=1; shift;;
     -subsHBAHigh)       SUBBANDS_HBAHigh=$2; user_subbands_hbahigh=1; shift;;
     -subsHBAMid)        SUBBANDS_HBAMid=$2; user_subbands_hbamid=1; shift;;
     -subsHBALow)        SUBBANDS_HBALow=$2; user_subbands_hbalow=1; shift;;
     -antenna)           ANTENNA=$2; user_antenna=1; shift;;
     -modeHBA)           modeHBA_IM=$2; modeHBA_BF=$2; user_modeHBA=1; shift;;
     -modeLBA)           modeLBA=$2; user_modeLBA=1; shift;;
     -chansubsHBA)       CHAN_SUBS_HBA=$2; CHAN_SUBS_HBA_IM=$2; user_chan_subs_hba=1; shift;;
     -chansubsLBA)       CHAN_SUBS_LBA=$2; CHAN_SUBS_LBA_IM=$2; user_chan_subs_lba=1; shift;;
     -integstepsHBA)     STEPS_HBA=$2; user_steps_hba=1; shift;;
     -integstepsLBA)     STEPS_LBA=$2; user_steps_lba=1; shift;;
     -integHBA)          INTEG_HBA=$2; user_integ_hba=1; shift;;
     -integLBA)          INTEG_LBA=$2; user_integ_lba=1; shift;;
     -stations)          STATIONS=$2; user_stations=1; shift;;
     -project)           PROJECT=$2; user_project=1; shift;;
     +IM)                IM=1; IM_LIST=$2; shift;;
     +IS)                IS=1; IS_LIST=$2; shift;;
     -IS)                ISNOT=1; ISNOT_LIST=$2; shift;;
     +CS)                CS=1; CS_LIST=$2; shift;;
     +FD)                FD=1; FD_LIST=$2; shift;;
     +BF)                BF=1; BF_LIST=$2; shift;;
     -help)              HELP=$2; user_help=1; shift;;
     -lst|-LST)          LST=1;;
     +multi)             MULTI=1; user_multi=1;;
     -namecol)           NAMECOL=1; user_namecol=1;;
     -sexages)           sexages=1;;
     -debug)             debug=1;;
     -clock)             clock=$2; shift;;
     -nof_rings)         NOF_RINGS=$2; shift;;
     -ring_size)         RING_SIZE=$2; shift;;
     -cat)               user_catfile=$2; user_cat=1; shift;;
       -*)
            print >&2 \
            "$USAGE1" \
            "$USAGE2" \
            "$USAGE3" \
            "$USAGE4" \
            "$USAGE5" 
            exit 1;;
        *)  break;;     
    esac
    shift
done

##################################################
# If -help was requested, print the help and exit
if [ $user_help == 1 ]
then
   if [ $HELP == "IM" ]
   then
      echo "Printing help setting for Imaging (only) input and switches."
      echo "For detailed help on Beam-Formed input and switches, using '-help BF'."
      cat $LOFARSOFT/release/share/pulsar/data/multi-template_helpIM.txt
   else
      echo "Printing help setting for Beam-Formed (+IM) input and switches"
      echo "For detailed help on Imaging input and switches, using '-help IM'."
      cat $LOFARSOFT/release/share/pulsar/data/multi-template_helpBF.txt
   fi
   exit 1
fi

if [[ $debug == 1 ]]
then
   echo "====================Turning on ksh DEBUGGING (to STDOUT)===================="
   echo ""
   set -x
fi


#if [ $# -lt 8 ]                    
#then
#   print "$USAGE1" 
#   print "$USAGE2" 
#   print "$USAGE3" 
#   print "$USAGE4" 
#   print "$USAGE5" 
#   exit 1
#fi

##################################################
# Check required input/output settings;  exit if there are problems.
if [ $user_infile == 0 ]
then
   echo "ERROR: Input file name is required with '-in' argument."
   exit 1
fi

if [ $user_outfile == 0 ]
then
   echo "ERROR: Output file name is required with '-out' argument."
   exit 1
fi

if [ $user_project == 0 ]
then
   echo "ERROR: Project name must be specified with '-project' argument and must match exactly with MOM."
   exit 1
fi

if [ $user_intype == 0 ]
then
   echo "ERROR: Input file type must be specified with '-intype position' or '-intype source'."
   exit 1
else
   if [ $INTYPE_STR != "source" ]  && [ $INTYPE_STR != "position" ]
   then
      echo "ERROR: Input file type must be specified with '-intype position' or '-intype source'."
      exit 1
   fi
   if [ $INTYPE_STR == "source" ] 
   then
       INTYPE=1
   else
       INTYPE=2
   fi
fi

if [ $user_inswitch == 0 ]
then
   echo "ERROR: Input switch '-inswitch IM' or '-inswitch BF' must be specified, to choose Imaging or BF based settings."
   exit 1
else
   if [ $INSWITCH_STR != "IM" ]  && [ $INSWITCH_STR != "BF" ]
   then
      echo "ERROR: Input switch '-inswitch IM' or '-inswitch BF' must be specified, to choose Imaging or BF based settings."
      exit 1
   fi
   if [ $INSWITCH_STR == "BF" ] 
   then
       INSWITCH=1
   elif [ $INSWITCH_STR == "IM" ] 
   then
       INSWITCH=2
   fi
fi

if (( $clock != 200 )) && (( $clock != 160 ))
then
      echo "ERROR: Input clock setting must be either 200 or 160 (units of MHz); user setting was $clock."
      exit 1
fi

#write the version number to the screen
echo "Script V$VERSION"
echo "$0 $input_string"
echo "Run on date: $date_start" 

##################################################
# Check if input/output files exist
if [ ! -f $infile ]
then
   echo "ERROR: $infile does not exist"
   exit 1
fi

if [ -f $outfile ]
then
   echo "WARNING: $outfile exists; overwriting!"
   rm -rf $outfile
fi

if (( $user_cat == 1 ))
then
	if [ ! -f $user_catfile ]
	then
	   echo "ERROR: $user_catfile does not exist; if using '-cat filename', please specify correct input catalog"
	   exit 1
	else
	   echo "Using user specified catalog;  position must be in degrees, format MUST be:  ra dec name"
	fi
fi

# NAMECOL is generally used to push survey observations through this script.
# if the NAMECOL has been set, rework the file into two files:
#   - one with just the first col
#   - and the other with just the data
if [ $NAMECOL == 1 ]
then
   grep -v "#" $infile |  awk '{print $1}' > /tmp/$$xmltmp_namecol
   grep -v "#" $infile |  awk '{for (i=2;i<=NF;i++) printf $i" "; print '\n' }' > /tmp/$$xmltmp_datacol
   infile=/tmp/$$xmltmp_datacol
fi

if [ $MULTI == 0 ]
then
   echo "Single-beam input file specification."
   nbeams=1
else 
   is_multi=`grep -v "#" $infile | grep -v '^$' | grep ";"`
   if [[ $is_multi == "" ]]
   then
      echo "WARNING: unable to find field separtor ';' in input file therefore turning off multi-beam request"
      nbeams=1
      MULTI=0
   else
      # find the line with the most number of beams listed - will be the last in the sorted list
      nbeams=`grep -v "#" $infile | grep -v '^$' | grep ";" | awk -F";" '{print NF}' | sort | tail -1`
      echo "Multi-beam input confirmed;  found max = $nbeams beams in the input file."
   fi
fi

if (( $NOF_RINGS < 0 )) || (( $NOF_RINGS > 2 ))
then
   echo "ERROR: Number of TA rings must be 0, 1 (7 TA beams) or 2 (19 TA beams)."
   exit 1
fi

if (( $NOF_RINGS != 0 )) && (( $RING_SIZE <= 0 ))
then
   echo "WARNING: User specified number of TA Rings > 0, but TA Ring Size is not specified."
   echo "         Setting TA Ring Size to default value of 0.00872663 rad = 0.5degrees."
   RING_SIZE=0.00872663
fi

##################################################
#The 'date' command is highly dependant on the OS;  
#BSD 'date' has more functionality than Linux/Ubuntu date;
#therefore, need to make to cases for the date-oriented calculations
system_typ=`uname -s | awk '{print $1}'`

##################################################
# Find out which type of input file was given.
# Input is dependant on whether this is Imaging or BF;
# depending on the type of input, check for required input parameters
ncols=0
ncols=`grep -v "#" $infile | grep -v '^$' | tail -1 | awk -F";" '{print $1}' | wc -w | sed 's/ //g'`
echo "Your input file $infile contains $ncols columns"

# if there are at least 8 columns, then subbands are provided in the input file and command line switches are ignored
if (( $ncols >= 8 ))
then
   if (( $user_subbands_hba != 0 )) || (( $user_subbands_hbahigh != 0 )) || (( $user_subbands_hbalow != 0 )) || (( $user_subbands_hbamid != 0 ))
   then
      echo "WARNING: ignoring user-specified command line HBA subbands setting;  subbands from observation list will be used."
      user_subbands_hba=0
      user_subbands_hbahigh=0
      user_subbands_hbamid=0
      user_subbands_hbalow=0
   fi
   if (( $user_subbands_lba != 0 )) || (( $user_subbands_lbahigh != 0 )) || (( $user_subbands_lbalow != 0 )) 
   then
      echo "WARNING: ignoring user-specified command line LBA subbands setting;  subbands from observation list will be used."
      user_subbands_lba=0
      user_subbands_lbahigh=0
      user_subbands_lbalow=0
   fi
fi

# if there are at least 7 column, then the time-related information is provided in the input file;  command line switches are ignored
if (( $ncols >= 7 ))
then
   if (( $user_gap != 0 ))
   then
      echo "WARNING: ignoring user-specified gap setting;  start time from observation list will be used."
      user_gap=0
   fi
   if (( $user_start == 1 ))
   then
      echo "WARNING: ignoring user-specified start setting;  start time from observation list will be used."
      user_start=0
   fi
   if (( $user_duration == 1 ))
   then
      echo "WARNING: ignoring user-specified gap setting;  start time from observation list will be used."
      user_duration=0
   fi
   if (( $user_stations == 1 ))
   then
      echo "WARNING: ignoring user-specified stations list;  input file station list will be used."
      user_stations=0
   fi
fi

# if there are at least 4 column, then the chan per subband and integ steps are in the input file;  command line switches are ignored
if (( $ncols >= 4 ))
then
   if (( $user_chan_subs_hba == 1 ))
   then
      echo "WARNING: ignoring user-specified channels per subband HBA setting;  input file setting will be used."
      user_chan_subs_hba=0
   fi
   if (( $user_chan_subs_lba == 1 ))
   then
      echo "WARNING: ignoring user-specified channels per subband LBA setting;  input file setting will be used."
      user_chan_subs_lba=0
   fi
fi

if (( (( $ncols >= 4 )) && (( $INTYPE == 1 )) ))
then
   if (( $user_steps_hba == 1 )) || (( $user_integ_hba == 1 )) 
   then
      echo "WARNING: ignoring user-specified integration steps/interval HBA setting;  input file setting will be used."
      user_steps_hba=0
      user_integ_hba=0
   fi
   if (( $user_steps_lba == 1 )) || (( $user_integ_lba == 1 ))
   then
      echo "WARNING: ignoring user-specified integration steps/interval LBA setting;  input file setting will be used."
      user_steps_lba=0
      user_integ_lba=0
   fi
fi

if (( (( $ncols >= 2 )) && (( $INTYPE == 1 )) )) || (( (( $ncols >= 3 )) && (( $INTYPE == 2 )) ))
then
   if (( $user_antenna == 1 ))
   then
      echo "WARNING: ignoring user-specified antenna setting;  input file antenna setting will be used."
      user_antenna=0
   fi
fi

if (( $ncols == 6 )) 
then
   echo "ERROR: Your file contains $ncols columns which is not allowed (1,2,3,4,5,7,8,9 columns allowed)."
   echo "       Please refer to the script usage for input file options (type > $0) by itself."
   exit 1
fi

if (( (( $ncols <= 4 )) && (( $INTYPE == 1 )) )) || (( (( $ncols <= 5 )) && (( $INTYPE == 2 )) ))
then
   # check that the user has specified required input
   if (( $user_start == 0 )) 
   then
      echo "ERROR: $ncols column input file requires '-start obs_start' as input"
      exit 1
   elif (( $user_duration == 0 )) 
   then
      echo "ERROR: $ncols column input file requires '-time duration' as input"
      exit 1
   elif (( $user_stations == 0 )) 
   then
      echo "ERROR: $ncols column input file requires '-stations stations_list' as input"
      exit 1
   fi
fi

if [ $INSWITCH == 1 ]  # BF data
then
	if (( ($ncols == 1) && ($INTYPE == 1) )) || (( ($ncols == 2) && ($INTYPE == 2) ))
	then
	   # set the default antenna to HBALow for BF when user has not specified one
	   if [[ $user_antenna == 0 ]]
	   then
	       ANTENNA="HBALow"
	       ANT_SHORT=HBA
	       echo "NOTE: Antenna not set by the user, will be set to HBALow by default for BF data."
	   fi
	
	   # check that the user has specified optional params
	   if [[ $ANTENNA == "HBA" ]] || [[ $ANTENNA == "HBALow" ]] || [[ $ANTENNA == "HBAMid" ]] || [[ $ANTENNA == "HBAHigh" ]]
	   then
	       ANT_SHORT=HBA
	       CHAN_SUBS=$CHAN_SUBS_HBA
	       STEPS=$STEPS_HBA
	       INTERVAL=$INTEG_HBA
	   elif [[ $ANTENNA == "LBA" ]] || [[ $ANTENNA == "LBALow" ]] || [[ $ANTENNA == "LBAHigh" ]]
	   then
	       ANT_SHORT=LBA
	       CHAN_SUBS=$CHAN_SUBS_LBA
	       STEPS=$STEPS_LBA
	       INTERVAL=$INTEG_LBA
	   else 
	      echo "ERROR: Input antenna setting is not understood ($ANTENNA); must be: HBA, HBALow, HBAMid, HBAHigh or LBA, LBALow, LBAHigh"
	      exit 1
	   fi
	fi   
else  # IM data
	if (( ($ncols == 1) && ($INTYPE == 1) )) || (( ($ncols == 2) && ($INTYPE == 2) ))
	then
	
	   # set the default antenna to HBAHigh for IM when user has not specified one
	   if [[ $user_antenna == 0 ]]
	   then
	       ANTENNA="HBAHigh"
	       echo "NOTE: Antenna not set by the user, will be set to HBAHigh by default for IM data."
	   fi

	   # check that the user has specified optional params
	   if [[ $ANTENNA == "HBALow" ]] || [[ $ANTENNA == "HBAMid" ]] || [[ $ANTENNA == "HBAHigh" ]]
	   then
	       ANT_SHORT=HBA
	       CHAN_SUBS=$CHAN_SUBS_HBA_IM
	       INTERVAL=$INTEG_HBA
	       STEPS=$STEPS_HBA
	   elif [[ $ANTENNA == "LBA" ]] || [[ $ANTENNA == "LBALow" ]] || [[ $ANTENNA == "LBAHigh" ]]
	   then
	       ANT_SHORT=LBA
	       CHAN_SUBS=$CHAN_SUBS_LBA_IM
	       INTERVAL=$INTEG_LBA
	       STEPS=$STEPS_LBA
	   else
	      echo "ERROR: Input antenna setting is not understood ($ANTENNA); must be: HBALow, HBAMid, HBAHigh or LBA, LBALow, LBAHigh"
	      exit 1
	   fi
	fi 
	  
fi # end if [ $INSWITCH == 1 ]

##################################################
# If LST was set by the user, then calculate the time difference 
if (( $LST == 1 )) && (( $user_start == 1 ))
then
    LST_DIFF=120
    # change the start time from LST to UT
    #new_start=`date -j -v +$LST_DIFF"M" -f "%Y-%m-%dT%H:%M:%S" $START "+%Y-%m-%dT%H:%M:%S"`
    #echo "new start is $new_start"
    #START=$new_start
    #previous_start=$new_start
    
    tot_time=$LST_DIFF
   			   
	if [[ $system_typ == "Darwin" ]]
	then
          previous_start=`date -j -v +$tot_time"M" -f "%Y-%m-%dT%H:%M:%S" $START "+%Y-%m-%dT%H:%M:%S"`
          START=$previous_start
	else
          tmp_start=`echo "$START" | sed 's/T/ /'`
          tmp=`date -d "$tmp_start" "+%s"`
          new_date_seconds=`echo "$tmp + $tot_time * 60" | bc` 
	  #new_date_seconds=`echo "date -d \"$START\" \"+%s\" + $tot_time * 60" | bc`
          previous_start=`date -d @$new_date_seconds "+%Y-%m-%dT%H:%M:%S"`
          START=$previous_start
	fi

else 
    LST_DIFF=0
fi

##################################################
# Set up the catalogs for source -> position searches
# Set up the header, middle and beam sections depending on IM/BF
num_lines=0
num_lines=`grep -v "#" $infile | wc -l | awk '{print $1}'`
#if [ $INSWITCH == 1 ]
#then
#   catalog=$LOFARSOFT/release/share/pulsar/data/PSR_catalog.txt
#else
#   catalog=$LOFARSOFT/release/share/pulsar/data/3cCatalog_RADEC.txt
#fi
catalog=$LOFARSOFT/release/share/pulsar/data/Combined_3C_PSR_catalog.txt
echo "Working on $infile with $num_lines sources"
if (( $INTYPE == 1 ))
then
   if [ ! -f $catalog ]
   then
      echo "ERROR: Unable to find combined catalog file $catalog."
      echo "       Make sure you have run 'make install' in \$LOFARSOFT/build/pulsar"
      echo "       Or, use the Daily Build 'LUS' package."
      exit 1
   else
      echo "Using source catalog file $catalog"
   fi

   # combine user cat and script cat:
   if (( $user_cat == 1 ))
   then
      echo "Combining user catalog and standard catalog: /tmp/$$_combinedcat.txt"
      cat $user_catfile $catalog | grep -v "#" > /tmp/$$_combinedcat.txt
      catalog=/tmp/$$_combinedcat.txt
   fi
fi
echo "Writing to output file $outfile"

##################################################
# For BF processing, check if +IM was turned on;  create the image number list
if [ $IM == 1 ] && [ $INSWITCH == 1 ]
then
   if [ $IM_LIST == "ALL" ]  || [ $IM_LIST == "all" ]
   then
      # create the full range of row numbers for the Image data
      tmp=""
      ii=1
      while (( $ii <= $num_lines ))
      do
         tmp="$tmp $ii"
         ((ii += 1))
      done
      IM_LIST=$tmp
   else 
      # change the commas to spaces for the row numbers which need Image data
      tmp=`echo $IM_LIST | sed 's/,/ /g'`
      IM_LIST=$tmp
   fi
fi

##################################################
# For BF processing, check if +IS|+CS|+BF|+FD was turned on;  create the bf number list
if [ $ISNOT == 1 ] && [ $INSWITCH == 1 ]
then
   if [ $CS == 0 ]  && [ $FD == 0 ] && [ $BF == 0 ]
   then
      echo "ERROR: Incoherentstokes (-IS) is turned off, but +CS|+FD|+BF are not turned on."
      echo "       You must turn on at least one of the BF modes for observations: +IS|+CS|+FD|+BF"
      exit 1
   fi
fi
if [ $ISNOT == 0 ] && [ $IS == 0 ] && [ $INSWITCH == 1 ]
then
   IS=1
   IS_TF=true
   IS_LIST="ALL"
fi

if [ $ISNOT == 1 ] && [ $INSWITCH == 1 ]
then
   if [ $ISNOT_LIST == "ALL" ]  || [ $ISNOT_LIST == "all" ]
   then
      # create the full range of row numbers for the Image data
      tmp=""
      ii=1
      while (( $ii <= $num_lines ))
      do
         tmp="$tmp $ii"
         ((ii += 1))
      done
      ISNOT_LIST=$tmp
   else 
      # change the commas to spaces for the row numbers which need Image data
      tmp=`echo $ISNOT_LIST | sed 's/,/ /g'`
      ISNOT_LIST=$tmp
   fi
fi
if [ $IS == 1 ] 
then
   if [ $IS_LIST == "ALL" ]  || [ $IS_LIST == "all" ]
   then
      # create the full range of row numbers for the Image data
      tmp=""
      ii=1
      while (( $ii <= $num_lines ))
      do
         tmp="$tmp $ii"
         ((ii += 1))
      done
      IS_LIST=$tmp
   else 
      # change the commas to spaces for the row numbers which need Image data
      tmp=`echo $IS_LIST | sed 's/,/ /g'`
      IS_LIST=$tmp
   fi
fi
if [ $CS == 1 ] 
then
   if [ $CS_LIST == "ALL" ]  || [ $CS_LIST == "all" ]
   then
      # create the full range of row numbers for the Image data
      tmp=""
      ii=1
      while (( $ii <= $num_lines ))
      do
         tmp="$tmp $ii"
         ((ii += 1))
      done
      CS_LIST=$tmp
   else 
      # change the commas to spaces for the row numbers which need Image data
      tmp=`echo $CS_LIST | sed 's/,/ /g'`
      CS_LIST=$tmp
   fi
fi
if [ $FD == 1 ] 
then
   if [ $FD_LIST == "ALL" ]  || [ $FD_LIST == "all" ]
   then
      # create the full range of row numbers for the Image data
      tmp=""
      ii=1
      while (( $ii <= $num_lines ))
      do
         tmp="$tmp $ii"
         ((ii += 1))
      done
      FD_LIST=$tmp
   else 
      # change the commas to spaces for the row numbers which need Image data
      tmp=`echo $FD_LIST | sed 's/,/ /g'`
      FD_LIST=$tmp
   fi
fi
if [ $BF == 1 ] 
then
   if [ $BF_LIST == "ALL" ]  || [ $BF_LIST == "all" ]
   then
      # create the full range of row numbers for the Image data
      tmp=""
      ii=1
      while (( $ii <= $num_lines ))
      do
         tmp="$tmp $ii"
         ((ii += 1))
      done
      BF_LIST=$tmp
   else 
      # change the commas to spaces for the row numbers which need Image data
      tmp=`echo $BF_LIST | sed 's/,/ /g'`
      BF_LIST=$tmp
   fi
fi

#create the header of the xml output file
header=$LOFARSOFT/release/share/pulsar/data/XML-template-header.txt
if [ ! -f $header ]
then
   echo "ERROR: missing USG installation of files \$LOFARSOFT/release/share/pulsar/data/XML-template-*.txt"
   echo "       Make sure you have run 'make install' in \$LOFARSOFT/build/pulsar"
   echo "       Or, use the Daily Build 'LUS' package."
   exit 1
fi 


cat $header | sed "s/Pulsars/$PROJECT/g" > $outfile

if [ $INSWITCH == 1 ]
then
   middle=$LOFARSOFT/release/share/pulsar/data/XML-template-BFsingleobs_HBALBA_parent.txt
   beam_xml=$LOFARSOFT/release/share/pulsar/data/XML-template-BFsingleobs_HBALBA_child.txt
else
   middle=$LOFARSOFT/release/share/pulsar/data/XML-template-IMsingleobs_HBALBA_parent.txt
   beam_xml=$LOFARSOFT/release/share/pulsar/data/XML-template-IMsingleobs_HBALBA_child.txt
fi
if [ ! -f $middle ] || [ ! -f $beam ] 
then
   echo "ERROR: missing USG installation of files \$LOFARSOFT/release/share/pulsar/data/XML-template-*.txt"
   echo "       Make sure you have run 'make install' in \$LOFARSOFT/build/pulsar"
   echo "       Or, use the Daily Build 'LUS' package."
   exit 1
fi 

##################################################
# Set some basic RA/DEC variables
date=`date "+%Y-%m-%dT%H:%M:%S"`
PID=$$
pi=$(echo "scale=10; 4*a(1)" | bc -l)

RA_DEG=0.0
DEC_DEG=0.0
OBJECT=""
RA=0.0
DEC=0.0

##################################################
# Main Loop over each line in the input file:
# skip blank lines and comment lines
# for each source, create an observation
# print basic observation information to STDOUT
counter=0
skip=0
while read line
do
 line_orig=$line
 beam=1
 beam_str=""
 echo "------------------------------------------------------------------"
 if [[ $line != "" ]]
 then
   is_header=`echo $line | grep "#"`
   if [[ $is_header == "" ]]
   then
        ((counter += 1))
        if [ $MULTI == 0 ]
        then
           nbeams=1
        else
           nbeams=`echo $line_orig | awk -F";" '{print NF}'`
        fi
        
        while (( $beam <= $nbeams ))
        do 
            beam_str=`echo $line_orig | awk -F";" -v beam=$beam '{print $beam}'`
            echo "Reading input:  $beam_str"
            line=$beam_str
            
            if (( $beam > 1 ))
            then
				ncols_beam=`echo $line | awk -F" " '{print NF}'`
               # only have source/pos and possibly subband info
				    
		        if (( $INTYPE == 1))
		        then
			       OBJECT=`echo $line | awk '{print $1}'`
			       
				   if (( $ncols_beam == 2 ))
				   then
				       SUBBANDS=`echo $line | awk '{print $2}'`
				       SUBBANDS_SET=1
				   fi
			    else
			       RA_DEG=`echo $line | awk '{print $1}'`
			       DEC_DEG=`echo $line | awk '{print $2}'`
				   if (( $ncols_beam == 3 ))
				   then
				       SUBBANDS=`echo $line | awk '{print $3}'`
				       SUBBANDS_SET=1
				   fi
			    fi
			    
			    if [ $INSWITCH == 1 ] # BF
			    then				    
				    PULSAR=$OBJECT
                fi
            else # when beam == 1
		        # For BF data, check which columns are in the dataset
#			    if [ $INSWITCH == 1 ] # BF
#			    then
		        if (( $INTYPE == 1))
		        then

				    OBJECT=`echo $line | awk '{print $1}'`

                    if [ $INSWITCH == 1 ] # BF
                    then
					   PULSAR=$OBJECT
				    fi
				    
				    if (( $ncols >= 2 ))
				    then
				       ANTENNA=`echo $line | awk '{print $2}'`

                       if [[ $INSWITCH == 2 ]] # IM
                       then
						   if [[ $ANTENNA == "HBALow" ]] || [[ $ANTENNA == "HBAMid" ]] || [[ $ANTENNA == "HBAHigh" ]]
						   then
						       ANT_SHORT=HBA
						   elif [[ $ANTENNA == "LBA" ]] || [[ $ANTENNA == "LBALow" ]] || [[ $ANTENNA == "LBAHigh" ]]
						   then
						       ANT_SHORT=LBA
						   else
						      echo "ERROR: Antenna must be set to any of these values IM: HBALow, HBAMid, HBAHigh, LBA, LBALow, LBAHigh"
						      exit 1
						   fi
                       else # BF
						   if [[ $ANTENNA == "HBA" ]] || [[ $ANTENNA == "HBALow" ]] || [[ $ANTENNA == "HBAMid" ]] || [[ $ANTENNA == "HBAHigh" ]]
						   then
						       ANT_SHORT=HBA
						   elif [[ $ANTENNA == "LBA" ]] || [[ $ANTENNA == "LBALow" ]] || [[ $ANTENNA == "LBAHigh" ]]
						   then
						       ANT_SHORT=LBA
						   else
						      echo "ERROR: Antenna must be set to any of these values BF: HBA, HBALow, HBAMid, HBAHigh, LBA, LBALow, LBAHigh"
						      exit 1
						   fi
                       fi
				    fi # end if (( $ncols >= 2 ))
			
				    if (( $ncols == 2 ))
				    then
	                    if [ $INSWITCH == 1 ] # BF
	                    then
							if [[ $ANT_SHORT == "HBA" ]] 
							then 
						        CHAN_SUBS=$CHAN_SUBS_HBA
						        STEPS=$STEPS_HBA
						        INTERVAL=$INTEG_HBA
							else
						        CHAN_SUBS=$CHAN_SUBS_LBA
						        STEPS=$STEPS_LBA
						        INTERVAL=$INTEG_LBA
							fi
						else
							if [[ $ANT_SHORT == "HBA" ]]
							then 
						        CHAN_SUBS=$CHAN_SUBS_HBA_IM
						        INTERVAL=$INTEG_HBA
						        STEPS=$STEPS_HBA
							else
						        CHAN_SUBS=$CHAN_SUBS_LBA_IM
						        INTERVAL=$INTEG_LBA
						        STEPS=$STEPS_LBA
							fi
						fi # end if [ $INSWITCH == 1 ]
				    fi # end (( $ncols == 2 ))
			
				    if (( $ncols >= 4 ))
				    then
				        CHAN_SUBS=`echo $line | awk '{print $3}'`
				        if [ $INSWITCH == 1 ] # BF
				        then
				            STEPS=`echo $line | awk '{print $4}'`
				            if [[ $ANT_SHORT == "HBA" ]]
				            then 
				                INTERVAL=$INTEG_HBA
				            else
				                INTERVAL=$INTEG_LBA
				            fi
				        else
				            INTERVAL=`echo $line | awk '{print $4}'`
				        fi
				    fi
			
				    if (( $ncols >= 7 ))
				    then
				        START=`echo $line | awk '{print $5}'`
				        TIME=`echo $line | awk '{print $6}'`
				        STATIONS=`echo $line | awk '{print $7}'`                
			
					    if (( $LST == 1 ))
					    then
				           LST_DIFF=120
					       # change the start time from LST to UT
					       #new_start=`date -j -v +$LST_DIFF"M" -f "%Y-%m-%dT%H:%M:%S" $START "+%Y-%m-%dT%H:%M:%S"`
					       #echo "new start is $new_start"
					       #START=$new_start
					       #previous_start=$new_start
					    else 
					       LST_DIFF=0
					    fi
				    fi
		
				    if (( $ncols == 8 ))
				    then
				        SUBBANDS=`echo $line | awk '{print $8}'`
				        SUBBANDS_SET=1
				    fi
				    
				    if [[ $ANTENNA == "HBA" ]] || [[ $ANTENNA == "HBAHigh" ]] || [[ $ANTENNA == "HBAMid" ]] || [[ $ANTENNA == "HBALow" ]]
				    then
				        ANT_SHORT=HBA
				    elif [[ $ANTENNA == "LBA" ]] || [[ $ANTENNA == "LBAHigh" ]] || [[ $ANTENNA == "LBALow" ]]
				    then
				        ANT_SHORT=LBA
				    else
					   echo "ERROR: Antenna must be set to any of these values BF: HBA, HBALow, HBAMid, HBAHigh, LBA, LBALow, LBAHigh"
					   echo "ERROR: Antenna must be set to any of these values IM: HBALow, HBAMid, HBAHigh, LBA, LBALow, LBAHigh"
				       exit 1
				    fi
				# when input table contains ra and dec values
				else # if (( $INTYPE == 2))
			        RA_DEG=`echo $line | awk '{print $1}'`
			        DEC_DEG=`echo $line | awk '{print $2}'`

				    if (( $ncols >= 3 ))
				    then
				       ANTENNA=`echo $line | awk '{print $3}'`
				       
                       if [[ $INSWITCH == 2 ]] # IM
                       then
						   if [[ $ANTENNA == "HBALow" ]] || [[ $ANTENNA == "HBAMid" ]] || [[ $ANTENNA == "HBAHigh" ]]
						   then
						       ANT_SHORT=HBA
						   elif [[ $ANTENNA == "LBA" ]] || [[ $ANTENNA == "LBALow" ]] || [[ $ANTENNA == "LBAHigh" ]]
						   then
						       ANT_SHORT=LBA
						   else
						      echo "ERROR: Antenna must be set to any of these values IM: HBALow, HBAMid, HBAHigh, LBA, LBALow, LBAHigh"
						      exit 1
						   fi
                       else  # BF
						   if [[ $ANTENNA == "HBA" ]] || [[ $ANTENNA == "HBALow" ]] || [[ $ANTENNA == "HBAMid" ]] || [[ $ANTENNA == "HBAHigh" ]]
						   then
						       ANT_SHORT=HBA
						   elif [[ $ANTENNA == "LBA" ]] || [[ $ANTENNA == "LBALow" ]] || [[ $ANTENNA == "LBAHigh" ]]
						   then
						       ANT_SHORT=LBA
						   else
						      echo "ERROR: Antenna must be set to any of these values BF: HBA, HBALow, HBAMid, HBAHigh, LBA, LBALow, LBAHigh"
						      exit 1
						   fi
                       fi
				    fi
			
				    if (( $ncols == 3 ))
				    then
	                    if [ $INSWITCH == 1 ] # BF
	                    then
							if [[ $ANT_SHORT == "HBA" ]] 
							then 
						        CHAN_SUBS=$CHAN_SUBS_HBA
						        STEPS=$STEPS_HBA
						        INTERVAL=$INTEG_HBA
							else
						        CHAN_SUBS=$CHAN_SUBS_LBA
						        STEPS=$STEPS_LBA
						        INTERVAL=$INTEG_LBA
							fi
						else
							if [[ $ANT_SHORT == "HBA" ]]  
							then 
						        CHAN_SUBS=$CHAN_SUBS_HBA_IM
						        INTERVAL=$INTEG_HBA
						        STEPS=$STEPS_HBA
							else
						        CHAN_SUBS=$CHAN_SUBS_LBA_IM
						        INTERVAL=$INTEG_LBA
						        STEPS=$STEPS_LBA
							fi
						fi # end if [ $INSWITCH == 1 ]
				    fi # end if (( $ncols == 3 ))
			
				    if (( $ncols >= 5 ))
				    then
				        CHAN_SUBS=`echo $line | awk '{print $4}'`
				        if [ $INSWITCH == 1 ] # BF
				        then
				            STEPS=`echo $line | awk '{print $5}'`
				            if [[ $ANT_SHORT == "HBA" ]]
				            then
				                INTERVAL=$INTEG_HBA
				            else
				                INTERVAL=$INTEG_LBA
				            fi
				        else
				            INTERVAL=`echo $line | awk '{print $5}'`
							if [[ $ANT_SHORT == "HBA" ]]  
							then 
						        STEPS=$STEPS_HBA
							else
						        STEPS=$STEPS_LBA
							fi
				        fi
				    fi
			
				    if (( $ncols >= 8 ))
				    then
				        START=`echo $line | awk '{print $6}'`
				        TIME=`echo $line | awk '{print $7}'`
				        STATIONS=`echo $line | awk '{print $8}'`                
			
					    if (( $LST == 1 ))
					    then
				           LST_DIFF=120
					       # change the start time from LST to UT
					       #new_start=`date -j -v +$LST_DIFF"M" -f "%Y-%m-%dT%H:%M:%S" $START "+%Y-%m-%dT%H:%M:%S"`
					       #echo "new start is $new_start"
					       #START=$new_start
					       #previous_start=$new_start
					    else 
					       LST_DIFF=0
					    fi
				    fi
		
				    if (( $ncols == 9 ))
				    then
				        SUBBANDS=`echo $line | awk '{print $9}'`
				        SUBBANDS_SET=1
				    fi
				    
				    if [[ $ANTENNA == "HBA" ]] || [[ $ANTENNA == "HBAHigh" ]] || [[ $ANTENNA == "HBAMid" ]] || [[ $ANTENNA == "HBALow" ]]
				    then
				        ANT_SHORT=HBA
				    elif [[ $ANTENNA == "LBA" ]] || [[ $ANTENNA == "LBAHigh" ]] || [[ $ANTENNA == "LBALow" ]]
				    then
				        ANT_SHORT=LBA
				    else
					   echo "ERROR: Antenna must be set to any of these values BF: HBA, HBALow, HBAMid, HBAHigh, LBA, LBALow, LBAHigh"
					   echo "ERROR: Antenna must be set to any of these values IM: HBALow, HBAMid, HBAHigh, LBA, LBALow, LBAHigh"
				       exit 1
				    fi
				fi  #end if (( $INTYPE == 1))
		
		        ##################################################
		        # Set up the Subbands if they were not set via the file
		        if [ $SUBBANDS_SET == 0 ]
		        then
		            if [ $INSWITCH == 1 ]  # BF
		            then
		                if [ $user_subbands_hba == 0 ] && [ $ANTENNA == "HBA" ]
		                then
		                    if [ $clock == "200" ]
		                    then
		                        SUBBANDS="200..443"
		                    else
		                        SUBBANDS="195..438"
                            fi
		                elif [ $user_subbands_hba == 1 ] && [ $ANTENNA == "HBA" ]
		                then
                            needs_expand=`echo $SUBBANDS_HBA | grep "," | grep ".."`
				            if [ $needs_expand != "" ]
				            then
				               SUBBANDS=`echo "$SUBBANDS_HBA" | expand_sblist.py`
				            else
						       SUBBANDS=$SUBBANDS_HBA
						    fi
		                elif [ $user_subbands_hbahigh == 0 ] && [ $ANTENNA == "HBAHigh" ]
		                then
		                    if [ $clock == "200" ]
		                    then
		                        SUBBANDS="77..320"
		                    else
		                        SUBBANDS="3..246"
                            fi
		                elif [ $user_subbands_hbahigh == 1 ] && [ $ANTENNA == "HBAHigh" ]
		                then
                            needs_expand=`echo $SUBBANDS_HBAHigh | grep "," | grep ".."`
				            if [ $needs_expand != "" ]
				            then
				               SUBBANDS=`echo "$SUBBANDS_HBAHigh" | expand_sblist.py`
				            else
						       SUBBANDS=$SUBBANDS_HBAHigh
						    fi
		                elif [ $user_subbands_hbamid == 0 ] && [ $ANTENNA == "HBAMid" ]
		                then
		                    if [ $clock == "200" ]
		                    then
		                        echo "ERROR: HBAMid is not allowed with the 200MHz clock;  please recheck your inputs; clock is set for entire list of observations."
		                        exit 1
		                    else
		                        SUBBANDS="140..383"
                            fi
		                elif [ $user_subbands_hbamid == 1 ] && [ $ANTENNA == "HBAMid" ]
		                then
		                    if [ $clock == "200" ]
		                    then
		                        echo "ERROR: HBAMid is not allowed with the 200MHz clock;  please recheck your inputs; clock is set for entire list of observations."
		                        exit 1
		                    else
	                            needs_expand=`echo $SUBBANDS_HBAMid | grep "," | grep ".."`
					            if [ $needs_expand != "" ]
					            then
					               SUBBANDS=`echo "$SUBBANDS_HBAMid" | expand_sblist.py`
					            else
							       SUBBANDS=$SUBBANDS_HBAMid
							    fi
                            fi
		                elif [ $user_subbands_hbalow == 0 ] && [ $ANTENNA == "HBALow" ]
		                then
		                    if [ $clock == "200" ]
		                    then
		                        SUBBANDS="200..443"
		                    else
		                        SUBBANDS="195..438"
                            fi
		                elif [ $user_subbands_hbalow == 1 ] && [ $ANTENNA == "HBALow" ]
		                then
                            needs_expand=`echo $SUBBANDS_HBALow | grep "," | grep ".."`
				            if [ $needs_expand != "" ]
				            then
				               SUBBANDS=`echo "$SUBBANDS_HBALow" | expand_sblist.py`
				            else
						       SUBBANDS=$SUBBANDS_HBALow
						    fi
		                elif [ $user_subbands_lba == 0 ] && [ $ANTENNA == "LBA" ]
		                then
		                    if [ $clock == "200" ]
		                    then
		                        SUBBANDS="100..343"
		                    else
		                        SUBBANDS="128..371"
                            fi
		                elif [ $user_subbands_lba == 1 ] && [ $ANTENNA == "LBA" ]
		                then
                            needs_expand=`echo $SUBBANDS_LBA | grep "," | grep ".."`
				            if [ $needs_expand != "" ]
				            then
				               SUBBANDS=`echo "$SUBBANDS_LBA" | expand_sblist.py`
				            else
						       SUBBANDS=$SUBBANDS_LBA
						    fi
		                elif [ $user_subbands_lbahigh == 0 ] && [ $ANTENNA == "LBAHigh" ]
		                then
		                    if [ $clock == "200" ]
		                    then
		                        SUBBANDS="154..397"
		                    else
		                        SUBBANDS="195..438"
                            fi
		                elif [ $user_subbands_lbahigh == 1 ] && [ $ANTENNA == "LBAHigh" ]
		                then
                            needs_expand=`echo $SUBBANDS_LBAHigh | grep "," | grep ".."`
				            if [ $needs_expand != "" ]
				            then
				               SUBBANDS=`echo "$SUBBANDS_LBAHigh" | expand_sblist.py`
				            else
						       SUBBANDS=$SUBBANDS_LBAHigh
						    fi
		                elif [ $user_subbands_lbalow == 0 ] && [ $ANTENNA == "LBALow" ]
		                then
		                    if [ $clock == "200" ]
		                    then
		                        SUBBANDS="100..343"
		                    else
		                        SUBBANDS="128..371"
                            fi
		                elif [ $user_subbands_lbalow == 1 ] && [ $ANTENNA == "LBALow" ]
		                then
		                    needs_expand=""
                            needs_expand=`echo $SUBBANDS_LBALow | grep "," | grep ".."`
                            echo "A2, 1 - $needs_expand"
				            if [ $needs_expand != "" ]
				            then
				               SUBBANDS=`echo "$SUBBANDS_LBALow" | expand_sblist.py`
				            else
						       SUBBANDS=$SUBBANDS_LBALow
						    fi
                        else
                            echo "ERROR: BF, unable to set proper list of subbands"
                            exit 1
		                fi
		            else  # if [ $INSWITCH == 2 ]  (Imaging)
		                if [ $user_subbands_hbahigh == 0 ] && [ $ANTENNA == "HBAHigh" ]
		                then
		                    if [ $clock == "200" ]
		                    then
		                        SUBBANDS="52..255"
		                    else
		                        SUBBANDS="52..255"  
                            fi
		                elif [ $user_subbands_hbahigh == 1 ] && [ $ANTENNA == "HBAHigh" ]
		                then
                            needs_expand=`echo $SUBBANDS_HBAHigh | grep "," | grep ".."`
				            if [ $needs_expand != "" ]
				            then
				               SUBBANDS=`echo "$SUBBANDS_HBAHigh" | expand_sblist.py`
				            else
						       SUBBANDS=$SUBBANDS_HBAHigh
						    fi
		                elif [ $user_subbands_hbamid == 0 ] && [ $ANTENNA == "HBAMid" ]
		                then
		                    if [ $clock == "200" ]
		                    then
		                        echo "ERROR: HBAMid is not allowed with the 200MHz clock;  please recheck your inputs; clock is set for entire list of observations."
		                        exit 1
		                    else
		                        SUBBANDS="66..309"
                            fi
		                elif [ $user_subbands_hbamid == 1 ] && [ $ANTENNA == "HBAMid" ]
		                then
		                    if [ $clock == "200" ]
		                    then
		                        echo "ERROR: HBAMid is not allowed with the 200MHz clock;  please recheck your inputs; clock is set for entire list of observations."
		                        exit 1
		                    else
	                            needs_expand=`echo $SUBBANDS_HBAMid | grep "," | grep ".."`
					            if [ $needs_expand != "" ]
					            then
					               SUBBANDS=`echo "$SUBBANDS_HBAMid" | expand_sblist.py`
					            else
							       SUBBANDS=$SUBBANDS_HBAMid
							    fi
                            fi
		                elif [ $user_subbands_hbalow == 0 ] && [ $ANTENNA == "HBALow" ]
		                then
		                    if [ $clock == "200" ]
		                    then
		                        SUBBANDS="77..320"
		                    else
		                        SUBBANDS="77..320"
                            fi
		                elif [ $user_subbands_hbalow == 1 ] && [ $ANTENNA == "HBALow" ]
		                then
                            needs_expand=`echo $SUBBANDS_HBALow | grep "," | grep ".."`
				            if [ $needs_expand != "" ]
				            then
				               SUBBANDS=`echo "$SUBBANDS_HBALow" | expand_sblist.py`
				            else
						       SUBBANDS=$SUBBANDS_HBALow
						    fi
		                elif [ $user_subbands_lbahigh == 0 ] && [ $ANTENNA == "LBAHigh" ]
		                then
		                    if [ $clock == "200" ]
		                    then
		                        SUBBANDS="156..399"
		                    else
		                        SUBBANDS="195..438"
                            fi
		                elif [ $user_subbands_lbahigh == 1 ] && [ $ANTENNA == "LBAHigh" ]
		                then
                            needs_expand=`echo $SUBBANDS_LBAHigh | grep "," | grep ".."`
				            if [ $needs_expand != "" ]
				            then
				               SUBBANDS=`echo "$SUBBANDS_LBAHigh" | expand_sblist.py`
				            else
						       SUBBANDS=$SUBBANDS_LBAHigh
						    fi
		                elif [ $user_subbands_lba == 0 ] && [ $ANTENNA == "LBA" ]
		                then
		                    if [ $clock == "200" ]
		                    then
		                        SUBBANDS="156..399"
		                    else
		                        SUBBANDS="195..438"
                            fi
		                elif [ $user_subbands_lba == 1 ] && [ $ANTENNA == "LBA" ]
		                then
                            needs_expand=`echo $SUBBANDS_LBA | grep "," | grep ".."`
				            if [ $needs_expand != "" ]
				            then
				               SUBBANDS=`echo "$SUBBANDS_LBA" | expand_sblist.py`
				            else
						       SUBBANDS=$SUBBANDS_LBA
						    fi
		                elif [ $user_subbands_lbalow == 0 ] && [ $ANTENNA == "LBALow" ]
		                then
		                    if [ $clock == "200" ]
		                    then
		                        SUBBANDS="54..297"
		                    else
		                        SUBBANDS="128..371"
                            fi
		                elif [ $user_subbands_lbalow == 1 ] && [ $ANTENNA == "LBALow" ]
		                then
		                	needs_expand=""
                            needs_expand=`echo $SUBBANDS_LBALow | grep "," | grep ".."`
                            echo "A2, 2 - $needs_expand"
				            if [ $needs_expand != "" ]
				            then
				               SUBBANDS=`echo "$SUBBANDS_LBALow" | expand_sblist.py`
				            else
						       SUBBANDS=$SUBBANDS_LBALow
						    fi
                        else
                            echo "ERROR: IM, unable to set proper list of subbands"
                            exit 1
		                fi		            
		            fi # if [ $INSWITCH == 1 ] 
			    fi # end if [ $SUBBANDS_SET == 0 ]
		    fi # end if (( $beam > 1 ))
		    
		    ##################################################
		    # If multi-beam obs, split up subbands by N beams (TBD) if subbands > 244
		    if (( $MULTI == 1 )) && (( $beam == 1 )) && (( $nbeams > 1 )) && (( $SUBBANDS_SET == 0 ))
		    then
		       total_subbands=`echo $SUBBANDS | expand_sblist.py | awk -F"," '{print NF}'`
		       if (( $total_subbands >= 244 ))
		       then
		          echo "WARNING: Multi-beam specification must have total subbands <= 244;"
		          echo "         Input has $total_subbands;  splitting subbands by $nbeams beams"
		          split_subs=`echo $total_subbands / $nbeams | bc`
		       else
		          split_subs=$total_subbands
		       fi
		       SUBBANDS_ORIG=`echo $SUBBANDS | expand_sblist.py`
		    fi
		    if (( $MULTI == 1 )) && (( $SUBBANDS_SET == 0 )) && (( $nbeams > 1 ))
		    then
		          if (( $beam == 1 ))
		          then
		             min_range=1
		             max_range=$split_subs
		          elif [[  $user_subbands_hba == 1  &&  $ANTENNA == "HBA"  ]] ||  [[  $user_subbands_hbahigh == 1  &&  $ANTENNA == "HBAHigh"  ]] ||  [[  $user_subbands_hbamid == 1  &&  $ANTENNA == "HBAMid"  ]] ||   [[  $user_subbands_hbalow == 1  &&  $ANTENNA == "HBALow"  ]] ||   [[  $user_subbands_lba == 1  &&  $ANTENNA == "LBA"  ]] || [[  $user_subbands_lbahigh == 1  &&  $ANTENNA == "LBAHigh"  ]] ||  [[ $user_subbands_lbalow == 1  &&  $ANTENNA == "LBALow"  ]] 
		          then
		             min_range=1
		             max_range=$split_subs
		          else
		             min_range=`echo "$max_range + 1" | bc`
		             max_range=`echo "$min_range + $split_subs - 1" | bc`
#		             min_range=1
#		             max_range=$split_subs
		          fi
		          SUBBANDS=`echo $SUBBANDS_ORIG | cut -d"," -f$min_range-$max_range`
            fi
            
		    #make sure station list has all capital letters
		    STATIONS=`echo $STATIONS | sed 's/a-z/A-Z/g'`                
		                   
		    if (( $INTYPE == 1 ))
		    then
			    found_OBJECT=`grep -i $OBJECT $catalog | head -1`
			    if [[ $found_OBJECT == "" ]]
			    then
			       echo "WARNING: OBJECT $OBJECT not found in catalog $catalog."
			       echo "WARNING: skipping OBJECT $OBJECT from template creation."
			       ((beam = $beam + 1))
			       skip=1
			       continue
			    fi
	        fi
	        
	        ##################################################
	        # Check stations are set and create the XML stations list
		    found_stations=`echo $STATIONS | egrep -i "CS|RS"`
		    if [[ $found_stations == "" ]]
		    then
		       echo "WARNING: STATION list ($STATIONS) seems invalid, does not contain CS or RS characters."
		       echo "WARNING: skipping OBJECT $OBJECT from template creation."
		       continue
		    fi
	
	        STATION_LIST=""
	        list=`echo $STATIONS | sed 's/,/ /g'`
	        for station in $list
	        do
	           STATION_LIST="$STATION_LIST \<station name=\"$station\"\/\>"
	#           STATION_LIST="<station name=\"$station\"/> RT $STATION_LIST"
	#           STATION_LIST=`echo $STATION_LIST | tr "RT" "\n"`
	        done
	
	        ##################################################
	        # Get the RA an DEG and convert to radians
	        if (( $INTYPE == 1 )) && (( $INSWITCH == 1 ))
	        then
			    RA_DEG=`grep -i $OBJECT $catalog  | head -1 | awk '{print $1}'`
			    DEC_DEG=`grep -i $OBJECT $catalog  | head -1 | awk '{print $2}'`
	        elif (( $INTYPE == 1 )) && (( $INSWITCH == 2 ))
	        then
		        RA_DEG=`grep -i "$OBJECT" $catalog  | head -1 | awk '{print $1}'`
		        DEC_DEG=`grep -i "$OBJECT" $catalog  | head -1 | awk '{print $2}'`
		    fi
		    if [[ $RA_DEG == "" ]] || [[ $DEC_DEG == "" ]] 
		    then
		       echo "WARNING: unable to find position for OBJECT $OBJECT in position catalog ($catalog)."
		       echo "WARNING: skipping OBJECT $OBJECT from template creation."
		       continue
		    fi

	        if (( $sexages == 0 ))
	        then
	            check_sexages=`echo $RA_DEG | grep ':'`
	            if [[ $check_sexages != "" ]]
	            then
	               echo "WARNING: your input RA appears to be in sexagesimal coordinates, not decimal degrees;  assuming -sexages flag"
	               sexages=1
	            fi
            fi
	        if (( $sexages == 1 ))
	        then
	           RA_DEG=`rasex2deg.sh $RA_DEG`
	           DEC_DEG=`decsex2deg.sh $DEC_DEG`
	        fi

	        #convert to radians	   
	        
			RA=`echo "scale=10; $RA_DEG * $pi / 180.0" | bc | awk '{printf("%1.9f\n",$1)}'`
			DEC=`echo "scale=10; $DEC_DEG * $pi / 180.0" | bc | awk '{printf("%1.9f\n",$1)}'`
					
	        ##################################################
	        # Calculate start/stop related values
	        # Break up into time less than one minute or >= one minute
            if (( $beam == 1 ))
            then

				if (( $TIME < 1 )) && (( $TIME > 0 ))
				then
			 	    #calculate the start/stop time if set by the user as input (assume 1 min for times less than 1 min)
					if (( $user_start == 1 ))
					then
					    START=$previous_start
					    tot_time=`echo "1 + $GAP" | bc"`
					   
						if [[ $system_typ == "Darwin" ]]
						then
					          previous_start=`date -j -v +$tot_time"M" -f "%Y-%m-%dT%H:%M:%S" $START "+%Y-%m-%dT%H:%M:%S"`
						else
		                                  tmp_start=`echo "$START" | sed 's/T/ /'`
		                                  tmp=`date -d "$tmp_start" "+%s"`
		                                  new_date_seconds=`echo "$tmp + $tot_time * 60" | bc` 
						  #new_date_seconds=`echo "date -d \"$START\" \"+%s\" + $tot_time * 60" | bc`  
					          previous_start=`date -d @$new_date_seconds "+%Y-%m-%dT%H:%M:%S"`
						fi
					elif (( $LST == 1 ))
					then
					    tot_time=$LST_DIFF
					   
						if [[ $system_typ == "Darwin" ]]
						then
					          START=`date -j -v +$tot_time"M" -f "%Y-%m-%dT%H:%M:%S" $START "+%Y-%m-%dT%H:%M:%S"`
						else
		                                  tmp_start=`echo "$START" | sed 's/T/ /'`
		                                  tmp=`date -d "$tmp_start" "+%s"`
		                                  new_date_seconds=`echo "$tmp + $tot_time * 60" | bc` 
						  # new_date_seconds=`echo "date -d \"$START\" \"+%s\" + $tot_time * 60" | bc`  
					          START=`date -d @$new_date_seconds "+%Y-%m-%dT%H:%M:%S"`
						fi
					fi
		
				   # change decimal minutes into seconds
				   TIME=`echo $TIME | awk '{print ($1 * 60)}'`
				   if (( $TIME > 0 )) && (( $TIME <= 10 )) 
				   then
				      DURATION=`echo "PT0"$TIME"S"`	
				   else 
				      DURATION=`echo "PT"$TIME"S"`	
				   fi	
				   
		        elif (( $TIME >= 1 ))
		        then
			 	    #calculate the start/stop time if set by the user as input (assume 1 min for times less than 1 min)
					if (( $user_start == 1 ))
					then
					    START=$previous_start
					    tot_time=`echo "$TIME + $GAP" | bc`
					   			   
						if [[ $system_typ == "Darwin" ]]
						then
					          previous_start=`date -j -v +$tot_time"M" -f "%Y-%m-%dT%H:%M:%S" $START "+%Y-%m-%dT%H:%M:%S"`
						else
		                                  tmp_start=`echo "$START" | sed 's/T/ /'`
		                                  tmp=`date -d "$tmp_start" "+%s"`
		                                  new_date_seconds=`echo "$tmp + $tot_time * 60" | bc` 
			  			  #new_date_seconds=`echo "date -d \"$START\" \"+%s\" + $tot_time * 60" | bc`
					          previous_start=`date -d @$new_date_seconds "+%Y-%m-%dT%H:%M:%S"`
						fi
					elif (( $LST == 1 ))
					then
					    tot_time=$LST_DIFF
					   
						if [[ $system_typ == "Darwin" ]]
						then
					          START=`date -j -v +$tot_time"M" -f "%Y-%m-%dT%H:%M:%S" $START "+%Y-%m-%dT%H:%M:%S"`
						else
		                                  tmp_start=`echo "$START" | sed 's/T/ /'`
		                                  tmp=`date -d "$tmp_start" "+%s"`
		                                  new_date_seconds=`echo "$tmp + $tot_time * 60" | bc` 
						  #new_date_seconds=`echo "date -d \"$START\" \"+%s\" + $tot_time * 60" | bc`  
					          START=`date -d @$new_date_seconds "+%Y-%m-%dT%H:%M:%S"`
						fi
					fi
		
					if (( $TIME > 0 )) && (( $TIME <= 10 ))
					then
					   DURATION=`echo "PT0"$TIME"M"`
					elif (( $TIME >= 10 ))
					then
					   DURATION=`echo "PT"$TIME"M"`
					else
					   echo "ERROR: duration value ($TIME) is not understood"
					   continue
					fi
				else
					   echo "ERROR: duration value ($TIME) is not understood"
					   continue
			    fi
		
				if [[ $system_typ == "Darwin" ]]
				then
				      END=`date -j -v +$TIME"M" -f "%Y-%m-%dT%H:%M:%S" $START "+%Y-%m-%dT%H:%M:%S"`
				else
		              tmp_start=`echo "$START" | sed 's/T/ /'`
		              tmp=`date -d "$tmp_start" "+%s"`
				      new_date_seconds=`echo "$tmp + $TIME * 60" | bc` 
		  	              END=`date -d @$new_date_seconds "+%Y-%m-%dT%H:%M:%S"`
				fi
	        fi # end if (( $beam == 1 ))
	        
	        ##################################################
	        # Antenna filter related setttings	        
            
		    if [ $ANTENNA == "HBAHigh" ]
		    then
		       INSTRUMENT_FILTER="210-250"
		    elif [ $ANTENNA == "HBAMid" ] && [ $clock == "200" ]
		    then
		       echo "ERROR: HBAMid is not allowed with the 200MHz clock;  please recheck your inputs; clock is set for entire list of observations."
		       exit 1
		    elif [ $ANTENNA == "HBAMid" ] && [ $clock == "160" ]
		    then
		       INSTRUMENT_FILTER="170-230"
		    elif [ $ANTENNA == "HBALow" ] || [ $ANTENNA == "HBA" ]
		    then
		       INSTRUMENT_FILTER="110-190"
		    elif [ $ANTENNA == "LBAHigh" ] 
		    then
		       if (( $clock == 200 ))
		       then 
		          INSTRUMENT_FILTER="30-90"
		       else
		          INSTRUMENT_FILTER="30-70"
		       fi
		    elif [ $ANTENNA == "LBALow" ]  
		    then
		       if (( $clock == 200 ))
		       then 
		          INSTRUMENT_FILTER="10-90"
		       else
		          INSTRUMENT_FILTER="10-70"
		       fi
		    elif [ $ANTENNA == "LBA" ]  && [ $INSWITCH == 1 ]  # defaults to LBALow values
		    then
		       if (( $clock == 200 ))
		       then 
		          INSTRUMENT_FILTER="10-90"
		       else
		          INSTRUMENT_FILTER="10-70"
		       fi
		    elif [ $ANTENNA == "LBA" ]  && [ $INSWITCH == 2 ]  # defaults to LBAHigh values
		    then
		       if (( $clock == 200 ))
		       then 
		          INSTRUMENT_FILTER="30-90"
		       else
		          INSTRUMENT_FILTER="30-70"
		       fi
            fi

	        ##################################################
	        # Antenna mode settings  	        

	       if [ $user_modeHBA == 0 ] && [ $ANT_SHORT == "HBA" ]
	       then
	          if [ $INSWITCH == 1 ] #BF
	          then
	             ANTENNA_SETTING="HBA $modeHBA_BF"
	          else
	             ANTENNA_SETTING="HBA $modeHBA_IM"
	          fi
	       elif [ $user_modeHBA == 1 ] && [ $ANT_SHORT == "HBA" ]
	       then
	          if [ $INSWITCH == 1 ] #BF
	          then
	             modeHBA=$modeHBA_BF
	          else
	             modeHBA=$modeHBA_IM
	          fi
	          if [ $modeHBA == "Zero" ] || [ $modeHBA == "One" ] || [ $modeHBA == "Dual" ] || [ $modeHBA == "Joined" ] || [ $modeHBA == "Dual_Inner" ]
	          then
	             ANTENNA_SETTING="HBA $modeHBA"
	          else 
	              echo "WARNING: user antenna mode HBA $modeHBA is not recognized, using default"
		          if [ $INSWITCH == 1 ] #BF
		          then
		             ANTENNA_SETTING="HBA $modeHBA_BF"
		          else
		             ANTENNA_SETTING="HBA $modeHBA_IM"
		          fi
	          fi
	       fi

	       if [ $user_modeLBA == 0 ] && [ $ANT_SHORT == "LBA" ]
	       then
	          ANTENNA_SETTING="LBA $modeLBA"
	       elif [ $user_modeLBA == 1 ] && [ $ANT_SHORT == "LBA" ]
	       then 
	          if [ $modeLBA == "Outer" ] || [ $modeLBA == "Inner" ] || [ $modeLBA == "Sparse Even" ] || [ $modeLBA == "Sparse Odd" ] || [ $modeLBA == "X" ] || [ $modeLBA == "Y" ]
	          then
	             ANTENNA_SETTING="LBA $modeLBA"
	          else 
	             echo "WARNING: user antenna mode LBA $modeLBA is not recognized, using default LBA Outer"
	             ANTENNA_SETTING="LBA Outer"
	          fi
	       fi

	        # sanity check on the time interval						
			if (( $INTERVAL < 1 ))
			then
			    echo "ERROR: integration time of $INTERVAL is invalid; must be >= 1."
			    exit 1
			fi

            # increase the coutner if in multi beam mode
	       	if [ $MULTI == 0 ]
	        then
	           beam_counter=0
	        else
	           ((beam_counter = $beam - 1))
	        fi
			
	        ##################################################
			# Print the basic information about input parameters to STDOUT at start 
			if (( $INTYPE == 1 ))
			then
			   echo "OBJECT NAME = $OBJECT"
			   OBJECT="$OBJECT ($ANT_SHORT)"
			   if [ $nbeams == 1 ]
			   then
			      OBJECT_LONG=`echo $OBJECT | sed -e "s/$ANT_SHORT/$ANTENNA/g"`
			      OBJECT_LONG_BEAM=`echo $OBJECT | sed -e "s/$ANT_SHORT/$ANTENNA/g"`
			   else
			      OBJECT_LONG=`echo "$OBJECT Multi-Beam" | sed -e "s/$ANT_SHORT/$ANTENNA/g"`
			      OBJECT_LONG_BEAM=`echo "$OBJECT Multi-Beam #$beam" | sed -e "s/$ANT_SHORT/$ANTENNA/g"`
			   fi
			else
			   if [ $nbeams == 1 ]
			   then
			      OBJECT="Pos $RA_DEG $DEC_DEG"
			      PULSAR="Pos $RA_DEG $DEC_DEG"
			      OBJECT_LONG="Pos $RA_DEG $DEC_DEG ($ANTENNA)"
			      OBJECT_LONG_BEAM="Pos $RA_DEG $DEC_DEG ($ANTENNA)"
			   else
			      OBJECT="Pos $RA_DEG $DEC_DEG"
			      PULSAR="Pos $RA_DEG $DEC_DEG"
			      OBJECT_LONG="Pos $RA_DEG $DEC_DEG ($ANTENNA) Multi-Beam"
			      OBJECT_LONG_BEAM="Pos $RA_DEG $DEC_DEG ($ANTENNA) Multi-Beam #$beam"
			   fi
			fi
			if [ $NAMECOL = 1 ]
			then
			   # grab the same row number from the namecol file
			   OBSNAME=`sed -n "$counter p" /tmp/$$xmltmp_namecol`
			   OBSNAME="$OBSNAME ($ANT_SHORT)"
			   if [[ $OBSNAME == "" ]]
			   then
			      echo "WARNING: unable to get the proper Observation Name column;  using source/position instead" 
			      OBSNAME=$OBJECT
			   fi
			else
			   OBSNAME=$OBJECT
			fi
			
			echo "MOM Observation name will be: $OBSNAME" 
			echo "RA Pointing = $RA_DEG deg"
			echo "DEC Pointing = $DEC_DEG deg"
			echo "RA Pointing = $RA radians"
			echo "DEC Pointing = $DEC radians"
		    echo "ANTENNA = $ANTENNA"
		    echo "ANTENNA short name = $ANT_SHORT"
		    echo "ANTENNA Setting = $ANTENNA_SETTING"
		    echo "Instrument Filter = $INSTRUMENT_FILTER"
		    echo "IM Integration Interval = $INTERVAL"
			if (( $LST == 1 ))
			then
			   echo "Observation Start Time is given as input in LST units"
			fi
			echo "Observation Start Time = $START (UT)"
			echo "Observation duration = $TIME minutes"
			echo "Time Start $START + duration $TIME min = $END End Time"
			echo "Subbands list = $SUBBANDS"
			echo "Stations list = $STATIONS"
			if [ $INSWITCH = 1 ]
			then
			   echo "Channels per Subband = $CHAN_SUBS"
			   echo "Integration Steps = $STEPS"
	        else
			   echo "Channels per Subband = $CHAN_SUBS"
	        fi
	        
			IM_TF=false
			if (( $IM == 1 )) 
			then
			   for jj in $IM_LIST
			   do 
			      if (( $counter == $jj ))
			      then
			         echo "Imaging turned on (TRUE) for this Pulsar Observation"
			         IM_TF=true
			      fi
			   done
			fi

            # when BF, turn on/off Incoherentstokes, coherent, beam-formed, filtered-data modes
			if [ $INSWITCH = 1 ]
			then
			    IS_TF=true
			    if [[ "$IS_LIST" != "" ]]
			    then 
				   for jj in $IS_LIST
				   do 
				      if (( $counter == $jj ))
				      then
				         echo "Beam-Formed Incoherentstokes turned on (TRUE) for this Observation"
				         IS_TF=true
				      fi
				   done			    
			    fi
			    
			    if (( $ISNOT == 1 ))
			    then 
			        IS_TF=false
				    if [[ "$ISNOT_LIST" != "" ]]
				    then 
					   for jj in $ISNOT_LIST
					   do 
					      if (( $counter == $jj ))
					      then
					         echo "Beam-Formed Incoherentstokes turned OFF (false) for this Observation"
					         IS_TF=false
					      fi
					   done			    
				    fi
				fi
			fi

            # when Imaging, can only turn ON additional BF-related modes
			if [ $INSWITCH = 2 ]
			then
			    IS_TF=false
			    if (( $IS == 1 )) 
				then
				   for jj in $IS_LIST
				   do 
				      if (( $counter == $jj ))
				      then
				         echo "Beam-Formed Incoherentstokes (IS) turned on (TRUE) for this Observation"
				         IS_TF=true
				      fi
				   done
				fi
			fi
						
			CS_TF=false
			if (( $CS == 1 )) 
			then
			   for jj in $CS_LIST
			   do 
			      if (( $counter == $jj ))
			      then
			         echo "Beam-Formed Coherentstokes (CS) turned on (TRUE) for this Observation"
			         CS_TF=true
			      fi
			   done
			fi

			FD_TF=false
			if (( $FD == 1 )) 
			then
			   for jj in $FD_LIST
			   do 
			      if (( $counter == $jj ))
			      then
			         echo "Beam-Formed Filtered-Data (FD) turned on (TRUE) for this Observation"
			         FD_TF=true
			      fi
			   done
			fi

			BF_TF=false
			if (( $BF == 1 )) 
			then
			   for jj in $BF_LIST
			   do 
			      if (( $counter == $jj ))
			      then
			         echo "Beam-Formed (beam-formed data 'BF') turned on (TRUE) for this Observation"
			         BF_TF=true
			      fi
			   done
			fi
		        
			#write the observation section per OBJECT in the list
	
	#		if [[ $ANTENNA == "HBA" ]] 
	#		then 
	#		   echo "ANTENNA set to: $ANTENNA"
	#		elif [[ $ANTENNA == "LBA" ]]
	#		then
	#		   echo "ANTENNA set to: $ANTENNA"
	#		   if [[ $MOM == "" ]]
	#		   then
	#		      echo "ERROR: for antenna=$ANTENNA, you must set the MOM_ID using the -mom input switch"
	#		      exit 1
	#		   fi 
	#		fi
	        
	        ##################################################
	        # Create the XML per beam
	       
	        if [ $INSWITCH == 1 ] && [ $skip == 0 ]
	        then 
	           if (( $beam_counter == 0 ))
	           then
#	              sed -e "s/FILL IN OBSERVATION NAME/$OBSNAME ($ANTENNA)/g" -e "s/RA/$RA/g" -e "s/DEC/$DEC/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" -e "s/FILL IN DESCRIPTION/Obs $OBJECT_LONG at $START for $TIME min/g" -e "s/RDEG/$RA_DEG/g" -e "s/DDEG/$DEC_DEG/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" -e "s/LENGTH/$DURATION/g" -e "s/FILL IN TIMESTAMP/$date/g" -e "s/SUBBANDS/$SUBBANDS/g" -e "s/STATION_LIST/$STATION_LIST/g" -e "s/PULSAR/$PULSAR/g" -e "s/CHANNELS PER SUBBAND/$CHAN_SUBS/g" -e "s/INTEG STEPS/$STEPS/g" -e "s/PROJECT NAME/$PROJECT/g" -e "s/IMAGING/$IM_TF/g" -e "s/IS_TF/$IS_TF/g" -e "s/CS_TF/$CS_TF/g" -e "s/FD_TF/$FD_TF/g" -e "s/BF_TF/$BF_TF/g" -e "s/ANTENNA SETTING/$ANTENNA_SETTING/g" -e "s/INSTRUMENT FILTER/$INSTRUMENT_FILTER/g" -e "s/INTEG INTERVAL/$INTERVAL/g" $middle >> $outfile
	              sed -e "s/FILL IN OBSERVATION NAME/$OBSNAME/g" -e "s/RA/$RA/g" -e "s/DEC/$DEC/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" -e "s/FILL IN DESCRIPTION/Obs $OBJECT_LONG at $START for $TIME min/g" -e "s/RDEG/$RA_DEG/g" -e "s/DDEG/$DEC_DEG/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" -e "s/LENGTH/$DURATION/g" -e "s/FILL IN TIMESTAMP/$date/g" -e "s/SUBBANDS/$SUBBANDS/g" -e "s/STATION_LIST/$STATION_LIST/g" -e "s/PULSAR/$PULSAR/g" -e "s/CHANNELS PER SUBBAND/$CHAN_SUBS/g" -e "s/INTEG STEPS/$STEPS/g" -e "s/PROJECT NAME/$PROJECT/g" -e "s/IMAGING/$IM_TF/g" -e "s/IS_TF/$IS_TF/g" -e "s/CS_TF/$CS_TF/g" -e "s/FD_TF/$FD_TF/g" -e "s/BF_TF/$BF_TF/g" -e "s/ANTENNA SETTING/$ANTENNA_SETTING/g" -e "s/INSTRUMENT FILTER/$INSTRUMENT_FILTER/g" -e "s/INTEG INTERVAL/$INTERVAL/g" -e "s/CLOCK/$clock/g" -e "s/NOF TA RINGS/$NOF_RINGS/g" -e "s/TA RING SIZE/$RING_SIZE/g" $middle >> $outfile
	           fi
	        elif [ $INSWITCH == 2 ]  && [ $skip == 0 ]
	        then
	           if (( $beam_counter == 0 ))
	           then
	              sed -e "s/FILL IN OBSERVATION NAME/$OBSNAME/g" -e "s/RA/$RA/g" -e "s/DEC/$DEC/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" -e "s/FILL IN DESCRIPTION/$OBJECT_LONG at $START for $TIME min/g" -e "s/RDEG/$RA_DEG/g" -e "s/DDEG/$DEC_DEG/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" -e "s/LENGTH/$DURATION/g" -e "s/FILL IN TIMESTAMP/$date/g" -e "s/SUBBANDS/$SUBBANDS/g" -e "s/STATION_LIST/$STATION_LIST/g" -e "s/OBJECT/$OBJECT/g" -e "s/PROJECT NAME/$PROJECT/g" -e "s/ANTENNA SETTING/$ANTENNA_SETTING/g" -e "s/INSTRUMENT FILTER/$INSTRUMENT_FILTER/g" -e "s/INTEG INTERVAL/$INTERVAL/g" -e "s/TARGET NAME/$OBJECT/g" -e "s/IS_TF/$IS_TF/g" -e "s/CS_TF/$CS_TF/g" -e "s/FD_TF/$FD_TF/g" -e "s/BF_TF/$BF_TF/g" -e "s/CHANNELS PER SUBBAND/$CHAN_SUBS/g" -e "s/INTEG STEPS/$STEPS/g" -e "s/CLOCK/$clock/g " $middle >> $outfile
	           fi
	        fi 
		        
	        if [ $INSWITCH == 1 ] && [ $skip == 0 ]
	        then 
#	           sed -e "s/FILL IN OBSERVATION NAME/$OBSNAME ($ANTENNA)/g" -e "s/RA/$RA/g" -e "s/DEC/$DEC/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" -e "s/FILL IN DESCRIPTION/Obs $OBJECT_LONG_BEAM at $START for $TIME min/g" -e "s/RDEG/$RA_DEG/g" -e "s/DDEG/$DEC_DEG/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" -e "s/LENGTH/$DURATION/g" -e "s/FILL IN TIMESTAMP/$date/g" -e "s/SUBBANDS/$SUBBANDS/g" -e "s/STATION_LIST/$STATION_LIST/g" -e "s/PULSAR/$PULSAR/g" -e "s/CHANNELS PER SUBBAND/$CHAN_SUBS/g" -e "s/INTEG STEPS/$STEPS/g" -e "s/PROJECT NAME/$PROJECT/g" -e "s/IMAGING/$IM_TF/g" -e "s/ANTENNA SETTING/$ANTENNA_SETTING/g" -e "s/BEAM NUMBER/$beam_counter/g" -e "s/INSTRUMENT FILTER/$INSTRUMENT_FILTER/g" -e "s/INTEG INTERVAL/$INTERVAL/g" $beam_xml >> $outfile
	           sed -e "s/FILL IN OBSERVATION NAME/$OBSNAME/g" -e "s/RA/$RA/g" -e "s/DEC/$DEC/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" -e "s/FILL IN DESCRIPTION/Obs $OBJECT_LONG_BEAM at $START for $TIME min/g" -e "s/RDEG/$RA_DEG/g" -e "s/DDEG/$DEC_DEG/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" -e "s/LENGTH/$DURATION/g" -e "s/FILL IN TIMESTAMP/$date/g" -e "s/SUBBANDS/$SUBBANDS/g" -e "s/STATION_LIST/$STATION_LIST/g" -e "s/PULSAR/$PULSAR/g" -e "s/CHANNELS PER SUBBAND/$CHAN_SUBS/g" -e "s/INTEG STEPS/$STEPS/g" -e "s/PROJECT NAME/$PROJECT/g" -e "s/IMAGING/$IM_TF/g" -e "s/ANTENNA SETTING/$ANTENNA_SETTING/g" -e "s/BEAM NUMBER/$beam_counter/g" -e "s/INSTRUMENT FILTER/$INSTRUMENT_FILTER/g" -e "s/INTEG INTERVAL/$INTERVAL/g" $beam_xml >> $outfile
	        elif [ $INSWITCH == 2 ] && [ $skip == 0 ]
	        then
	           sed -e "s/FILL IN OBSERVATION NAME/$OBSNAME/g" -e "s/RA/$RA/g" -e "s/DEC/$DEC/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" -e "s/FILL IN DESCRIPTION/$OBJECT_LONG_BEAM at $START for $TIME min/g" -e "s/RDEG/$RA_DEG/g" -e "s/DDEG/$DEC_DEG/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" -e "s/LENGTH/$DURATION/g" -e "s/FILL IN TIMESTAMP/$date/g" -e "s/SUBBANDS/$SUBBANDS/g" -e "s/STATION_LIST/$STATION_LIST/g" -e "s/OBJECT/$OBJECT/g" -e "s/PROJECT NAME/$PROJECT/g" -e "s/ANTENNA SETTING/$ANTENNA_SETTING/g" -e "s/INSTRUMENT FILTER/$INSTRUMENT_FILTER/g" -e "s/INTEG INTERVAL/$INTERVAL/g" -e "s/INTEG STEPS/$STEPS/g" -e "s/TARGET NAME/$OBJECT/g" -e "s/BEAM NUMBER/$beam_counter/g" -e "s/CHANNELS PER SUBBAND/$CHAN_SUBS/g " $beam_xml >> $outfile
	        fi
	        
	        if (( $MULTI == 1 ))  && (( $nbeams > 1 ))
	        then	        
	           echo "Finished working on Beam #$beam"
	        fi
	        ((beam = $beam + 1))
       done # end loop over beams
       
       if (( $skip == 0 ))
       then
		   echo "   </children>" >> $outfile
	       echo "" >> $outfile
	       echo "</lofar:observation>" >> $outfile
	       echo "</item>" >> $outfile
	   fi
   fi 
 else
    echo "WARNING: it appears that there is a blank line in your input file;  ignoring blank line"
 fi  
done < $infile

# close the XML file with ending
echo "</children>" >> $outfile
echo "</lofar:project>"  >> $outfile

# clean up the tmp files
if [ $NAMECOL == 1 ]
then
   rm /tmp/$$xmltmp_namecol /tmp/$$xmltmp_datacol
fi

if (( $user_cat == 1 ))
then
   rm /tmp/$$_combinedcat.txt
fi

echo ""
echo "DONE: output template is: $outfile" 

exit 0
