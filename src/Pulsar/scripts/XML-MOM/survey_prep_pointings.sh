#!/bin/ksh


USAGE="\nusage : survey_prep_pointings.sh pointing_range pointings_file xml_script_options \n\n"\
"      pointing_range  ==> Specify the range of pointing numbers from file;\n"\
"                          Can be a range, a comma-separated list or single number:\n"\
"                          (i.e. 75-139)   (i.e. 77,139,189,190)      (i.e. 75)  \n"\
"      pointings_file  ==> Input pointings table (i.e. LOFAR_HBA_pointings_cluster7_101021.txt) \n"\
"      xml_script_options  ==> All required arguments to run the multi-templatesNbeams.sh script (except -in infile) \n"\
"\n"\
"      Example using range:\n"\
"      survey_prep_pointings.sh 75-139 LOFAR_HBA_catalog_cluster7_101031.txt -inswitch BF -intype position -project Pulsars -out output.xml -start 2010-07-22T09:40:00 -time 25 -stations CS001,CS002 -antenna HBA +multi -namecol \n"\
"\n"\
"      Example using row list:\n"\
"      survey_prep_pointings.sh 75,79,93,106 LOFAR_HBA_catalog_cluster7_101031.txt -inswitch BF -intype position -project Pulsars -out output.xml -start 2010-07-22T09:40:00 -time 25 -stations CS001,CS002 -antenna HBA +multi -namecol \n"\
"\n"\
"      Example using a single row:\n"\
"      survey_prep_pointings.sh 75 LOFAR_HBA_catalog_cluster7_101031.txt -inswitch BF -intype position -project Pulsars -out output.xml -start 2010-07-22T09:40:00 -time 25 -stations CS001,CS002 -antenna HBA +multi -namecol \n"\
"\n"\
"      multi-templatesNbeams.sh HELP:\n"


range=""
range=$1
infile=""
infile=$2

if [ $# -lt 4 ]                    
then
   print "$USAGE"    
   multi-templatesNbeams.sh -help BF | head -46
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
# get all pointings w/o reduntant central pointing
# print just the pointing name and ra,dec in decimal deg
# add a ; to the end of all lines except beam 06
# append lines with ;

# 1st version does not name the NAMECOL in results:
# grep -v "#" $infile | grep "_[0-9]" | awk '{print $1 " " $4 " " $5}' | sed '/_0[0-5]/s/$/ ;/g' | sed -e :a -e '/;$/N; s/;\n/ ; /; ta' | sed -n "$min_range,$max_range p" | sed 's/LOFAR_HBA....._.. //g' > $tmp
# 2nd version does have the NAMECOL in 1st beam results:

if (( $RANGE == 1 ))
then
   grep -v "#" $infile | grep "_[0-9]" | awk '{print $1 " " $4 " " $5}' | sed '/_0[0-5]/s/$/ ;/g' | sed -e :a -e '/;$/N; s/;\n/ ; /; ta' | sed -n "$min_range,$max_range p" | sed 's/LOFAR_HBA....._0[1-9] //g' | sed 's/_00//g' > $tmp
else 
   touch $tmp
   for ii in $range
   do
      grep -v "#" $infile | grep "_[0-9]" | awk '{print $1 " " $4 " " $5}' | sed '/_0[0-5]/s/$/ ;/g' | sed -e :a -e '/;$/N; s/;\n/ ; /; ta' | sed -n "$ii,$ii p" | sed 's/LOFAR_HBA....._0[1-9] //g' | sed 's/_00//g' >> $tmp
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

