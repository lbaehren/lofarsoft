#!/bin/ksh 

#Combine pieces of MOM XML-templates to create an observation
#Add obsering aspects like the target name and observation time

#Check the usage
USAGE="\nusage : create_obs_template.sh -p Pulsar_name -start Obs_Start_Time -time Obs_Duration_Time_Mins -ra RA -dec DEC -o Output_XMLtempate_Name -st Station_List\n\n"\
"      -p Pulsar_name ==> Specify the Pulsar Name (i.e. B2111+46) \n"\
"      -start Obs_Start_Time  ==> Specify the Observation Start Time in UT (i.e. 2010-07-15T05:00:00) \n"\
"      -time Obs_Duration_Time_Mins  ==> Specify the duration of the Observation in MINUTES (i.e. 10) \n"\
"      -ra RA ==> Right Ascension of the pointing in Decimal Degrees (i.e. 52.25) \n"\
"      -dec DEC ==> Declination of the pointing in Decimal Degrees (i.e. 54.0) \n"\
"      -o Output_XMLtempate_Name ==> Specify the Output XML Tempate Name (i.e. test.xml) \n"\
"      -st Station_List ==> The comma-separated list of stations to be used (capital letters) (i.e. CS001,CS002) \n"

if [ $# -lt 12 ]                    # this script needs at least 6 args, including -switches
then
   print "$USAGE"    
   exit 1
fi

#Get the input arguments
START=""
TIME=""
PULSAR=""
OUTFILE=""
RA=""
RA_DEG=""
DEC=""
DEC_DEG=""
STATIONS=""
input_string=$*
while [ $# -gt 0 ]
do
    case "$1" in
    -start)    START=$2; shift;;
     -time)    TIME=$2; shift;;
        -p)    PULSAR="$2"; shift;;
        -o)    OUTFILE="$2"; shift;;
        -ra)   RA="$2"; shift;;
        -dec)  DEC="$2"; shift;;
        -st)   STATIONS="$2"; shift;;
        -*)
            echo >&2 \
            "$USAGE"
            exit 1;;
        *)  break;;     
    esac
    shift
done

# Print the basic information about input parameters to STDOUT at start of pipeline run
echo "------------------------------------------------------------------"
echo "Running create_obs_template.sh with the following input arguments:"
echo "Observation Start Time = $START (UT)"
echo "Observation duration = $TIME minutes"
echo "PULSAR NAME = $PULSAR"
echo "RA Pointing = $RA deg"
echo "DEC Pointing = $DEC deg"
echo "Stations list is = $STATIONS"
echo "Output XML File = $OUTFILE"
echo "Calculating..."
RA_DEG=$RA
DEC_DEG=$DEC

# If output file exists, overwrite it
if [ -f $OUTFILE ] 
then
   rm -rf $OUTFILE
   echo "WARNING: $OUTFILE files exists;  overwriting." 
fi

# Use a mixture of 'cat' and 'sed' to create the output file, filling in pieces as needed
if [ ! -f $LOFARSOFT/release/share/pulsar/data/XML-template-StatusTop.txt ]
then
   echo "ERROR: missing USG installation of files \$LOFARSOFT/release/share/pulsar/data/XML-template-*.txt"
   echo "       Make sure you have run 'make install' in \$LOFARSOFT/build/pulsar"
   exit 1
fi 

# Top template section comes from XML-template-StatusTop.txt
# Middle template section comes from XML-template-ObsAttMid.txt
# Middle template section comes from XML-template-ChildBottom.txt

top=$LOFARSOFT/release/share/pulsar/data/XML-template-StatusTop.txt
middle=$LOFARSOFT/release/share/pulsar/data/XML-template-ObsAttMid.txt
bottom=$LOFARSOFT/release/share/pulsar/data/XML-template-ChildBottom.txt
# date format needs to be like this: 2010-07-15T12:25:14
date=`date "+%Y-%m-%dT%H:%M:%S"`
PID=$$

# Top XML section with changes:
sed -e "s/\<name\>FILL IN OBSERVATION NAME\<\/name\>/\<name\>Observation $PULSAR PID=$PID\<\/name\>/" -e "s/\<description\>FILL IN DESCRIPTION\<\/description\>/\<description\>Observation $PULSAR at $START for $TIME min\<\/description\>/" -e "s/FILL IN TIMESTAMP/$date/g" $top > $OUTFILE

# Middle XML section with changes:
pi=$(echo "scale=10; 4*a(1)" | bc -l)
RA=`echo "scale=10; $RA * $pi / 180.0" | bc | awk '{printf("%1.9f\n",$1)}'`
echo "RA (radians) = $RA"
DEC=`echo "scale=10; $DEC * $pi / 180.0" | bc | awk '{printf("%1.9f\n",$1)}'`
echo "DEC (radians) = $DEC"
DURATION=`echo "PT"$TIME"M"`

#END=`date -v +$TIME"M" "+%Y-%m-%dT%H:%M:%S"`
END=`date -j -v +$TIME"M" -f "%Y-%m-%dT%H:%M:%S" $START "+%Y-%m-%dT%H:%M:%S"`
echo "Time Start $START + duration $TIME min = $END End Time"

sed -e "s/\<name\>FILL IN OBSERVATION NAME\<\/name\>/\<name\>Observation $PULSAR PID=$PID\<\/name\>/" -e "s/RA/$RA/g" -e "s/DEC/$DEC/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" $middle >> $OUTFILE


# Bottom XML section with changes:
sed -e "s/\<name\>FILL IN OBSERVATION NAME\<\/name\>/\<name\>Observation $PULSAR PID=$PID\<\/name\>/" -e "s/\<description\>FILL IN DESCRIPTION\<\/description\>/\<description\>Observation $PULSAR at $START for $TIME min\<\/description\>/" -e "s/RA_DEG/$RA_DEG/g" -e "s/DEC_DEG/$DEC_DEG/g" -e "s/STARTTIME/$START/g" -e "s/ENDTIME/$END/g" -e "s/DURATION/$DURATION/g" -e "s/FILL IN TIMESTAMP/$date/g" $bottom >> $OUTFILE

# At the end, close the file with the last XML tag:
echo "</lofar:observation>" >> $OUTFILE

echo "Results: $OUTFILE"
echo ""
exit 0