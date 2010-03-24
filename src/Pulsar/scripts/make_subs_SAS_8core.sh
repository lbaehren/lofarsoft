#!/bin/ksh -x
#Convert raw LOFAR data
#Assumes 16 channels per subband and divides into 8 chunks of 32 subbands

#Set these parameters by hand
OBSID=L2010_06296
PARSET=/globalhome/lofarsystem/log/${OBSID}/RTCP.parset.0
PULSAR=B2111+46
STOKES=incoherentstokes
#STOKES=stokes
COLLAPSE=              #To collapse put "-C" here

alias bf2presto=/home/hassall/bin/bf2presto-working

######################
# DON'T CHANGE BELOW #
######################
date_start=`date`

ARRAY=`cat $PARSET | grep "Observation.bandFilter" | awk -F "= " '{print $2}' | awk -F "_" '{print $1}'`
CHAN=`cat $PARSET | grep "Observation.channelsPerSubband" | awk -F "= " '{print $2}'`
DOWN=`cat $PARSET | grep "Observation.ObservationControl.OnlineControl.OLAP.Stokes.integrationSteps" | awk -F "= " '{print $2}'`
MAGIC_NUM=`cat $PARSET | grep "OLAP.CNProc.integrationSteps" | awk -F "= " '{print $2}'`
SAMPLES=`echo ${MAGIC_NUM}/${DOWN}| bc`

echo "PULSAR:" $PULSAR 
echo "ARRAY:" $ARRAY 
echo "CHANNELS:" $CHAN 
echo "Number of SAMPLES:" $SAMPLES

echo "Starting Time"
date

num_dir="0 1 2 3 4 5 6 7"
subband_tot=(248 * 16)
echo Total number of Subbands is $subband_tot

#Set up the list of files called "DONE" for output checking
done_list=""
for ii in $num_dir
do
   if [ $ii -ne 7 ]
   then
      done_list=`echo "&& -e "${STOKES}/"RSP"$ii"/DONE" $done_list`
   else
      done_list=`echo ${STOKES}/"RSP"$ii"/DONE"  $done_list`
   fi   
done

#As a testing measure, you can set run_this to 1 
# in order to skip the bf2presto if it's already done;
# the script picks up after the bf2presto step.
# By default (0), everything is run.
run_this=0
if [ $run_this -eq 0 ] 
then
	#Create directories with appropriate permissions
	for ii in $num_dir
	do
	   mkdir -p ${STOKES}/RSP$ii
	   chmod 774 ${STOKES}/RSP$ii
	   chgrp pulsar ${STOKES}/RSP$ii
	done
	
	master_list=${STOKES}/SB_master.list
	#Create subband lists
	all_list=`ls /net/sub[456]/lse*/data?/${OBSID}/SB*.MS.incoherentstokes | sort -t B -g -k 2`
	ls /net/sub[456]/lse*/data?/${OBSID}/SB*.MS.incoherentstokes | sort -t B -g -k 2 > $master_list
	all_num=`wc -l $master_list | awk '{print $1}'`
	
	if [ $all_num -lt 8 ]
	then
	   echo "Error: Less than 8 subbands found, unlikely to be a valid observation"
	   exit 1
	fi
	
	div_files=`echo $all_num | awk '{print $1 / 8}'`
	count=0
	
	#Create 8-sections of the file list
	split -a 1 -d -l $div_files $master_list ${STOKES}/$$"_split_"
	
	#Move the lists to the directories
	for ii in $num_dir
	do
	   mv ${STOKES}/$$"_split_"$ii ${STOKES}/"RSP"$ii"/RSP"$ii".list"
	done
	
	echo "Starting bf2presto conversion"
	date
	
	#Convert the subbands with bf2presto
	for ii in $num_dir
	do
	   echo 'Converting subbands: '`cat ${STOKES}/"RSP"$ii"/RSP"$ii".list"` >> ${STOKES}/RSP$ii"/bf2presto_RSP"$ii".out" 2>&1 
	   bf2presto ${COLLAPSE} -A 10 -f 0 -c ${CHAN} -n ${DOWN} -N ${SAMPLES} -o ${STOKES}/RSP$ii"/"${PULSAR}_${OBSID}"_RSP"$ii `cat ${STOKES}/"RSP"$ii"/RSP"$ii".list"` >> ${STOKES}"/RSP"$ii"/bf2presto_RSP"$ii".out" 2>&1 && touch ${STOKES}/"RSP"$ii"/DONE"  >> ${STOKES}"/RSP"$ii"/bf2presto_RSP"$ii".out"  2>&1 &
	done
	
	echo "Running bf2presto in the background..." 
		
	#Check when all 8 DONE files are available, then all processes have exited
	ii=1
	yy=0
	while [ $ii -ne $yy ]
	do
	   if [ -e $done_list ]
	   then
	      echo "All bf2presto tasks have completed!" 
	      yy=1
	   fi
	   sleep 10
	done
	
