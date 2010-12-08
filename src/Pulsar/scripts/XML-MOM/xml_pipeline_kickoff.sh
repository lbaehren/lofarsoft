#!/bin/ksh

USAGE="\nusage : xml_pipeline_kickoff.sh -infile obs_finished.xml -prefix prefix [ -splits number ] \n\n"\
"      -infile obs_finished.xml  ==> Specify the finished xml file name (i.e. Obs_B1254-10_HBA.xml) \n"\
"      -prefix run  ==> Output name prefix (i.e. Obs_20100730) \n"\
"      [-splits N]  ==> Number of output split files to contain the processing commands (i.e. 4) \n"\
"      [-survey]    ==> Indicates that all the observations are in survey mode;  changes pipeline args to:\n"\
"                       pulp.sh -id OBSID -o OBSID_red -p position -rfi   [without -all] \n"\
"\n"\
"      Example:\n"\
"      xml_pipeline_kickoff.sh -infile Obs_B1254-10_HBA.xml -prefix Obs_20100730 -splits 4 \n"\
"      xml_pipeline_kickoff.sh -infile Obs_B1254-10_HBA.xml -prefix Obs_20100730 -splits 4 -survey \n"\
"      xml_pipeline_kickoff.sh -infile Obs_B1254-10_HBA.xml -prefix Obs_20100730 -survey \n"

infile=""
prefix=""
splits=1
survey=0

if [ $# -lt 4 ]                    
then
   print "$USAGE"    
   exit 1
fi

while [ $# -gt 0 ]
do
    case "$1" in
    -infile)    infile=$2; shift;;
	-prefix)    prefix=$2; shift;;
	-splits)    splits=$2; shift;;
	-survey)    survey=1;;
	-*)
	    echo >&2 \
	    "$USAGE"
	    exit 1;;
	*)  break;;	
    esac
    shift
done


#temporary file
outfile=$prefix"_PipeExec"

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

# in one command, do the following steps:
# grab the observationId and name w/(.BA) string from the xml file
# add a "\" to the end of the line containing observationId
# paste the line with "\" and the next line together
# grep for lines which only contain observationId
# strip the actual ObsID and the Pulsar name (or Position) from this line
# create the run script for the shell script pipeline

# previous version to extract the OBSID and Target name
#egrep "observationId|\(.BA" $infile | sed 's/\/observationId./observationId\>\\/g' | sed -e :a -e '/\\$/N; s/\\\n//; ta' | grep observationId | sed 's/\<observationId\>//' | sed 's/<>//' | sed 's/<.*Obs / /g' | sed 's/(.BA.*//g' | awk '{printf("pulp.sh -id L2010_%05d -p %s -o L2010_%05d_red -all -rfi\n", $1, $2, $1)}' > $outfile

if [[ $survey == 0 ]]
then
   egrep "description|observationId" $infile  | sed 's/\/observationId./observationId\>\\/g' | sed -e :a -e '/\\$/N; s/\\\n//; ta' | grep observationId | sed 's/\<observationId\>//' | sed 's/<>//' | sed 's/\</ \</g' | sed 's/\>/\> /g' | sed 's/(.*//g' | sed 's/\<.*\>//g' | awk '{ if ( $3 == "Pos" ) printf("pulp.sh -id L2010_%05d -p position -o L2010_%05d_red -all -rfi\n", $1, $1); else if ( $2 == "Obs" ) printf("pulp.sh -id L2010_%05d -p %s -o L2010_%05d_red -all -rfi\n", $1, $3, $1) ; else printf("pulp.sh -id L2010_%05d -p %s -o L2010_%05d_red -all -rfi\n", $1, $2, $1)}' > $outfile
else
   egrep "description|observationId" $infile  | sed 's/\/observationId./observationId\>\\/g' | sed -e :a -e '/\\$/N; s/\\\n//; ta' | grep observationId | sed 's/\<observationId\>//' | sed 's/<>//' | sed 's/\</ \</g' | sed 's/\>/\> /g' | sed 's/(.*//g' | sed 's/\<.*\>//g' | awk '{ printf("pulp.sh -id L2010_%05d -p position -o L2010_%05d_red -rfi\n", $1, $1)}' > $outfile
fi

cp $outfile $outfile.all

# perform the splitting up of the all files into multiples
ii=0
splits_list=""
jj=$splits
last=$jj
while (( $jj > 0 ))
do 
    splits_list=`echo $jj $splits_list`
    jj=`expr $jj - 1`
done

num_lines=`wc -l $outfile | awk '{print $1}'`

modulo_files=`echo $num_lines $splits | awk '{print ($1 % $2)}'`
if (( modulo_files != 0 ))
then
   echo "Warning: this is an uneven split; one file will contain more OBSIDs than the rest."
fi

div_files=`echo $num_lines $splits | awk '{printf("%d"), $1 / $2}'`

# split the single file into multiples with the same number per split
if (( $splits > 1 ))
then
	for ii in $splits_list
	do
		head -$div_files $outfile > $outfile.$ii
		c=$div_files
		sed "1,$c d" $outfile > $outfile.tmp
		mv $outfile.tmp $outfile
	done
fi

# when there's an uneven split, concatinate the rest to the last split file
if (( modulo_files != 0 ))
then
   cat $outfile >> $outfile.$ii
   rm $outfile
fi

rm $outfile

chmod 777 $outfile.*

echo "Single script '$outfile.all' contains all the processing commands in one file."
if (( $splits > 1 ))
then
   echo "Run that file or, run the N split files:"
   echo "`ls $outfile.[0-9]*`"
   echo "in the processing locations."
fi
