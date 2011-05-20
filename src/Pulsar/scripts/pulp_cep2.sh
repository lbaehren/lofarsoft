#!/bin/ksh

USAGE="\nusage : pulp_cep2.sh <args identical to pulp.sh> \n\n"\
"      NOTE: If '-incoh_only' or '-coh_only' is not used, then both will be run."\
"\n"\
"      Example:\n"\
"      pulp_cep2.sh -id L26170 -p B1254-10 -o L26170_red -incoh_only -rfi -all \n"\
"      pulp_cep2.sh -id L26170 -p B1254-10 -o L26170_red -coh_only -rfi \n"\
"      pulp_cep2.sh -id L26170 -p B1254-10 -o L26170_red -rfi -all\n"\
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
incoh=`echo $args | grep "\-incoh_only"`
coh=`echo $args | grep "\-coh_only"`

if [[ $incoh == "" ]] && [[ $coh == "" ]]
then
   echo "pulp.sh $args -incoh_only" | sed 's/_red/_redIS/g' > pulp_$$tmp.all
   echo "pulp.sh $args -coh_only"  >> pulp_$$tmp.all
elif [[ $incoh != "" ]] && [[ $coh == "" ]]
then
   echo "pulp.sh $args" | sed 's/_red/_redIS/g' > pulp_$$tmp.all
elif [[ $incoh == "" ]] && [[ $coh != "" ]]
then
   echo "pulp.sh $args"  > pulp_$$tmp.all
else
   echo "ERROR: unable to interpret input"
fi

#feed the input into xml_pipeline_kickoff.sh
xml_pipeline_kickoff.sh -infile pulp_$$tmp -cep2 -noxml -prefix pulp_$$tmp
