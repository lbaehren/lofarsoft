#!/bin/ksh 

# pulsars_at_location.sh  ==  Find the closest and brightest N Pulsars
#
# Input: position ra and dec;  LOFAR antenna;  and maximum number of Pulsars to return
# Output: string of pulsar names separated by "," of all the brightest Pulsars within the FOV of the input location (top N are taken;  max N is set by the user)
# Output: (exit 1) when none are found

catalog=$LOFARSOFT/release/share/pulsar/data/PSR_catalog_NA.txt

ra=$1
dec=$2
antenna=$3
nmax=$4

matched=0

matched_string=""
matched_file=/tmp/$$_matches

while read line
do
	
   is_header=`echo $line | grep "#"`
   is_blank=`echo $line | grep J`

#   is_noflux=`echo $line | grep "NA"`
#   if [[ $is_header == "" ]] && [[ $is_blank != "" ]] && [[ $is_noflux == "" ]]

   if [[ $is_header == "" ]] && [[ $is_blank != "" ]] 
   then
	   NAMEB=`echo $line | awk '{print $2}'`   
	   NAMEJ=`echo $line | awk '{print $3}'`   
	   RA_CAT=`echo $line | awk '{print $6}'` 
	   DEC_CAT=`echo $line | awk '{print $7}'` 
	   FLUX_CAT=`echo $line | awk '{print $10}'` 

       check_executable=`which skycoor`
       status=$!
       if (( status	!= 0 ))
       then
          echo "ERROR: Unble to find skycoor executable;  try 'make pulsar' in $LOFARSOFT/build"
          echo "ERROR"
       fi
       
	   distance_deg=`skycoor -r $ra $dec $RA_CAT $DEC_CAT | awk '{print $1 / 3600}'`

       if [[ $antenna == "HBA" ]]
       then 
         max_distance=2.25
       elif [[ $antenna == "LBA" ]]
       then
         max_distance=8.5
       else
          echo "ERROR: Antenna setting of $antenna is not recognised"
          echo "ERROR"
       fi

	   if (( $distance_deg < $max_distance ))
	   then
	      echo "$NAMEB $RA_CAT $DEC_CAT $FLUX_CAT near $obj at $distance_deg deg-separation" >> $matched_file
	      matched=1
	   fi
   fi
done < $catalog

if (( $matched == 0 ))
then
      echo "Position not matched to Pulsar Catalog"
      echo "NONE"
fi

matched_string=`cat $matched_file | sort -k 4,4 | head -$nmax | awk '{print $1}' |  tr '\n' ',' | sed 's/,$//g'`
echo $matched_string

rm $matched_file

