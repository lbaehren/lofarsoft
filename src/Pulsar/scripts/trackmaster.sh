#!/bin/bash

DELAY=300
PI=3.14159265
OBSFREQ=150


usage()
{
  echo
  echo "TrackMaaasteeer (c)2011 ..."
  echo
  echo
  echo "usage: $0 options

 This script calculates the pointing offset caused by the tracking interval (default: ${DELAY} s).

 OPTIONS:
   -h     Show this message
   -p     target pulsar name
  [-f     Observing frequency (MHz) <${OBSFREQ} MHz> ]
  [-r     target RA  (hh:mm:ss)               ]
  [-d     target Dec (sdd:mm:ss)              ]
  [-t     tracking interval (s) <${DELAY} s>       ]"
  echo
  echo
}



while getopts "hp:f:r:d:t:" OPTION
do
     case $OPTION in
         h)
             usage
             exit 1
             ;;
         p)
             psr=$OPTARG
             ;;
         f)
             OBSFREQ=$OPTARG
             ;;
         r)
             RA=$OPTARG
             ;;
         d)
             dec=$OPTARG
             ;;
         t)
             DELAY=$OPTARG
             ;;
         ?)
             usage
             exit
             ;;
     esac
done



if [ "$psr" != "" ]
then
  RA=`psrcat -c "RAJD DECJD" $psr -o short | awk 'NR==5{print $2}'`
  dec=`psrcat -c "RAJD DECJD" $psr -o short | awk 'NR==5{print $3}'`
else
  hh=${RA:0:2}
  mm=${RA:3:2}
  ss=${RA:6}
  RA=`echo | awk '{print '${hh}'*360.0/24.0+'${mm}'/60.0+'${ss}'/3600.0}'`
  
  s=${dec:0:1}
  dd=${dec:1:2}
  mm=${dec:4:2}
  ss=${dec:7:2}

  if [ "${dec:0:1}" != "-" ]
  then
   if [ "${dec:0:1}" != "+" ]
   then
     s="+"
     dd=${dec:0:2}
     mm=${dec:3:2}
     ss=${dec:6:2}
   fi
  fi

  dec=`echo | awk '{print '${s}${dd}'+'${mm}'/60.0+'${ss}'/3600.0}'`  
  
fi



rad2deg=`echo | awk '{print 180.0/'${PI}'}'`
deg2rad=`echo | awk '{print '${PI}'/180.0}'`


dH=`echo | awk '{print '${DELAY}'*15.0/3600.0*'${deg2rad}'}'`


RA=`echo | awk '{print '${RA}'*'${deg2rad}'}'`
dec=`echo | awk '{print '${dec}'*'${deg2rad}'}'`

cosTheta=`echo | awk '{print cos('${dec}')*cos('${dec}')*(cos('${RA}')*cos('${RA}'+'${dH}')+sin('${RA}')*sin('${RA}'+'${dH}'))+sin('${dec}')*sin('${dec}')}'`

Theta=`echo | awk '{print '${rad2deg}'*atan2(sqrt(1-'${cosTheta}'*'${cosTheta}'), '${cosTheta}')}'`


beamsize=`echo | awk '{print 2878.552/'${OBSFREQ}'}'`

RA=`echo | awk '{print '${RA}'*'${rad2deg}'}'`
dec=`echo | awk '{print '${dec}'*'${rad2deg}'}'`

pcnt=`echo | awk '{print '${Theta}'/'${beamsize}'*100.0}'`

echo
echo
echo ">>  Frequency: ${OBSFREQ} MHz |::.::| Analog-beam size: ${beamsize} deg"
echo
echo ">>  Pointing ---  RA: ${RA} deg  --  Dec: ${dec} deg"
echo
echo ">>  Track offset: ${Theta} deg (${pcnt}% of beam)"  
echo