fi # end of run_this for testing purposes to skip the bf2presto section

#Clean up the DONE file
for ii in $num_dir
do
   rm -rf ${STOKES}/"RSP"$ii"/DONE" 
done

#Create .sub.inf files with par2inf.py
cp $PARSET ./${OBSID}.parset
cp ~hessels/default.inf .
python ${LOFARSOFT}/release/share/pulsar/bin/par2inf.py -S ${PULSAR} -o test -n 31 -r 8 ./${OBSID}.parset
jj=0
for ii in `ls test*.inf`
do
  mv ${ii} ${STOKES}/RSP${jj}/${PULSAR}_${OBSID}_RSP${jj}.sub.inf
  jj=`expr $jj + 1`
done

echo "Starting folding"
date

#Fold the data
location=`pwd`
for ii in $num_dir
do
   cd ${location}/${STOKES}/RSP${ii}
###   prepfold -noxwin -psr ${PULSAR} -n 256 -fine -nopdsearch -o ${STOKES}/RSP${ii}/${PULSAR}_${OBSID}_RSP${ii} ${STOKES}/RSP${ii}/${PULSAR}_${OBSID}_RSP${ii}.sub[0-9]* >> ${STOKES}/RSP${ii}/${PULSAR}_${OBSID}_RSP${ii}.prepout 2>&1 && touch ${STOKES}/"RSP"$ii"/DONE" >> ${STOKES}/RSP${ii}/${PULSAR}_${OBSID}_RSP${ii}.prepout 2>&1 &
   prepfold -noxwin -psr ${PULSAR} -n 256 -fine -nopdsearch -o ${PULSAR}_${OBSID}_RSP${ii} ${PULSAR}_${OBSID}_RSP${ii}.sub[0-9]* >> ${PULSAR}_${OBSID}_RSP${ii}.prepout 2>&1 && touch "DONE" >> ${PULSAR}_${OBSID}_RSP${ii}.prepout 2>&1 &
   sleep 5
done

cd ${location}

#Check when all 8 DONE files are available, then all processes have exited
ii=1
yy=0
while [ $ii -ne $yy ]
do
   if [ -e $done_list ]
   then
      echo "All prepfold tasks have completed!" 
      yy=1
   fi
   sleep 15
done

#Clean up the DONE file
for ii in $num_dir
do
   rm -rf ${STOKES}/"RSP"$ii"/DONE" 
done

#Make a cumulative plot of the profiles
python ${LOFARSOFT}/release/share/pulsar/bin/plot_profs8.py
convert profiles.ps ${PULSAR}_${OBSID}_profiles.pdf
rm profiles.ps

#Make a .pdf version of the plots
for ii in $num_dir
do
   convert ${STOKES}/RSP${ii}/${PULSAR}_${OBSID}_RSP${ii}_PSR_${PULSAR}.pfd.ps ${STOKES}/RSP${ii}/${PULSAR}_${OBSID}_RSP${ii}_PSR_${PULSAR}.pfd.pdf
done

#Make a tarball of all the plots
tar cvf ${PULSAR}_${OBSID}_plots.tar *profiles.pdf ${STOKES}/RSP*/*pfd.ps ${STOKES}/RSP*/*pfd.pdf ${STOKES}/RSP*/*pfd.bestprof ${STOKES}/RSP*/*.sub.inf
gzip ${PULSAR}_${OBSID}_plots.tar

#Change permissions and move files
chmod 774 * ${STOKES}/RSP*/*
chgrp pulsar * ${STOKES}/RSP*/*
mv *.tar.gz *.pdf ${STOKES}

#RFI-Report
echo "Producing rfi report"
date
for ii in $num_dir
do
   cd ${location}/${STOKES}/RSP${ii}
   python ${LOFARSOFT}/release/share/pulsar/bin/subdyn.py --saveonly -n `echo ${SAMPLES}*10 | bc` *.sub0???  && touch DONE &
done

cd ${location}

#Check when all 8 DONE files are available, then all processes have exited
ii=1
yy=0
while [ $ii -ne $yy ]
do
   if [ -e $done_list ]
   then
      echo "All subdyn tasks have completed!" 
      yy=1
   fi
   sleep 15
done

#Clean up the DONE file
for ii in $num_dir
do
   rm -rf ${STOKES}/"RSP"$ii"/DONE" 
done

date_end=`date`
echo "Start Time: " $date_start
echo "End Time: " $date_end

exit 0
