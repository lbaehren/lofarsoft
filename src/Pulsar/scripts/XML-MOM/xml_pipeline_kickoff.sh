#!/bin/ksh 

USAGE="\nusage : multi-xml_pipeline_kickoff.sh obs_finished.xml pipeline_output_processing_location\n\n"\

infile=$1
out_location=$2
outfile=$2/run_$$

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

if [ $# -lt 2 ]                    
then
   print "$USAGE"    
   exit 1
fi

# in one command, do the following steps:
# grab the observationId and name w/(HBA) string from the xml file
# add a "\" to the end of the line containing observationId
# paste the line with "\" and the next line together
# grep for lines which only contain observationId
# strip the actual ObsID and the Pulsar name from this line
# create the run script for the shell script pipeline

egrep "observationId|\(HBA\) " $infile | sed 's/\/observationId./observationId\>\\/g' | sed -e :a -e '/\\$/N; s/\\\n//; ta' | grep observationId | sed 's/\<observationId\>//' | sed 's/<>//' | sed 's/<.*Obs / /g' | sed 's/(HBA.*//g' | awk -v location=$out_location '{printf("pulp.sh -id L2010_0%s -p %s -o %s/L2010_0%s_red -all -rfi\n", $1, $2, location, $1)}' > $outfile

chmod 777 $outfile

echo "DONE: now you can execute script '$outfile' in $location/"
