#!/bin/ksh 

# take a list of observations, and create multiple templates for MOM upload (Imaging ONLY)
# required input: list of object names or ra/dec positions
# output is an XML file which can be uploaded into MOM (if you have ADMIN privileges)

USAGE1="\nusage : $0 -in observation_list_file -intype source_or_position -out template_output_file -project project_name [-start obs_start] [-time duration] [-st stations_list] [[-lst|-LST]] [[-subsHBA subband_range]] [[-subsLBA subband_range]] [[-gap duration]] [[-antenna antenna_setup]]\n\n"\
"      This script is run using several different methodologies which are explained below: \n"\
"         \n"\
"      1) The input file contains all the information: object-name/position  antenna  obs-start  duration  station-list \n"\
"         In this case, you only need to specify the input file and type, output file and the project name.:\n"\
"         \n"\
"         -in observation_list_file ==> Specify the ascii file with observation listing (i.e. in.txt) \n"\
"         -intype source_or_position ==> The input file type is either `source` based or `position` based \n"\
"         -out template_output_file ==> Specify the name of the output XML template file (i.e. out.xml) \n"\
"         -project project_name  ==> The name of the project exactly as it appears in MOM (including capital letters) \n"\
"         \n"\
"         Optionally you can also specify the following parameters:\n"\
"         [[-LST ]]  ==> This flags the input start time (command line or file) as LST, otherwise UT is assumed.\n"\
"         [[-subsHBA subband_range]] ==> The HBA subband range (default = '200..447') \n"\
"         [[-subsLBA subband_range]] ==> The LBA subband range (default = '154..401') \n"\
"         \n"\
"         Example Type I input file using source names:\n"\
"         # object     antenna           start-time           duration-mins     stations\n"\
"         3C14            HBA-high       2010-07-22T14:50:00     10           CS001,CS002,CS003,CS004,CS005,CS006,CS007\n"\
"         3C14            LBA            2010-07-22T15:03:00     20           CS001,CS002,CS003,CS004,CS005,CS006,CS007\n"\
"         3C99            HBA-low        2010-07-22T15:15:30     10           CS001,CS002,CS003,CS004,CS005,CS006,CS007\n"\
"         3C83.1          LBA            2010-07-22T15:27:30     60           CS001,CS002,CS003,CS004,CS005,CS006,CS007\n"\
"         3C99            LBA            2010-07-22T15:39:00    120           CS001,CS002,CS003,CS004,CS005,CS006,CS007\n"\
"         3C99            HBA-high       2010-07-22T15:52:00     60           CS001,CS002,CS003,CS004,CS005,CS006,CS007\n"\
"         3C99            HBA-low        2010-07-22T16:55:00     60           CS001,CS002,CS003,CS004,CS005,CS006,CS007\n"\
"         \n"\
"         Example runs:\n"\
"         > $0 -in obs_list_names.txt -intype source -out output.xml -project LEA016\n"\
"         > $0 -in obs_list_names.txt -intype source -out output.xml -project LEA017 -subsLBA 10..45\n"\
"         \n"\
"         Example Type I input file using ra dec positions:\n"\
"         # ra     dec           antenna           start-time           duration-mins     stations\n"\
"         13.98   +68.487         HBA-high       2010-07-22T14:50:00     10           CS001,CS002,CS003,CS004,CS005,CS006,CS007\n"\
"         13.98   +68.487         LBA            2010-07-22T15:03:00     20           CS001,CS002,CS003,CS004,CS005,CS006,CS007\n"\
"         70.498  +1.728          HBA-low        2010-07-22T15:15:30     10           CS001,CS002,CS003,CS004,CS005,CS006,CS007\n"\
"         208.074 +31.409         LBA            2010-07-22T15:27:30     60           CS001,CS002,CS003,CS004,CS005,CS006,CS007\n"\
"         217.17   -1.623         LBA            2010-07-22T15:39:00    120           CS001,CS002,CS003,CS004,CS005,CS006,CS007\n"\
"         217.17   -1.623         HBA-high       2010-07-22T15:52:00     60           CS001,CS002,CS003,CS004,CS005,CS006,CS007\n"\
"         217.17   -1.623         HBA-low        2010-07-22T16:55:00     60           CS001,CS002,CS003,CS004,CS005,CS006,CS007\n"\
"         \n"\
"         Example runs:\n"\
"         > $0 -in obs_list_pos.txt -intype position -out output.xml -project LEA016\n"\
"         > $0 -in obs_list_pos.txt -intype position -out output.xml -project LEA017 -subsLBA 10..45 -subsHBA 200..245\n"\
"         \n"
USAGE2="      2) The input file contains just: object-name/position  antenna \n"\
"         You must specify the input file name and type, output file name and project name. \n"\
"         In this case, you MUST also specify the following parameters on the command line as input...\n"\
"         Note, these parameters are used for the entire list;  start time of subsequent observations\n"\
"         is calculated based on observation duration and gap between observations.\n"\
"         \n"\
"         [-start obs_start] ==> The start date/time for 1st source in the list of observations; note format; supercede's infile values (i.e. 2010-07-22T14:50:00) \n"\
"         [-time duration] ==> The duration of ALL observations in the list, in minutes; supercede's infile values (i.e. 60) \n"\
"         [-stations stations_list] ==> The list of LOFAR stations, supercede's infile values and used for all obs (i.e. 'CS001,CS002') \n"\
"         \n"\
"         Optionally you can also specify the following parameters:\n"\
"         [[-LST ]]  ==> This flags the input start time (command line or file) as LST, otherwise UT is assumed.\n"\
"         [[-subsHBA subband_range]] ==> The subband range (default = '200..447') \n"\
"         [[-subsLBA subband_range]] ==> The subband range (default = '154..401') \n"\
"         [[-gap duration]] ==> The time between ALL observations in minutes (default = 3) \n"\
"         \n"\
"         Example Type II input file using source names:\n"\
"         # object       antenna\n"\
"         3C99           LBA \n"\
"         3C99           HBA-low \n"\
"         3C327          HBA-low \n"\
"         \n"\
"         Example runs:\n"\
"         > $0 -in obs_list_names.txt -intype source -out output.xml -project LEA016 -start 2010-07-22T14:00:00 -time 33 -stations CS001,CS002,CS003,CS004\n"\
"         > $0 -in obs_list_names.txt -intype source -out output.xml -project LEA116 -start 2010-07-22T14:50:00 -time 10 -stations CS001,CS002 -gap 6\n"\
"         > $0 -in obs_list_names.txt -intype source -out output.xml -project LEA216 -start 2010-07-22T09:40:00 -time 25 -stations CS001 -subsLBA 10..45 \n"\
"         > $0 -in obs_list_names.txt -intype source -out output.xml -project LEA033 -start 2010-07-22T09:40:00 -time 25 -stations CS001 -subsLBA 10..45 -subsLBA 10..45  \n"\
"         \n"\
"         Example Type II input file using ra dec positions:\n"\
"         # ra     dec           antenna\n"\
"         13.98   +68.487         LBA \n"\
"         70.498  +1.728          HBA-low \n"\
"         217.17   -1.623         HBA-low \n"\
"         \n"\
"         Example runs:\n"\
"         > $0 -in obs_list_names.txt -intype position -out output.xml -project LEA016 -start 2010-07-22T14:00:00 -time 33 -stations CS001,CS002,CS003,CS004\n"\
"         > $0 -in obs_list_names.txt -intype position -out output.xml -project LEA116 -start 2010-07-22T14:50:00 -time 10 -stations CS001,CS002 -gap 6\n"\
"         > $0 -in obs_list_names.txt -intype position -out output.xml -project LEA216 -start 2010-07-22T09:40:00 -time 25 -stations CS001 -subsLBA 10..45 \n"\
"         > $0 -in obs_list_names.txt -intype position -out output.xml -project LEA033 -start 2010-07-22T09:40:00 -time 25 -stations CS001 -subsLBA 10..45 -subsLBA 10..45  \n"\
"         \n"
USAGE3="      3) The input file contains just: object-name/position. \n"\
"         You must specify the input file name and type, output file name and project name. \n"\
"         In this case, you MUST also specify the following parameters on the command line as input...\n"\
"         Note, these parameters are used for the entire list;  start time of subsequent observations\n"\
"         is calculated based on observation duration and gap between observations.\n"\
"         Only one ANTENNA is used for the entire list of input pulsars (default = HBA-high). \n"\
"         \n"
USAGE4="         \n"\
"         [-start obs_start] ==> The start date/time for 1st source in the list of observations; note format; supercede's infile values (i.e. 2010-07-22T14:50:00) \n"\
"         [-time duration] ==> The duration of ALL observations in the list, in minutes; supercede's infile values (i.e. 60) \n"\
"         [-stations stations_list] ==> The list of LOFAR stations, supercede's infile values and used for all obs (i.e. 'CS001,CS002') \n"\
"         \n"\
"         Optionally you can also specify the following parameters:\n"\
"         [[-LST ]]  ==> This flags the input start time (command line or file) as LST, otherwise UT is assumed.\n"\
"         [[-project project_name]]  ==> The name of the MOM project 'Pulsars' or 'LEA032' (default = Pulsars)\n"\
"         [[-antenna antenna_setup]] ==> The antenna name: HBA-high, HBA-low or LBA (default = HBA-high) \n"\
"         [[-subsHBA subband_range]] ==> The subband range (default = '200..447') \n"\
"         [[-subsLBA subband_range]] ==> The subband range (default = '154..401') \n"\
"         [[-gap duration]] ==> The time between ALL observations in minutes (default = 3) \n"\
"         \n"
USAGE5="         Example Type III input file using source names:\n"\
"         # object      \n"\
"         3C99   \n"\
"         3C327  \n"\
"         372.1  \n"\
"         \n"\
"         Example runs:\n"\
"         > $0 -in obs_list_names.txt -intype source -out output.xml -project LEA016 -start 2010-07-22T14:00:00 -time 33 -stations CS001,CS002,CS003,CS004 -antenna HBA-high\n"\
"         > $0 -in obs_list_names.txt -intype source -out output.xml -project LEA116 -start 2010-07-22T14:50:00 -time 10 -stations CS001,CS002 -gap 6 -antenna HBA-low\n"\
"         > $0 -in obs_list_names.txt -intype source -out output.xml -project LEA216 -start 2010-07-22T09:40:00 -time 25 -stations CS001 -subsLBA 10..45 -antenna LBA\n"\
"         \n"\
"         Example Type II input file using ra dec positions:\n"\
"         # ra     dec        \n"\
"         13.98   +68.487     \n"\
"         70.498  +1.728      \n"\
"         217.17   -1.623     \n"\
"         \n"\
"         Example runs:\n"\
"         > $0 -in obs_list_names.txt -intype position -out output.xml -project LEA016 -start 2010-07-22T14:00:00 -time 33 -stations CS001,CS002,CS003,CS004\n"\
"         > $0 -in obs_list_names.txt -intype position -out output.xml -project LEA116 -start 2010-07-22T14:50:00 -time 10 -stations CS001,CS002 -gap 6 -antenna HBA-low\n"\
"         > $0 -in obs_list_names.txt -intype position -out output.xml -project LEA216 -start 2010-07-22T09:40:00 -time 25 -stations CS001 -subsLBA 10..45 -antenna LBA \n"\
"         \n"



