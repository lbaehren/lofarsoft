#!/bin/ksh 

# take a list of observations, and create multiple templates for MOM upload
# required input: list of observations
# Example run:
# prompt> multi-templates.sh -in obs.lis -out out.xml

USAGE="\nusage : multi-templates.sh -in observation_list_file -out template_output_file [-start obs_start] [-time duration] [-st stations_list] [[-subs subband_range]] [[-gap duration]] [[-antenna LBA_or_HBA]] [[-mom MOM_ID]] \n\n"\
"      This script is run using two different methodologies: \n"\
"         \n"\
"      1) The input file contains all the information: pulsar-name  obs-start  duration  station-list \n"\
"         In this case, you only need to specify the input file and output file names (if HBA):\n"\
"         \n"\
"         -in observation_list_file ==> Specify the ascii file with observation listing (i.e. in.txt) \n"\
"         -out template_output_file ==> Specify the name of the output XML template file (i.e. out.xml) \n"\
"         \n"\
"         Optionally you can also change the antenna (HBA is the default) and subband list:\n"\
"         [[-antenna LBA_or_HBA]] ==> Specify the antenna, either 'HBA' or 'LBA'. (i.e. HBA) \n"\
"         [[-subs subband_range]] ==> The subband range (default '200..447') \n"\
"         \n"\
"         For antenna=LBA, you MUST also specify the MOM_ID to overwrite a template:\n"\
"         [[-mom MOM_ID]] ==> Specify the MOM_ID when using LBAs. (i.e. 11090) \n"\
"         \n"\
"         Example input file:\n"\
"         # pulsar         start-time            duration            stations\n"\
"         B1133+16         2010-07-22T14:50:00    10                 CS001,CS002,CS003,CS004,CS005,CS006,CS007\n"\
"         B1133+16         2010-07-22T15:03:00    10                 CS001,CS002,CS003,CS004,CS005,CS006,CS007\n"\
"         B1133+16         2010-07-22T15:15:30    10                 CS001,CS002,CS003,CS004,CS005,CS006,CS007\n"\
"         B1133+16         2010-07-22T15:27:30    10                 CS001,CS002,CS003,CS004,CS005,CS006,CS007\n"\
"         B1133+16         2010-07-22T15:39:00    10                 CS001,CS002,CS003,CS004,CS005,CS006,CS007\n"\
"         B1237+25         2010-07-22T15:52:00    60                 CS001,CS002,CS003,CS004,CS005,CS006,CS007\n"\
"         B1508+55         2010-07-22T16:55:00    60                 CS001,CS002,CS003,CS004,CS005,CS006,CS007\n"\
"         \n"\
"         Example runs:\n"\
"         > multi-templates.sh -in obs_list_July22_b.txt -out output.xml \n"\
"         > multi-templates.sh -in obs_list_July22_b.txt -out output.xml -subs 10..45 \n"\
"         > multi-templates.sh -in obs_list_July22_b.txt -out output.xml -antenna LBA -mom 11090 -subs 10..45 \n"\
"         \n"\
"      2) The input file contains just one columns with the pulsar name. \n"\
"         You must specify the input file name and output file name. \n"\
"         In this case, you MUST also specify the following parameters on the command line as input...\n"\
"         Note, these parameters are used for the entire list;  start time of subsequent observations\n"\
"         is calculated based on observation duration and gap between observations.\n"\
"         \n"\
"         [-start obs_start] ==> The start date/time for 1st source in the list of observations; note format; supercede's infile values (i.e. 2010-07-22T14:50:00) \n"\
"         [-time duration] ==> The duration of ALL observations in the list, in minutes; supercede's infile values (i.e. 60) \n"\
"         [-stations stations_list] ==> The list of LOFAR stations, supercede's infile values and used for all obs (i.e. 'CS001,CS002') \n"\
"         \n"\
"         Optionally you can also change the antenna (HBA is the default), subband list and the gap between observations::\n"\
"         [[-antenna LBA_or_HBA]] ==> Specify the antenna, either 'HBA' or 'LBA'. (i.e. HBA) \n"\
"         [[-subs subband_range]] ==> The subband range (default '200..447') \n"\
"         [[-gap duration]] ==> The time between ALL observations in minutes (default 3) \n"\
"         \n"\
"         Example input file:\n"\
"         #pulsar\n"\
"         B0154+61\n"\
"         B0331+45\n"\
"         J0459-0210\n"\
"         \n"\
"         Example runs:\n"\
"         > multi-templates.sh -in obs_single_col.txt -out output.xml -start 2010-07-22T14:00:00 -time 33 -stations CS001,CS002,CS003,CS004\n"\
"         > multi-templates.sh -in obs_single_col.txt -out output.xml -start 2010-07-22T14:50:00 -time 10 -stations CS001,CS002 -gap 6\n"\
"         > multi-templates.sh -in obs_single_col.txt -out output.xml -start 2010-07-22T09:40:00 -time 25 -stations CS001 -subs 10..45 \n"\
"         > multi-templates.sh -in obs_single_col.txt -out output.xml -start 2010-07-22T09:40:00 -time 25 -stations CS001 -subs 10..45 -antenna LBA -mom 11090\n"\
"         > multi-templates.sh -in obs_single_col.txt -out output.xml -start 2010-07-22T09:40:00 -time 25 -stations CS001 -antenna LBA -mom 11090\n"\
"         \n"



