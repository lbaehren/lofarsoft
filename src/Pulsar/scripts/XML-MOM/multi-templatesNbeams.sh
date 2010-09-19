#!/bin/ksh 

# take a list of observations, and create multiple templates for MOM upload (Imaging ONLY)
# required input: list of object names or ra/dec positions
# output is an XML file which can be uploaded into MOM (if you have ADMIN privileges)

USAGE1="\nUsage for Imaging: $0 [[-help IM]] \n"\
"       -in observation_list_file -inswitch IM -intype source_or_position \n"\
"       -out template_output_file -project project_name [-stations stations_list]   \n"\
"       [-start obs_start] [-time duration] [-gap duration] [-lst|-LST] \n"\
"       [-subsHBA subband_range] [-subsLBA subband_range] [-interation integration_interval] \n"\
"       [-antenna antenna_setup]  [-modeHBA antenna_submode] [-modeLBA antenna_submode] [-multi] \n"

USAGE2="\nUsage for BeamFormed: $0 [[-help BF]] \n"\
"       -in observation_list_file -inswitch BF -intype source_or_position \n"\
"       -out template_output_file -project project_name [-st stations_list]   \n"\
"       [-start obs_start] [-time duration] [-gap duration] [-lst|-LST] \n"\
"       [-subsHBA subband_range] [-subsLBA subband_range] [-interation integration_interval] \n"\
"       [-antenna antenna_setup]  [-modeHBA antenna_submode] [-modeLBA antenna_submode] [-multi] \n"\
"       [-IM list_or_ALL] [-chansubsHBA channels_per_subband_HBA] [-chansubsLBA channels_per_subband_LBA] \n"\
"       [-integstepsHBA integration_steps_HBA] [-integstepsLBA integration_steps_LBA] \n"