if [ $# -lt 8 ]                    
then
   print "$USAGE1" 
   print "$USAGE2" 
   print "$USAGE3" 
   print "$USAGE4" 
   print "$USAGE5" 
   print "$USAGE6" 
   exit 1
fi

#input parametes and switches
infile=""
user_infile=0
outfile=""
user_outfile=0
INTYPE=""
user_intype=0
SUBBANDS_HBA="200..447"
user_subbands_hba=0
SUBBANDS_LBA="154..401"
user_subbands_lba=0
SUBBANDS=""
START=""
user_start=0
previous_start=""
TIME=0
user_duration=0
STATIONS=""
user_stations=0
GAP=3
user_gap=0
ANTENNA=HBA-high
user_antenna=0
LST=0
LST_DIFF=0
PROJECT=""
user_project=0


while [ $# -gt 0 ]
do
    case "$1" in
     -in)                infile=$2; user_infile=1; shift;;
     -intype)            INTYPE=$2; user_intype=1; shift;;
     -out)               outfile=$2; user_outfile=1; shift;;
     -start)             START=$2; previous_start=$2; user_start=1; shift;;
     -time)              TIME=$2; user_duration=1; shift;;
     -gap)               GAP=$2; user_gap=1; shift;;
     -subsHBA)           SUBBANDS_HBA=$2; user_subbands_hba=1; shift;;
     -subsLBA)           SUBBANDS_LBA=$2; user_subbands_lba=1; shift;;
     -antenna)           ANTENNA=$2; user_antenna=1; shift;;
     -stations)          STATIONS=$2; user_stations=1; shift;;
     -project)           PROJECT=$2; user_project=1; shift;;
     -lst|-LST)          LST=1;;
       -*)
            print >&2 \
            "$USAGE1" \
            "$USAGE2" \
            "$USAGE3" \
            "$USAGE4" \
            "$USAGE5" \
            "$USAGE6" 
            exit 1;;
        *)  break;;     
    esac
    shift
