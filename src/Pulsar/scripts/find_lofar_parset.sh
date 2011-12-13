#!/bin/ksh 

# This program (find_lofar_parset.sh) takes one input argument - the OBSID.
# Example:
# > find_lofar_parset.sh L32629
# The return value is either "ERROR" if the parset was not found, or the correct parset location:
# /globalhome/lofarsystem/log/L32629/L32629.parset

# The older format OBSIDs are also accepted: 
# find_lofar_parset.sh L2011_25249
# /globalhome/lofarsystem/log/L25249/L25249.parset

# History of the various parset locations (where the program looks for parsets):
# (1) OLD parset was here: /globalhome/lofarsystem/log/${OBSID}/RTCP.parset.0
# (2) NEW parset as of May 10, 2010 is here: /globalhome/lofarsystem/log/L2010-MM-DD-DATE/RTCP-ID.parset
# (3) 2nd transpose parset as of Aug 20, 2010 is here: /globalhome/lofarsystem/production/lofar-trunk/bgfen/log/L2010-MM-DD-DATE/RTCP-ID.parset

OBSID=""
OBSID=$1
PARSET=""
new_parset=""

if [[ $# < 1 ]]
then
   echo "ERROR"
   exit
fi

#Check if case 1; else case 2
has_underscore=`echo $OBSID | grep "_"`
if [[ $has_underscore != "" ]]
then
    short_id=`echo $OBSID | sed 's/L.....//g'`
    PARSET=/globalhome/lofarsystem/log/${OBSID}/RTCP.parset.0
else
    short_id=`echo $OBSID | sed 's/L//g'`
    PARSET=/globalhome/lofarsystem/log/${OBSID}/${OBSID}.parset
fi


if [ ! -f $PARSET ] 
then
   
   if [ -d /globalhome/lofarsystem/log/ ]
   then 
      new_parset=`find /globalhome/lofarsystem/log/ -name RTCP-${short_id}.parset\* -print`
   fi
   
   if [[ $new_parset == "" ]]
   then
      if [ -d /globalhome/lofarsystem/production/lofar-trunk/bgfen/log/ ]
      then 
         new_parset=`find /globalhome/lofarsystem/production/lofar-trunk/bgfen/log/ -name RTCP-${short_id}.parset -print`
      fi
      
      if [[ $new_parset == "" ]]
      then
          # Sept 23, 2010 another parset location added
          if [ -d /globalhome/lofarsystem/log/ ]
          then 
  	         new_parset=/globalhome/lofarsystem/log/L${short_id}/L${short_id}.parset
  	      fi
		  if [ ! -f $new_parset ] 
		  then
#             echo "ERROR: Unable to find parset for L$short_id in /globalhome/lofarsystem/production/lofar-trunk/bgfen/log/L${short_id} directory"
             echo "ERROR"
             exit
          fi
      else
	      found_nof_parsets=`echo $new_parset | wc -l | awk '{print $1}'`
	      if (( $found_nof_parsets !=  1 ))
	      then
#	         echo "ERROR: Found more than one parset for $short_id in /globalhome/lofarsystem/production/lofar-trunk/bgfen/log/ directory; unable to resolve problem"
	         echo "ERROR"
	         exit
	      fi
      fi
   else
      found_nof_parsets=`echo $new_parset | wc -l | awk '{print $1}'`
      if (( $found_nof_parsets !=  1 ))
      then
#         echo "ERROR: Found more than one parset for $short_id in /globalhome/lofarsystem/log/ directory; unable to resolve problem"
         echo "ERROR"
         exit
      fi
   fi
   if [[ $new_parset == "" ]]
   then
      # unable to find the parset
      echo "ERROR"
   else
      PARSET=$new_parset
      echo $PARSET
   fi
else
   echo $PARSET
fi