USAGE3="Options: \n"\
"         -in observation_list_file ==> Specify the ascii file with observation listing (i.e. in.txt) \n"\
"         -inswitch BF_or_IM   ==>  Switch between 'BF' (Beam-Formed) or 'IM' (IMAGING) type of default obs \n"\
"         -intype source_or_position ==> Input file contains 'source'-based or 'position'-based input\n"\
"         -project project_name   ==> Name of the MOM project (usually 'Pulsars' or 'MSSS')\n"\
"         -out template_output_file ==> Specify the name of the output XML template file (i.e. out.xml) \n"         
USAGE4="         [[+multi]] ==> Turns on the multi-beam input specification;  otherwise beam separator is ignored in input file.\n"\
"         [[-LST ]]  ==> This flags the input start time (command line or file) as LST, otherwise UT is assumed.\n"\
"         [[-stations stations_list]] ==> Comma separated list of stations (i.e. CS001,CS002,RS405) \n"\
"         [[-antenna HBA_or_LBA]] ==> The antenna name - HBA, HBAHigh, HBALow or LBA (default = HBA (BF); HBAHigh (IM)) \n"\
"         [[-subsHBA subband_range]] ==> The subband range (default = '200..447') \n"\
"         [[-subsLBA subband_range]] ==> The subband range (default = '154..401') \n"\
"         [[-chansubsHBA channels_per_subband_HBA]] ==> The channels per subband for HBA (default = 16) \n"\
"         [[-chansubsLBA channels_per_subband_LBA]] ==> The channels per subband for LBA (default = 16) \n"\
"         [[-integstepsHBA integration_steps_HBA]] ==> The integration steps for HBA (default = 16) \n"\
"         [[-integstepsLBA integration_steps_LBA]] ==> The integration steps for LBA (default = 16) \n"\
"         [[-gap duration]] ==> The time between ALL observations in minutes (default = 3) \n"\
"         [[+IM list_or_ALL]] ==> Turn on Imaging with BF observations;  'ALL' or row-number-list '2,4,5' (rows start at #1)\n"\
"         [[+BF list_or_ALL]] ==> Turn on BF with Imaging observations;  'ALL' or row-number-list '2,4,5' (rows start at #1)\n"\
"         [[-modeHBA antenna_submode]] ==> The HBA antenna sub-mode (Zero, One (default), Dual, Joined)\n"\
"         [[-modeLBA antenna_submode]] ==> The LBA antenna sub-mode (Outer (default), Inner, Sparse Even, Sparse Odd, X, Y)\n"

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
SUBBANDS_HBA="200..447"
user_subbands_hba=0
SUBBANDS_LBA="154..401"
user_subbands_lba=0
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
user_chan_subs_hba=0
CHAN_SUBS_LBA=16
user_chan_subs_lba=0
CHAN_SUBS=0
STEPS_HBA=16
user_steps_hba=0
STEPS_LBA=16
user_steps_lba=0
STEPS=0
LST=0
LST_DIFF=0
PROJECT="Pulsars"
user_project=0
INTEGRATION=3
user_integration=0
IM=0
IM_LIST=""
BF=0
BF_LIST=""
modeHBA="Dual"
user_modeHBA=0
modeLBA="Outer"
user_modeLBA=0
HELP=""
user_help=0
MULTI=0
user_multi=0

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
     -antenna)           ANTENNA=$2; user_antenna=1; shift;;
     -modeHBA)           modeHBA=$2; user_modeHBA=1; shift;;
     -modeLBA)           modeLBA=$2; user_modeLBA=1; shift;;
     -chansubsHBA)       CHAN_SUBS_HBA=$2; user_chan_subs_hba=1; shift;;
     -chansubsLBA)       CHAN_SUBS_LBA=$2; user_chan_subs_lba=1; shift;;
     -integstepsHBA)     STEPS_HBA=$2; user_steps_hba=1; shift;;
     -integstepsLBA)     STEPS_LBA=$2; user_steps_lba=1; shift;;
     -stations)          STATIONS=$2; user_stations=1; shift;;
     -project)           PROJECT=$2; user_project=1; shift;;
     +IM)                IM=1; IM_LIST=$2; shift;;
     +BF)                BF=1; BF_LIST=$2; shift;;
     -integration)       INTEGRATION=$2; user_integration=1; shift;;
     -help)              HELP=$2; user_help=1; shift;;
     -lst|-LST)          LST=1;;
     +multi)             MULTI=1; user_multi=1;;
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
if [ user_infile == 0 ]
then
   echo "ERROR: Input file name is required with '-in' argument."
   exit 1
fi

if [ user_outfile == 0 ]
then
   echo "ERROR: Output file name is required with '-out' argument."
   exit 1
fi

if [ user_project == 0 ]
then
   echo "ERROR: Project name must be specified with '-project' argument and must match exactly with MOM."
   exit 1
fi

if [ user_intype == 0 ]
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

if [ user_inswitch == 0 ]
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
ncols=`grep -v "#" $infile | grep -v '^$' | tail -1 | awk -F";" '{print $1}' | wc -w`
echo "Your input file $infile contains $ncols columns"