done

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
   if [ $INTYPE != "source" ]  && [ $INTYPE != "position" ]]
   then
      echo "ERROR: Input file type must be specified with '-intype position' or '-intype source'."
      exit 1
   fi
fi

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


#The 'date' command is highly dependant on the OS;  
#BSD 'date' has more functionality than Linux/Ubuntu date;
#therefore, need to make to cases for the date-oriented calculations
system_typ=`uname -s | awk '{print $1}'`

#Find out which type of input file was given:
# (1) 7-column input
# (2) 4-column input
# (3) 2-column input
# (4) 1-column input
#depending on the type of input, check for required input parameters
ncols=0
ncols=`grep -v "#" $infile | grep -v '^$' | tail -1 | wc -w`
echo "Your input file $infile contains $ncols columns"
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
elif (( $ncols != 1 )) && (( $ncols != 2 )) && (( $ncols != 4 )) && (( $ncols != 7 ))
then
   echo "ERROR: Input file must have 7, 4, 2, or 1 columns;  your file contains $ncols columns which is not allowed."
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
      echo "ERROR: $ncols column input file requires '-st stations_list' as input"
      exit 1
   fi
fi

if (( $ncols == 1 )) 
then
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
	      new_date_seconds=`echo "date -d \"$START\" \"+%s\" + $tot_time * 60" | bc`
          previous_start=`date -d @$new_date_seconds "+%Y-%m-%dT%H:%M:%S"`
          START=$previous_start
	fi

