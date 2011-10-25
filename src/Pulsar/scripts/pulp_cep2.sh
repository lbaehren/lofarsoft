#!/bin/ksh

USAGE="\nusage : pulp_cep2.sh <args identical to pulp.sh> \n\n"\
"      NOTE: If '-incoh_only' or '-coh_only' is not used, then both will be run."\
"      NOTE: There is no need to specify the raw data location becuase cep2 is assumed."\
"      NOTE: There is little flexibility with the output directory name;  use _red for everything."\
"      To run just the hoover/collection part of coh_only, using switch '-hoover_only'."\
"\n"\
"      Example:\n"\
"      pulp_cep2.sh -id L26170 -p B1254-10 -o L26170_red -incoh_only -rfi -all \n"\
"      pulp_cep2.sh -id L26170 -p B1254-10 -o L26170_red -coh_only -rfi \n"\
"      pulp_cep2.sh -id L26170 -p B1254-10 -o L26170_red -rfi -all\n"\
"\n"\
"      To get older presto subband format output add '-subs' flag:\n"\
"      pulp_cep2.sh -id L26170 -p B1254-10 -o L26170_red -rfi -all -subs\n"\
"      \n"

if [ $# -lt 6 ]                    
then
   print "$USAGE"    
   exit 1
fi


args=""
while [ $# -gt 0 ]
do
    case "$1" in
	*)  args="$args $1";;	
    esac
    shift
done

# clean out the -del option if it's given
args=`echo $args | sed 's/-del//g'`
hoover_only=`echo $args | grep hoover_only`
hoover=0
if [[ $hoover_only != "" ]]
then
    args=`echo $args | sed 's/-hoover_only//g'`
    hoover=1
else
    hoover=0
fi
incoh=`echo $args | grep "\-incoh_only"`
coh=`echo $args | grep "\-coh_only"`

if [[ $incoh == "" ]] && [[ $coh == "" ]]
then
   echo "pulp.sh $args -incoh_only -raw \"/cep2/locus???_data/\"" | sed 's/_red/_redIS/g' > pulp_$$tmp.all
   echo "pulp.sh $args -coh_only -raw /data/"  >> pulp_$$tmp.all
elif [[ $incoh != "" ]] && [[ $coh == "" ]]
then
   echo "pulp.sh $args -raw \"/cep2/locus???_data/\"" | sed 's/_red/_redIS/g' > pulp_$$tmp.all
elif [[ $incoh == "" ]] && [[ $coh != "" ]]
then
   echo "pulp.sh $args -raw /data/"  > pulp_$$tmp.all
else
   echo "ERROR: unable to interpret input"
fi

#feed the input into xml_pipeline_kickoff.sh
if [[ $hoover == 0 ]]
then
   xml_pipeline_kickoff.sh -infile pulp_$$tmp -cep2 -noxml -prefix pulp_$$tmp
else
   xml_pipeline_kickoff.sh -infile pulp_$$tmp -cep2 -noxml -hoover_only -prefix pulp_$$tmp
fi