if [ $INSWITCH == 1 ] # BF
then
	if (( $ncols == 8 ))
	then
	   if (( $user_subbands_hba != 0 ))
	   then
	      echo "WARNING: ignoring user-specified command line HBA subbands setting;  subbands from observation list will be used."
	      user_subbands_hba=0
	   fi
	   if (( $user_subbands_lba != 0 ))
	   then
	      echo "WARNING: ignoring user-specified command line LBA subbands setting;  subbands from observation list will be used."
	      user_subbands_hba=0
	   fi
    fi
	if (( $ncols == 7 ))
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
	   if (( $user_steps_hba == 1 ))
	   then
	      echo "WARNING: ignoring user-specified integration steps HBA setting;  input file setting will be used."
	      user_steps_hba=0
	   fi
	   if (( $user_steps_lba == 1 ))
	   then
	      echo "WARNING: ignoring user-specified integration steps LBA setting;  input file setting will be used."
	      user_steps_lba=0
	   fi
	fi
	
	if (( $ncols >= 2 ))
	then
	   if (( $user_antenna == 1 ))
	   then
	      echo "WARNING: ignoring user-specified antenna setting;  input file antenna setting will be used."
	      user_antenna=0
	   fi
	fi
	
	if (( $ncols == 1 ))
	then
	   type=4
	elif (( $ncols != 1 )) && (( $ncols != 2 )) && (( $ncols != 4 )) && (( $ncols != 7 )) && (( $ncols != 8 ))
	then
	   echo "ERROR: Input file must have 8, 7, 4, 2, or 1 columns;  your file contains $ncols columns which is not allowed."
	   echo "       Please refer to the script usage for input file options (type > $0) by itself."
	   exit 1
	fi
	
	if (( $ncols <= 4 )) 
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
	
	if (( $ncols == 1 )) 
	then
       # set the default antenna to HBA for BF when user has not specified one
	   if [[ $user_antenna == 0 ]]
	   then
	       ANTENNA="HBA"
       fi

	   # check that the user has specified optional params
	   if [[ $ANTENNA == "HBA" ]]
	   then
	       CHAN_SUBS=$CHAN_SUBS_HBA
	       STEPS=$STEPS_HBA
	   fi
	   if [[ $ANTENNA == "LBA" ]]
	   then
	       CHAN_SUBS=$CHAN_SUBS_LBA
	       STEPS=$STEPS_LBA
	   fi
	fi   

else ###### Imaging column check

	if  (( $ncols >= 5 )) 
	then
	   if (( $user_subbands_hba != 0 ))
	   then
	      echo "WARNING: ignoring user-specified command line HBA subbands setting;  subbands from observation list will be used."
	      user_subbands_hba=0
	   fi
	   if (( $user_subbands_lba != 0 ))
	   then
	      echo "WARNING: ignoring user-specified command line LBA subbands setting;  subbands from observation list will be used."
	      user_subbands_hba=0
	   fi
    fi

	if (( $ncols >= 5 )) 
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
	
	
	if (( $ncols >= 3 )) || (( (( $ncols == 2 )) && (( $INTYPE == 1 )) ))
	then
	   if (( $user_antenna == 1 ))
	   then
	      echo "WARNING: ignoring user-specified antenna setting;  input file antenna setting will be used."
	      user_antenna=0
	   fi
	fi
	
	if (( $ncols <= 3 )) 
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
	
	if (( $ncols == 1 )) || (( (( $ncols == 2 )) && (( $INTYPE == 2 )) ))
	then
	   # check that the user has specified optional params
	   if [[ $ANTENNA == "HBALow" ]]
	   then
	       ANT_SHORT=HBA
	   elif [[ $ANTENNA == "HBAHigh" ]]
	   then
	       ANT_SHORT=HBA
	   elif [[ $ANTENNA == "LBA" ]]
	   then
	       ANT_SHORT=LBA
	   else
	      echo "ERROR: Antenna must be set to any of these three values: HBALow, HBAHigh or LBA"
	      exit 1
	   fi
	fi   

fi # end of input file column checks