else 
    LST_DIFF=0
fi

num_lines=0
num_lines=`grep -v "#" $infile | wc -l | awk '{print $1}'`
catalog=$LOFARSOFT/release/share/pulsar/data/PSR_catalog.txt
echo "Wokring on $infile with $num_lines sources"
echo "Using PSR catalog file $catalog"
echo "Writing to output file $outfile"

if (( $IM == 1 ))
then
   if [ $IM_LIST == "ALL" ]  || [ $IM_LIST == "all" ]
   then
      # create the full range of row numbers for the Image data
      tmp=""
      ii=1
      while (( $ii <= $num_lines ))
      do
         tmp="$tmp $ii"
         ((ii+=$ii))
      done
      IM_LIST=$tmp
   else 
      # change the commas to spaces for the row numbers which need Image data
      tmp=`echo $IM_LIST | sed 's/,/ /g'`
      IM_LIST=$tmp
   fi
fi

#create the header of the xml output file
header=$LOFARSOFT/release/share/pulsar/data/XML-template-header.txt
if [ ! -f $header ]
then
   echo "ERROR: missing USG installation of files \$LOFARSOFT/release/share/pulsar/data/XML-template-*.txt"
   echo "       Make sure you have run 'make install' in \$LOFARSOFT/build/pulsar"
   exit 1