if [ $# -lt 4 ]                    
then
   print "$USAGE"    
   exit 1
fi

infile=""
outfile=""
SUBBANDS=""
user_subbands=0
START=""
user_start=0
previous_start=""
TIME=0
user_duration=0
STATIONS=""
user_stations=0
GAP=3
user_gap=0
ANTENNA=HBA
MOM=""

while [ $# -gt 0 ]
do
    case "$1" in
      -in)    infile=$2; shift;;
     -out)    outfile=$2; shift;;
     -start)  START=$2; previous_start=$2; user_start=1; shift;;
     -time)   TIME=$2; user_duration=1; shift;;
     -gap)    GAP=$2; user_gap=1; shift;;
     -subs)   SUBBANDS=$2; user_subbands=1; shift;;
     -antenna)   ANTENNA=$2; shift;;
     -mom)    MOM=$2; shift;;
     -stations)     STATIONS=$2; user_stations=1; shift;;
       -*)
            echo >&2 \
            "$USAGE"
            exit 1;;
        *)  break;;     
    esac
    shift
done

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

#If user specified modes, then check that user has set them all on the command line
if (( $user_start == 0 )) && (( $user_duration == 0 )) && (( $user_stations == 0 )) 
then
   echo "Using the input file for all observation settings."
   if (( $user_gap != 0 ))
   then
      echo "WARNING: ignoring user-specified gap setting;  start time from observation list will be used."
   fi
else
   echo "Using the user-settings for start, duration and stations list."
   if (( $user_start != 1 )) || (( $user_duration != 1 )) || (( $user_stations != 1 ))
   then
      echo "ERROR: You must set all threee user-input parameters: [-start obs_start] [-time duration] [-st stations_list]"
      exit 1
   fi   
fi

if (( $user_subbands == 0 ))
then
   if [[ $ANTENNA == "HBA" ]]
   then
      SUBBANDS="200..447"
   elif [[ $ANTENNA == "LBA" ]]
   then
      SUBBANDS="154..401"
   fi   
fi

num_lines=`grep -v "#" $infile | wc -l | awk '{print $1}'`
catalog=$LOFARSOFT/release/share/pulsar/data/PSR_catalog.txt
echo "Wokring on $infile with $num_lines sources"
echo "Using PSR catalog file $catalog"
echo "Using output file $outfile"

#create the header of the xml output file
header=$LOFARSOFT/release/share/pulsar/data/XML-template-header.txt
if [ ! -f $header ]
then
   echo "ERROR: missing USG installation of files \$LOFARSOFT/release/share/pulsar/data/XML-template-*.txt"
   echo "       Make sure you have run 'make install' in \$LOFARSOFT/build/pulsar"
   exit 1
fi 

cat $header > $outfile

if [[ $ANTENNA == "HBA" ]] 
then 
   echo "ANTENNA set to: $ANTENNA"
elif [[ $ANTENNA == "LBA" ]]
then
   echo "ANTENNA set to: $ANTENNA"
   if [[ $MOM == "" ]]
   then
      echo "ERROR: for antenna=$ANTENNA, you must set the MOM_ID using the -mom input switch"
      exit 1
   fi 
else
   echo "ERROR: ANTENNA setting $ANTENNA is unrecognized (must be 'HBA' or 'LBA')."
   exit 1
fi

middle=$LOFARSOFT/release/share/pulsar/data/XML-template-singleobs_$ANTENNA.txt
if [ ! -f $middle ]
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


#The 'date' command is highly dependant on the OS;  
#BSD 'date' has more functionality than Linux/Ubuntu date;
#therefore, need to make to cases for the date-oriented calculations
system_typ=`uname -s | awk '{print $1}'`

#for each source, create an observation
while read line
do
   echo "------------------------------------------------------------------"
   is_header=`echo $line | grep "#"`
   if [[ $is_header == "" ]]
   then
	    PULSAR=`echo $line | awk '{print $1}'`
	    if (( $user_start == 0 ))
	    then
	       START=`echo $line | awk '{print $2}'`
	    fi
	    if (( $user_duration == 0 ))
	    then
	       TIME=`echo $line | awk '{print $3}'`
	    fi
	    if (( $user_stations == 0 ))
	    then
	       STATIONS=`echo $line | awk '{print $4}'`                
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
		echo "Running multi_template.sh with the following input arguments:"
		echo "PULSAR NAME = $PULSAR"
		echo "Observation Start Time = $START (UT)"
		echo "Observation duration = $TIME minutes"
		echo "Time Start $START + duration $TIME min = $END End Time"
		echo "RA Pointing = $RA_DEG deg"
		echo "DEC Pointing = $DEC_DEG deg"
		echo "RA Pointing = $RA radians"
		echo "DEC Pointing = $DEC radians"
		echo "Subbands list = $SUBBANDS"
		echo "Stations list = $STATIONS"

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


        sed -e "s/\<name\>FILL IN OBSERVATION NAME\<\/name\>/\<name\>Obs $PULSAR ($ANTENNA)\<\/name\>/" -e "s/RA/$RA/g" -e "s/DEC/$DEC/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" -e "s/\<description\>FILL IN DESCRIPTION\<\/description\>/\<description\>Obs $PULSAR ($ANTENNA) at $START for $TIME min\<\/description\>/" -e "s/RDEG/$RA_DEG/g" -e "s/DDEG/$DEC_DEG/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" -e "s/LENGTH/$DURATION/g" -e "s/FILL IN TIMESTAMP/$date/g" -e "s/SUBBANDS/$SUBBANDS/g" -e "s/STATION_LIST/$STATION_LIST/g" -e "s/PULSAR/$PULSAR/g" -e "s/MOM_ID/$MOM/g" $middle >> $outfile
   fi   
done < $infile

# close the XML file with ending
echo "</children>" >> $outfile
echo "</lofar:project>"  >> $outfile
echo ""
echo "DONE: output template is: $outfile" 

exit 0