##################################################
# If LST was set by the user, then calculate the time difference 
if (( $LST == 1 )) && (( user_start == 1 ))
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
# For IM processing, check if +BF was turned on;  create the bf number list
if [ $BF == 1 ] && [ $INSWITCH == 2 ]
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
			    if [ $INSWITCH == 1 ] # BF
			    then				    
				    PULSAR=`echo $line | awk '{print $1}'`
				    OBJECT=$PULSAR
				    
				    if (( $ncols_beam == 2 ))
				    then
				        SUBBANDS=`echo $line | awk '{print $2}'`
				        SUBBANDS_SET=1
				    fi
                 else
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
                 fi
            else # when beam == 1
		        # For BF data, check which columns are in the dataset
			    if [ $INSWITCH == 1 ] # BF
			    then
				    PULSAR=`echo $line | awk '{print $1}'`
				    OBJECT=$PULSAR
				    
				    if (( $ncols >= 2 ))
				    then
				        ANTENNA=`echo $line | awk '{print $2}'`
						if [[ $ANTENNA != "HBA" ]] && [[ $ANTENNA != "LBA" ]]
						then 
						   echo "ERROR: ANTENNA setting $ANTENNA is unrecognized (must be 'HBA' or 'LBA')."
						   exit 1
						fi
				    fi
			
				    if (( $ncols == 2 ))
				    then
						if [[ $ANTENNA == "HBA" ]] 
						then 
					        CHAN_SUBS=$CHAN_SUBS_HBA
					        STEPS=$STEPS_HBA
						else
					        CHAN_SUBS=$CHAN_SUBS_LBA
					        STEPS=$STEPS_LBA
						fi
				    fi
			
				    if (( $ncols >= 4 ))
				    then
				        CHAN_SUBS=`echo $line | awk '{print $3}'`
				        STEPS=`echo $line | awk '{print $4}'`
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
				    
				    if [[ $ANTENNA == "HBA" ]]
				    then
				        ANT_SHORT=HBA
				    elif [[ $ANTENNA == "LBA" ]]
				    then
				        ANT_SHORT=LBA
				    else
				       echo "ERROR: Antenna must be set to any of these three values: HBA or LBA"
				       exit 1
				    fi
		        else # IM
			        # when input table contains object-names
			        if (( $INTYPE == 1))
			        then
				       OBJECT=`echo $line | awk '{print $1}'`
				       if (( $ncols >= 2 ))
				       then
				           ANTENNA=`echo $line | awk '{print $2}'`
		
						   if [[ $ANTENNA == "HBALow" ]]
						   then
						       ANT_SHORT=HBA
						   elif [[ $ANTENNA == "HBAHigh" ]]
						   then
						       ANT_SHORT=HBA
						   elif [[ $ANTENNA == "LBA" ]]
						   then
						       ANT_SHORT=LBA
						   else
						      echo "ERROR: Antenna must be set to any of these three values: HBALow, HBSHigh or LBA"
						      exit 1
						   fi
			           fi
				       if (( $ncols >= 5 ))
				       then
					        START=`echo $line | awk '{print $3}'`
					        TIME=`echo $line | awk '{print $4}'`
					        STATIONS=`echo $line | awk '{print $5}'`                
				
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
				       if (( $ncols == 6 ))
				       then
					        SUBBANDS=`echo $line | awk '{print $6}'`
					        SUBBANDS_SET=1
		               fi
			        # when input table contains ra and dec values
				    else
				       RA_DEG=`echo $line | awk '{print $1}'`
				       DEC_DEG=`echo $line | awk '{print $2}'`
			
				       if (( $ncols >= 3 ))
				       then
				           ANTENNA=`echo $line | awk '{print $3}'`
			
						   if [[ $ANTENNA == "HBALow" ]]
						   then
						       ANT_SHORT=HBA
						   elif [[ $ANTENNA == "HBAHigh" ]]
						   then
						       ANT_SHORT=HBA
						   elif [[ $ANTENNA == "LBA" ]]
						   then
						       ANT_SHORT=LBA
						   else
						      echo "ERROR: Antenna must be set to any of these three values: HBALow, HBSHigh or LBA"
						      exit 1
						   fi
			           fi
				       if (( $ncols >= 6 ))
				       then
					        START=`echo $line | awk '{print $4}'`
					        TIME=`echo $line | awk '{print $5}'`
					        STATIONS=`echo $line | awk '{print $6}'`                
				
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
			           if (( $ncols == 7 ))
				       then
					        SUBBANDS=`echo $line | awk '{print $7}'`
					        SUBBANDS_SET=1
		               fi
		
				    fi # end if (( $INTYPE == 1))
		        fi # end if [ $INSWITCH == 1 ] 
		
		
		        ##################################################
		        # Set up the Subbands
		        if [ $SUBBANDS_SET == 0 ]
		        then
					if [ $user_subbands_hba == 0 ] && [ $ANTENNA == "HBA" ] && [ $INSWITCH == 1 ] 
					then
					    SUBBANDS="200..447"
					elif [ $user_subbands_hba == 0 ] && [ $ANTENNA == "HBAHigh" ] && [ $INSWITCH == 2 ] 
					then
					    SUBBANDS="77..324"
			        elif ( [ $user_subbands_hba == 1 ] && [ $ANTENNA == "HBA" ] && [ $INSWITCH == 1 ] ) || ( [ $user_subbands_hba == 1 ] && [ $ANTENNA == "HBAHigh" ] && [ $INSWITCH == 2 ] )
			        then
			            needs_expand=`echo $SUBBANDS_HBA | grep "," | grep ".."`
			            if [ $needs_expand != "" ]
			            then
			               SUBBANDS=`echo "$SUBBANDS_HBA" | expand_sblist.py`
			            else
					       SUBBANDS=$SUBBANDS_HBA
					    fi
			        elif [ $user_subbands_hba == 0 ] && [ $ANTENNA == "HBALow" ] && [ $INSWITCH == 2 ] 
			        then
					    SUBBANDS="54..301"
			        elif [ $user_subbands_hba == 1 ] && [ $ANTENNA == "HBALow" ] && [ $INSWITCH == 2 ] 
			        then
					    needs_expand=`echo $SUBBANDS_HBA | grep "," | grep ".."`
			            if [ $needs_expand != "" ]
			            then
			               SUBBANDS=`echo "$SUBBANDS_HBA" | expand_sblist.py`
			            else
					       SUBBANDS=$SUBBANDS_HBA
					    fi
					elif [ $user_subbands_lba == 0 ] && [ $ANTENNA == "LBA" ] 
					then
					    SUBBANDS="154..401"
			        elif [ $user_subbands_lba == 1 ] && [ $ANTENNA == "LBA" ] 
			        then
			            needs_expand=`echo $SUBBANDS_LBA | grep "," | grep ".."`
			            if [ $needs_expand != "" ]
			            then
			               SUBBANDS=`echo "$SUBBANDS_LBA" | expand_sblist.py`
			            else
					       SUBBANDS=$SUBBANDS_LBA
					    fi
				    fi   
			    fi # end if [ $SUBBANDS_SET == 0 ]
		    fi # end if (( $beam > 1 ))
		    
		    ##################################################
		    # If multi-beam obs, split up subbands by N beams (TBD) if subbands > 248
		    if (( $MULTI == 1 )) && (( $beam == 1 )) && (( $nbeams > 1 )) && (( $SUBBANDS_SET == 0 ))
		    then
		       total_subbands=`echo $SUBBANDS | expand_sblist.py | awk -F"," '{print NF}'`
		       if (( $total_subbands >= 248 ))
		       then
		          echo "WARNING: Multi-beam specification must have total subbands <= 248;"
		          echo "         Input has $total_subbands;  splitting subbands by $nbeams beams"
		          split_subs=`echo $total_subbands / $nbeams | bc`
		       fi
		       SUBBANDS_ORIG=`echo $SUBBANDS | expand_sblist.py`
		    fi
		    if (( $MULTI == 1 )) && (( $SUBBANDS_SET == 0 )) && (( $nbeams > 1 ))
		    then
		          if (( $beam == 1 ))
		          then
		             min_range=1
		             max_range=$split_subs
		          else
		             min_range=`echo "$max_range + 1" | bc`
		             max_range=`echo "$min_range + $split_subs - 1" | bc`
		          fi
		          SUBBANDS=`echo $SUBBANDS_ORIG | cut -d"," -f$min_range-$max_range`
            fi
            
		    #make sure station list has all capital letters
		    STATIONS=`echo $STATIONS | sed 's/a-z/A-Z/g'`                
		                   
		    if (( $INTYPE == 1 ))
		    then
			    found_OBJECT=`grep -i "$OBJECT " $catalog`
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
			    RA_DEG=`grep -i $OBJECT $catalog | awk '{print $1}'`
			    DEC_DEG=`grep -i $OBJECT $catalog | awk '{print $2}'`
	        elif (( $INTYPE == 1 )) && (( $INSWITCH == 2 ))
	        then
		        RA_DEG=`grep -i "$OBJECT " $catalog | awk '{print $1}'`
		        DEC_DEG=`grep -i "$OBJECT " $catalog | awk '{print $2}'`
		    fi
		    if [[ $RA_DEG == "" ]] || [[ $DEC_DEG == "" ]] 
		    then
		       echo "WARNING: unable to find position for OBJECT $OBJECT in position catalog ($catalog)."
		       echo "WARNING: skipping OBJECT $OBJECT from template creation."
		       continue
		    fi
	        #convert to radians	   
			RA=`echo "scale=10; $RA_DEG * $pi / 180.0" | bc | awk '{printf("%1.9f\n",$1)}'`
			DEC=`echo "scale=10; $DEC_DEG * $pi / 180.0" | bc | awk '{printf("%1.9f\n",$1)}'`
					
	        ##################################################
	        # Calculate start/stop related values
	        # Break up into time less than one minute or >= one minute
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
	
	        ##################################################
	        # Antenna related setttings, depending on input
		    if [ $ANTENNA == "HBAHigh" ]
		    then
		       INSTRUMENT_FILTER="210-250"
		       if [ $user_modeHBA == 0 ]
		       then
		          ANTENNA_SETTING="HBA Dual"
		       else
		          if [ $modeHBA == "Zero" ] || [ $modeHBA == "One" ] || [ $modeHBA == "Dual" ] || [ $modeHBA == "Joined" ]
		          then
		             ANTENNA_SETTING="HBA $modeHBA"
		          else 
		             echo "WARNING: user antenna mode HBA $modeHBA is not recognized, using defaul HBA Dual"
		             ANTENNA_SETTING="HBA Dual"
		          fi
		       fi
		    elif [ $ANTENNA == "HBALow" ] || [ $ANTENNA == "HBA" ]
		    then
		       INSTRUMENT_FILTER="110-190"
		       if [ $user_modeHBA == 0 ] && [ $INSWITCH == 1 ]
		       then
		          ANTENNA_SETTING="HBA Zero"
		       elif [ $user_modeHBA == 0 ] && [ $INSWITCH == 2 ]
		       then
		          ANTENNA_SETTING="HBA Dual"
		       else
		          if [ $modeHBA == "Zero" ] || [ $modeHBA == "One" ] || [ $modeHBA == "Dual" ] || [ $modeHBA == "Joined" ]
		          then
		             ANTENNA_SETTING="HBA $modeHBA"
		          else 
		             if [ $INSWITCH == 1 ]
		             then
		                echo "WARNING: user antenna mode HBA $modeHBA is not recognized, using defaul HBA Zero"
		                ANTENNA_SETTING="HBA Zero"
		             else
		                echo "WARNING: user antenna mode HBA $modeHBA is not recognized, using defaul HBA Dual"
		                ANTENNA_SETTING="HBA Dual"
		             fi
		          fi
		       fi
		    elif [ $ANTENNA == "LBA" ]
		    then
		       INSTRUMENT_FILTER="30-90"
	           ANTENNA_SETTING="LBA Outer"
		       if [ $modeLBA == "Outer" ] || [ $modeLBA == "Inner" ] || [ $modeLBA == "Sparse Even" ] || [ $modeLBA == "Sparse Odd" ] || [ $modeLBA == "X" ] || [ $modeLBA == "Y" ]
		       then
		          ANTENNA_SETTING="LBA $modeLBA"
		       else 
		             echo "WARNING: user antenna mode LBA $modeLBA is not recognized, using defaul LBA Outer"
		             ANTENNA_SETTING="LBA Outer"
		       fi
		    fi
			
			if [ $user_integration == 0 ] && [ $INSWITCH == 2 ]
			then
			   if [ $ANT_SHORT == "HBA" ]
			   then
			       INGETRATION=3
			   else
			       INGETRATION=1
			   fi
			elif [ $user_integration == 0 ] && [ $INSWITCH == 2 ]
			then
			       INGETRATION=1
			fi
			if (( $INTEGRATION < 1))
			then
			    echo "ERROR: integration time of $INTEGRATION is invalid; must be >= 1."
			    exit 1
			fi

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
			   OBJECT="$OBJECT ($ANTENNA)"
			   if [ $nbeams == 1 ]
			   then
			      OBJECT_LONG="$OBJECT"
			      OBJECT_LONG_BEAM="$OBJECT"
			   else
			      OBJECT_LONG="$OBJECT Multi-Beam"
			      OBJECT_LONG_BEAM="$OBJECT Multi-Beam #$beam"
			   fi
			else
			   if [ $nbeams == 1 ]
			   then
			      OBJECT="Pos $RA_DEG $DEC_DEG"
			      OBJECT_LONG="Pos $RA_DEG $DEC_DEG ($ANTENNA)"
			      OBJECT_LONG_BEAM="Pos $RA_DEG $DEC_DEG ($ANTENNA)"
			   else
			      OBJECT="Pos $RA_DEG $DEC_DEG"
			      OBJECT_LONG="Pos $RA_DEG $DEC_DEG ($ANTENNA) Multi-Beam"
			      OBJECT_LONG_BEAM="Pos $RA_DEG $DEC_DEG ($ANTENNA) Multi-Beam #$beam"
			   fi
			fi
			echo "RA Pointing = $RA_DEG deg"
			echo "DEC Pointing = $DEC_DEG deg"
			echo "RA Pointing = $RA radians"
			echo "DEC Pointing = $DEC radians"
		    echo "ANTENNA = $ANTENNA"
		    echo "ANTENNA short name = $ANT_SHORT"
		    echo "ANTENNA Setting = $ANTENNA_SETTING"
		    echo "Instrument Filter = $INSTRUMENT_FILTER"
		    echo "Integration Interval = $INTEGRATION"
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

			BF_TF=false
			if (( $BF == 1 )) 
			then
			   for jj in $BF_LIST
			   do 
			      if (( $counter == $jj ))
			      then
			         echo "Beam-Formed turned on (TRUE) for this Imaging Observation"
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
	       
	        if [ $INSWITCH == 1 ] && [ $skip = 0 ]
	        then 
	           if (( $beam_counter == 0 ))
	           then
	              sed -e "s/FILL IN OBSERVATION NAME/Obs $PULSAR ($ANTENNA)/g" -e "s/RA/$RA/g" -e "s/DEC/$DEC/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" -e "s/FILL IN DESCRIPTION/Obs $OBJECT_LONG at $START for $TIME min/g" -e "s/RDEG/$RA_DEG/g" -e "s/DDEG/$DEC_DEG/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" -e "s/LENGTH/$DURATION/g" -e "s/FILL IN TIMESTAMP/$date/g" -e "s/SUBBANDS/$SUBBANDS/g" -e "s/STATION_LIST/$STATION_LIST/g" -e "s/PULSAR/$PULSAR/g" -e "s/CHANNELS PER SUBBAND/$CHAN_SUBS/g" -e "s/INTEG STEPS/$STEPS/g" -e "s/PROJECT NAME/$PROJECT/g" -e "s/IMAGING/$IM_TF/g" -e "s/ANTENNA SETTING/$ANTENNA_SETTING/g" -e "s/INSTRUMENT FILTER/$INSTRUMENT_FILTER/g" $middle >> $outfile
	           fi
	        elif [ $INSWITCH == 2 ]  && [ $skip = 0 ]
	        then
	           if (( $beam_counter == 0 ))
	           then
	              sed -e "s/FILL IN OBSERVATION NAME/$OBJECT/g" -e "s/RA/$RA/g" -e "s/DEC/$DEC/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" -e "s/FILL IN DESCRIPTION/$OBJECT_LONG at $START for $TIME min/g" -e "s/RDEG/$RA_DEG/g" -e "s/DDEG/$DEC_DEG/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" -e "s/LENGTH/$DURATION/g" -e "s/FILL IN TIMESTAMP/$date/g" -e "s/SUBBANDS/$SUBBANDS/g" -e "s/STATION_LIST/$STATION_LIST/g" -e "s/OBJECT/$OBJECT/g" -e "s/PROJECT NAME/$PROJECT/g" -e "s/ANTENNA SETTING/$ANTENNA_SETTING/g" -e "s/INSTRUMENT FILTER/$INSTRUMENT_FILTER/g" -e "s/INTEG INTERVAL/$INTEGRATION/g" -e "s/TARGET NAME/$OBJECT/g" -e "s/INCOHERENTSTOKES/$BF_TF/g" $middle >> $outfile
	           fi
	        fi 
		        
	        if [ $INSWITCH == 1 ] && [ $skip = 0 ]
	        then 
	           sed -e "s/FILL IN OBSERVATION NAME/Obs $PULSAR ($ANTENNA)/g" -e "s/RA/$RA/g" -e "s/DEC/$DEC/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" -e "s/FILL IN DESCRIPTION/Obs $OBJECT_LONG_BEAM at $START for $TIME min/g" -e "s/RDEG/$RA_DEG/g" -e "s/DDEG/$DEC_DEG/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" -e "s/LENGTH/$DURATION/g" -e "s/FILL IN TIMESTAMP/$date/g" -e "s/SUBBANDS/$SUBBANDS/g" -e "s/STATION_LIST/$STATION_LIST/g" -e "s/PULSAR/$PULSAR/g" -e "s/CHANNELS PER SUBBAND/$CHAN_SUBS/g" -e "s/INTEG STEPS/$STEPS/g" -e "s/PROJECT NAME/$PROJECT/g" -e "s/IMAGING/$IM_TF/g" -e "s/ANTENNA SETTING/$ANTENNA_SETTING/g" -e "s/BEAM NUMBER/$beam_counter/g" -e "s/INSTRUMENT FILTER/$INSTRUMENT_FILTER/g" $beam_xml >> $outfile
	        elif [ $INSWITCH == 2 ] && [ $skip = 0 ]
	        then
	           sed -e "s/FILL IN OBSERVATION NAME/$OBJECT/g" -e "s/RA/$RA/g" -e "s/DEC/$DEC/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" -e "s/FILL IN DESCRIPTION/$OBJECT_LONG_BEAM at $START for $TIME min/g" -e "s/RDEG/$RA_DEG/g" -e "s/DDEG/$DEC_DEG/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" -e "s/LENGTH/$DURATION/g" -e "s/FILL IN TIMESTAMP/$date/g" -e "s/SUBBANDS/$SUBBANDS/g" -e "s/STATION_LIST/$STATION_LIST/g" -e "s/OBJECT/$OBJECT/g" -e "s/PROJECT NAME/$PROJECT/g" -e "s/ANTENNA SETTING/$ANTENNA_SETTING/g" -e "s/INSTRUMENT FILTER/$INSTRUMENT_FILTER/g" -e "s/INTEG INTERVAL/$INTEGRATION/g" -e "s/TARGET NAME/$OBJECT/g" -e "s/BEAM NUMBER/$beam_counter/g" $beam_xml >> $outfile
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
echo ""
echo "DONE: output template is: $outfile" 

exit 0