fi 

cat $header > $outfile

middleHBA=$LOFARSOFT/release/share/pulsar/data/XML-template-singleobs_HBA.txt
middleLBA=$LOFARSOFT/release/share/pulsar/data/XML-template-singleobs_LBA.txt
if [ ! -f $middleHBA ] || [ ! -f $middleLBA ]
then
   echo "ERROR: missing USG installation of files \$LOFARSOFT/release/share/pulsar/data/XML-template-*.txt"
   echo "       Make sure you have run 'make install' in \$LOFARSOFT/build/pulsar"
   exit 1
fi 

date=`date "+%Y-%m-%dT%H:%M:%S"`
PID=$$
pi=$(echo "scale=10; 4*a(1)" | bc -l)

RA_DEG=0.0
DEC_DEG=0.0
PULSAR=""
RA=0.0
DEC=0.0

#for each source, create an observation
counter=0
while read line
do
   echo "------------------------------------------------------------------"
   is_header=`echo $line | grep "#"`
   if [[ $is_header == "" ]]
   then
        ((counter += 1))

	    PULSAR=`echo $line | awk '{print $1}'`
	    
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

	    if (( $ncols == 7 ))
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
	
		if (( $user_subbands_hba == 0 )) && [[ $ANTENNA == "HBA" ]]
		then
		    SUBBANDS="200..447"
        elif (( $user_subbands_hba == 1 )) && [[ $ANTENNA == "HBA" ]]
        then
		    SUBBANDS=$SUBBANDS_HBA
		elif (( $user_subbands_lba == 0 )) && [[ $ANTENNA == "LBA" ]]
		then
		    SUBBANDS="154..401"
        else
		    SUBBANDS=$SUBBANDS_LBA
	    fi   
	    
	    #make sure station list has all capital letters
	    STATIONS=`echo $STATIONS | sed 's/a-z/A-Z/g'`                
	                   
	    found_pulsar=`grep $PULSAR $catalog`
	    if [[ $found_pulsar == "" ]]
	    then
	       echo "WARNING: Pulsar $PULSAR not found in catalog $catalog."
	       echo "WARNING: skipping pulsar $PULSAR from template creation."
	       continue
	    fi

	    found_stations=`echo $STATIONS | egrep -i "CS|RS"`
	    if [[ $found_stations == "" ]]
	    then
	       echo "WARNING: STATION list ($STATIONS) seems invalid, does not contain CS or RS characters."
	       echo "WARNING: skipping pulsar $PULSAR from template creation."
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

	    RA_DEG=`grep -i $PULSAR $catalog | awk '{print $6}'`
	    DEC_DEG=`grep -i $PULSAR $catalog | awk '{print $7}'`
	    if [[ $RA_DEG == "" ]] || [[ $DEC_DEG == "" ]] 
	    then
	       echo "WARNING: unable to find position for pulsar $PULSAR in position catalog ($catalog)."
	       echo "WARNING: skipping pulsar $PULSAR from template creation."
	       continue
	    fi
        #convert to radians	   
		RA=`echo "scale=10; $RA_DEG * $pi / 180.0" | bc | awk '{printf("%1.9f\n",$1)}'`
		DEC=`echo "scale=10; $DEC_DEG * $pi / 180.0" | bc | awk '{printf("%1.9f\n",$1)}'`
				
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
				      new_date_seconds=`echo "date -d \"$START\" \"+%s\" + $tot_time * 60" | bc`  
			          previous_start=`date -d @$new_date_seconds "+%Y-%m-%dT%H:%M:%S"`
				fi
			elif (( $LST == 1 ))
			then
			    tot_time=$LST_DIFF
			   
				if [[ $system_typ == "Darwin" ]]
				then
			          START=`date -j -v +$tot_time"M" -f "%Y-%m-%dT%H:%M:%S" $START "+%Y-%m-%dT%H:%M:%S"`
				else
				      new_date_seconds=`echo "date -d \"$START\" \"+%s\" + $tot_time * 60" | bc`  
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
				      new_date_seconds=`echo "date -d \"$START\" \"+%s\" + $tot_time * 60" | bc`
			          previous_start=`date -d @$new_date_seconds "+%Y-%m-%dT%H:%M:%S"`
				fi
			elif (( $LST == 1 ))
			then
			    tot_time=$LST_DIFF
			   
				if [[ $system_typ == "Darwin" ]]
				then
			          START=`date -j -v +$tot_time"M" -f "%Y-%m-%dT%H:%M:%S" $START "+%Y-%m-%dT%H:%M:%S"`
				else
				      new_date_seconds=`echo "date -d \"$START\" \"+%s\" + $tot_time * 60" | bc`  
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
		      new_date_seconds=`echo "date -d \"$START\" \"+%s\" + $TIME * 60" | bc` 
  	          END=`date -d @$new_date_seconds "+%Y-%m-%dT%H:%M:%S"`
		fi
		
		# Print the basic information about input parameters to STDOUT at start 
		echo "PULSAR NAME = $PULSAR"
	    echo "ANTENNA = $ANTENNA"
		echo "Channels per Subband = $CHAN_SUBS"
		echo "Integration Steps = $STEPS"
		if (( $LST == 1 ))
		then
		   echo "Observation Start Time is given as input in LST units"
		fi
		echo "Observation Start Time = $START (UT)"
		echo "Observation duration = $TIME minutes"
		echo "Time Start $START + duration $TIME min = $END End Time"
		echo "RA Pointing = $RA_DEG deg"
		echo "DEC Pointing = $DEC_DEG deg"
		echo "RA Pointing = $RA radians"
		echo "DEC Pointing = $DEC radians"
		echo "Subbands list = $SUBBANDS"
		echo "Stations list = $STATIONS"
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

		#write the observation section per pulsar in the list

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

        if [[ $ANTENNA == "HBA" ]]
        then 
            middle=$middleHBA 
        else
            middle=$middleLBA
        fi
        
        sed -e "s/\<name\>FILL IN OBSERVATION NAME\<\/name\>/\<name\>Obs $PULSAR ($ANTENNA)\<\/name\>/" -e "s/RA/$RA/g" -e "s/DEC/$DEC/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" -e "s/\<description\>FILL IN DESCRIPTION\<\/description\>/\<description\>Obs $PULSAR ($ANTENNA) at $START for $TIME min\<\/description\>/" -e "s/RDEG/$RA_DEG/g" -e "s/DDEG/$DEC_DEG/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" -e "s/LENGTH/$DURATION/g" -e "s/FILL IN TIMESTAMP/$date/g" -e "s/SUBBANDS/$SUBBANDS/g" -e "s/STATION_LIST/$STATION_LIST/g" -e "s/PULSAR/$PULSAR/g" -e "s/CHANNELS PER SUBBAND/$CHAN_SUBS/g" -e "s/INTEG STEPS/$STEPS/g" -e "s/PROJECT NAME/$PROJECT/g" -e "s/IMAGING/$IM_TF/g" $middle >> $outfile

   fi   
done < $infile

# close the XML file with ending
echo "</children>" >> $outfile
echo "</lofar:project>"  >> $outfile
echo ""
echo "DONE: output template is: $outfile" 

exit 0
