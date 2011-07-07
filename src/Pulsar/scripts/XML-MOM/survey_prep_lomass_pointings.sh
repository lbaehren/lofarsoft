#!/bin/ksh


USAGE="\nusage : survey_prep_lomass_pointings.sh pointing_range pointings_file xml_script_options \n\n"\
"      pointing_range  ==> Specify the range of pointing numbers from file;\n"\
"                          Can be a range, a comma-separated list or single number:\n"\
"                          (i.e. 75-139)   (i.e. 77,139,189,190)      (i.e. 75)  \n"\
"      pointings_file  ==> Input pointings table (i.e. LOFAR_HBA_pointings_cluster7_101021.txt) \n"\
"      xml_script_options  ==> All required arguments to run the multi-templatesNbeams.sh script (except -in infile) \n"\
"\n"\
"      Example using range:\n"\
"      survey_prep_pointings.sh 75-139 LoMASS_point_corrected.txt -inswitch BF -intype position -project Pulsars -out output.xml -start 2010-07-22T09:40:00 -time 25 -stations CS001,CS002 -antenna HBA +multi -namecol \n"\
"\n"\
"      Example using row list:\n"\
"      survey_prep_pointings.sh 75,79,93,106 LoMASS_point_corrected.txt -inswitch BF -intype position -project Pulsars -out output.xml -start 2010-07-22T09:40:00 -time 25 -stations CS001,CS002 -antenna HBA +multi -namecol \n"\
"\n"\
"      Example using a single row:\n"\
"      survey_prep_pointings.sh 75 LoMASS_point_corrected.txt -inswitch BF -intype position -project Pulsars -out output.xml -start 2010-07-22T09:40:00 -time 25 -stations CS001,CS002 -antenna HBA +multi -namecol \n"\
"\n"\
"      multi-templatesNbeams.sh HELP:\n"


range=""
range=$1
infile=""
infile=$2

if [ $# -lt 4 ]                    
then
   print "$USAGE"    
   multi-templatesNbeams.sh -help BF | head -60
   exit 1
fi

# shift two args over, and get the rest to feed to the xml-program
shift
shift
args=""
while [ $# -gt 0 ]
do
    case "$1" in
	*)  args="$args $1";;	
    esac
    shift
done

#echo $args

#temporary file
outfile=$prefix"_PipeExec"

if [ ! -f $infile ]
then
   echo "ERROR: $infile does not exist"
   exit 1
fi

is_range=`echo $range | grep "-"`
is_comma=`echo $range | grep ","`
if [[ $is_range != "" ]]
then
   min_range=`echo $range | sed 's/-.*//g'`
   max_range=`echo $range | sed 's/.*-//g'`
   RANGE=1
elif [[ $is_comma != "" ]]
then
   RANGE=0
   tmp_list=`echo $range | sed 's/,/ /g'`
   range=$tmp_list
else #assuming single row
   min_range=$range
   max_range=$range
   RANGE=1
fi

echo "Using input file $infile"
if (( $RANGE == 1 ))
then
   echo "Start at pointing $min_range and go to $max_range"
else
   echo "Using a comma separated list of row numbers"
fi
tmp=/tmp/$$survey.txt
# delete all lines with #
# get all pointings within the range requested - directly corresponds to line numbers
# input is in the following format (without the comment line):
# LoMASS_000   113.36 3.48 ; 113.36 10.78 ; 113.36 17.46 ; 113.36 23.70 ; 113.36 29.64 ; 113.36 35.38 ; 113.36 40.98 ; 113.36 46.50 ; 113.36 52.00 ; 113.36 57.50 ; 113.36 63.02 ; 113.36 68.62 ; 113.36 74.36 ; 113.36 80.30 ; 113.36 86.54


if (( $RANGE == 1 ))
then
   grep -v "#" $infile | sed -n "$min_range,$max_range p" > $tmp
else 
   touch $tmp
   for ii in $range
   do
      grep -v "#" $infile  | sed -n "$ii,$ii p" >> $tmp
   done 
fi

# run the xml script with the tmp file as input and rest of the arguments
echo "Running command:"
echo "multi-templatesNbeams.sh -in $tmp $args"
multi-templatesNbeams.sh -in $tmp $args
status=$?

rm $tmp

if (( $status != 0 ))
then
   echo "ERROR: There was an error either passing input arguments or running the multi-templatesNbeams.sh script."
   exit 1
fi

