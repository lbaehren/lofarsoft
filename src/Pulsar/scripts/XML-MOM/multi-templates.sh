#!/bin/ksh 

# take a list of observations, and create multiple templates for MOM upload
# required input: list of observations
# Example run:
# prompt> multi-template.sh -in obs.lis -out out.xml

USAGE="\nusage : multi-template.sh -in observation_list_file -out template_output_file\n\n"\
"      -in observation_list_file ==> Specify the ascii file with observation listing (i.e. in.txt) \n"\
"      -out template_output_file ==> Specify the name of the output XML template file (i.e. out.xml) \n"

if [ $# -lt 4 ]                    
then
   print "$USAGE"    
   exit 1
fi

infile=""
outfile=""

while [ $# -gt 0 ]
do
    case "$1" in
      -in)    infile=$2; shift;;
     -out)    outfile=$2; shift;;
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

middle=$LOFARSOFT/release/share/pulsar/data/XML-template-singleobs.txt
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
STATIONS=""
RA=0.0
DEC=0.0
START=""
TIME=0
SUBBANDS="153..400"

#for each source, create an observation
while read line
do
   echo "------------------------------------------------------------------"
   is_header=`echo $line | grep "#"`
   if [[ $is_header == "" ]]
   then
	    PULSAR=`echo $line | awk '{print $1}'`
	    START=`echo $line | awk '{print $2}'`
	    TIME=`echo $line | awk '{print $3}'`
	    #make sure station list has all capital letters
	    STATIONS=`echo $line | awk '{print $4}' | sed 's/a-z/A-Z/g'`                
	                   
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
           STATION_LIST="\<station name=\"$station\"\/\> $STATION_LIST"
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
		if (( $TIME < 1 ))
		then
		   # change decimal minutes into seconds
		   TIME=`echo $TIME | awk '{print $1 * 60}`
		   if (( $TIME > 0 )) && (( $TIME <= 10 )) 
		   then
		      DURATION=`echo "PT0"$TIME"S"`	
		   else 
		      DURATION=`echo "PT"$TIME"S"`	
		   fi	
		elif (( $TIME > 0 )) && (( $TIME <= 10 ))
		then
		   DURATION=`echo "PT0"$TIME"M"`
		elif (( $TIME >= 10 ))
		then
		   DURATION=`echo "PT"$TIME"M"`
		else
		   echo "ERROR: duration value ($TIME) is not understood"
		   continue
		fi
		
		#END=`date -v +$TIME"M" "+%Y-%m-%dT%H:%M:%S"`
		END=`date -j -v +$TIME"M" -f "%Y-%m-%dT%H:%M:%S" $START "+%Y-%m-%dT%H:%M:%S"`

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
		echo "Stations list is = $STATIONS"
		
		#write the observation section per pulsar in the list
#        sed -e "s/\<name\>FILL IN OBSERVATION NAME\<\/name\>/\<name\>Observation $PULSAR PID=$PID\<\/name\>/" -e "s/RA/$RA/g" -e "s/DEC/$DEC/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" -e "s/\<description\>FILL IN DESCRIPTION\<\/description\>/\<description\>Observation $PULSAR at $START for $TIME min\<\/description\>/" -e "s/RA_DEG/$RA_DEG/g" -e "s/DEC_DEG/$DEC_DEG/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" -e "s/DURATION/$DURATION/g" -e "s/FILL IN TIMESTAMP/$date/g" -e "s/STATION_LIST/$STATION_LIST/g" $middle >> $outfile
        sed -e "s/\<name\>FILL IN OBSERVATION NAME\<\/name\>/\<name\>Obs $PULSAR (HBA)\<\/name\>/" -e "s/RA/$RA/g" -e "s/DEC/$DEC/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" -e "s/\<description\>FILL IN DESCRIPTION\<\/description\>/\<description\>Obs $PULSAR (HBA) at $START for $TIME min\<\/description\>/" -e "s/RDEG/$RA_DEG/g" -e "s/DDEG/$DEC_DEG/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" -e "s/LENGTH/$DURATION/g" -e "s/FILL IN TIMESTAMP/$date/g" -e "s/SUBBANDS/$SUBBANDS/g" -e "s/STATION_LIST/$STATION_LIST/g" -e "s/PULSAR/$PULSAR/g" $middle >> $outfile

   fi   
done < $infile

# close the XML file with ending
echo "</children>" >> $outfile
echo "</lofar:project>"  >> $outfile
echo ""
echo "DONE: output template is: $outfile" 

exit 0
