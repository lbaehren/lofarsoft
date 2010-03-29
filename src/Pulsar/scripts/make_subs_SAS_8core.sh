#!/bin/ksh 
#Convert raw LOFAR data
#Assumes 16 channels per subband and divides into 8 chunks of 31 subbands

#PLEASE increment the version number when you edit this file!!!
VERSION=1.2

#Check the usage
USAGE="\nusage : make_subs_SAS_8core.sh -id OBS_ID -p Pulsar_name -o Output_Processing_Location [-all] [-rfi] [-C] [-del]\n\n"\
"      -id OBS_ID  ==> Specify the Observation ID (i.e. L2010_06296) \n"\
"      -p Pulsar_name ==> Specify the Pulsar Name (i.e. B2111+46) \n"\
"      -o Output_Processing_Location ==> Specify the Output Processing Location \n"\
"         (i.e. /net/sub5/lse013/data4/LOFAR_PULSAR_ARCHIVE_lse013/L2010_06296_red) \n"\
"      [-all] ==> optional parameter perform folding on entire subband set in addition to 8-splits (takes 11 extra min)\n"\
"      [-rfi] ==> optional parameter perform Vlas's RFI checker and only use clean results (takes 7 extra min)\n"\
"      [-C | -c] ==> optional parameter to switch on COLLAPSE\n"\
"      [-del] ==> optional parameter to delete the previous Output_Processing_Location if it exists (override previous results!)\n"

if [ $# -lt 6 ]                    # this script needs at least 6 args, including -switches
then
   print "$USAGE"    
   exit 1
fi

#Get the input arguments
OBSID=""
PULSAR=""
location=""
COLLAPSE=""
delete=0
all=0
rfi=0
while [ $# -gt 0 ]
do
    case "$1" in
    -id)  OBSID=$2; shift;;
	-p)   PULSAR="$2"; shift;;
	-o)   location="$2"; shift;;
	-C)   COLLAPSE="$1";;
	-c)   COLLAPSE="-C";;
	-del) delete=1;;
	-rfi) rfi=1;;
	-all) all=1;;
	-*)
	    echo >&2 \
	    "$USAGE"
	    exit 1;;
	*)  break;;	
    esac
    shift
done

echo "Running make_subs_SAS_8core.sh with the following input arguments:"
echo "    OBSID = $OBSID"
echo "    PULSAR = $PULSAR"
echo "    Output Processing Location = $location"
if [[ "$COLLAPSE" != "" ]]
then 
   echo "    COLLAPSE = $COLLAPSE"
else
   echo "    COLLAPSE is not set"
fi
if [ $all -eq 1 ]
then 
   echo "    Performing additional processing on ALL the subbands"
else
   echo "    No additional processing requested on ALL the subbands"
fi
if [ $rfi -eq 1 ]
then 
   echo "    Performing additional RFI processing"
else
   echo "    No additional RFI processing requested"
fi


#Check whether Output Processing Location already exists
if [ -d $location ] && [ $delete -ne 1 ]
then
   echo "ERROR: Output Processing Location $location already exists;  "
   echo "       please specify different output location or try again with delete option (-del)."
   exit 1
elif [ -d $location ] && [ $delete -eq 1 ]
then
   echo "Deleting previous processing run in $location/*"
   rm -rf $location/*
   cd $location
else
   mkdir -p $location 
   cd $location
fi

#In the event that location is a specified relative path, put the absolute path as location
location=`pwd`

#Set these parameters by hand
###OBSID=L2010_06296
PARSET=/globalhome/lofarsystem/log/${OBSID}/RTCP.parset.0
###PULSAR=B2111+46
STOKES=incoherentstokes
#STOKES=stokes
###COLLAPSE=              #To collapse put "-C" here

alias bf2presto=/home/hassall/bin/bf2presto-working

######################
# DON'T CHANGE BELOW #
######################
date_start=`date`

#Set up generic pipeline version log file
log=${location}/make_subs_SAS_8core.log
echo "$0 $*" > $log
echo "Pipeline V$VERSION" >> $log
echo "Start date: $date_start" >> $log

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

#Create directories with appropriate permissions
for ii in $num_dir
do
  mkdir -p ${STOKES}/RSP$ii
  chmod 774 ${STOKES}/RSP$ii
  chgrp pulsar ${STOKES}/RSP$ii
done
if [ $all -eq 1 ]
then 
  mkdir ${STOKES}/"RSPA"
  chmod 774 ${STOKES}/"RSPA"
  chgrp pulsar ${STOKES}/"RSPA"
  fi
  
master_list=${STOKES}/SB_master.list
#Create subband lists
all_list=`ls /net/sub[456]/lse*/data?/${OBSID}/SB*.MS.incoherentstokes | sort -t B -g -k 2`
ls /net/sub[456]/lse*/data?/${OBSID}/SB*.MS.incoherentstokes | sort -t B -g -k 2 > $master_list
all_num=`wc -l $master_list | awk '{print $1}'`

echo "Found a total of $all_num SB MS incoherentstokes input datafiles to process" 
echo "Found a total of $all_num SB MS incoherentstokes input datafiles to process" >> $log

if [ $all_num -lt 8 ]
then
  echo "Error: Less than 8 subbands found, unlikely to be a valid observation"
  exit 1
fi

div_files=`echo $all_num | awk '{print $1 / 8}'`
count=0

#Create 8-sections of the file list
split -a 1 -d -l $div_files $master_list ${STOKES}/$$"_split_"
status=$?

if [ $status -ne 0 ]
then
   echo "ERROR: 'split' command unable to split $all_num files into $div_files chunks (not integer number);  "
   echo "       you may need to run the non-multi-core pipeline version make_subs_SAS.sh by hand"
   exit 1
fi

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
###  bf2presto ${COLLAPSE} -A 10 -f 0 -c ${CHAN} -n ${DOWN} -N ${SAMPLES} -o ${STOKES}/RSP$ii"/"${PULSAR}_${OBSID}"_RSP"$ii `cat ${STOKES}/"RSP"$ii"/RSP"$ii".list"` >> ${STOKES}"/RSP"$ii"/bf2presto_RSP"$ii".out" 2>&1 && touch ${STOKES}/"RSP"$ii"/DONE"  >> ${STOKES}"/RSP"$ii"/bf2presto_RSP"$ii".out"  2>&1 &
  bf2presto ${COLLAPSE} -A 10 -f 0 -c ${CHAN} -n ${DOWN} -N ${SAMPLES} -o ${STOKES}/RSP$ii"/"${PULSAR}_${OBSID}"_RSP"$ii `cat ${STOKES}/"RSP"$ii"/RSP"$ii".list"` >> ${STOKES}"/RSP"$ii"/bf2presto_RSP"$ii".out" 2>&1 &
   set bf2presto_pid_$ii=$!
done

echo "Running bf2presto in the background..." 

#Create .sub.inf files with par2inf.py
cp $PARSET ./${OBSID}.parset
cp ~hessels/default.inf .
python ${LOFARSOFT}/release/share/pulsar/bin/par2inf.py -S ${PULSAR} -o test -n 31 -r 8 ./${OBSID}.parset
status=$?

if [ $status -ne 0 ]
then
   echo "ERROR: Unable to successfully run par2inf task"
   exit 1
fi

jj=0
for ii in `ls test*.inf`
do
 mv ${ii} ${STOKES}/RSP${jj}/${PULSAR}_${OBSID}_RSP${jj}.sub.inf
 jj=`expr $jj + 1`
done

#Create the .sub.inf file for the entire set (in background, as there is plenty of time to finish before file is needed
if [ $all -eq 1 ]
then 
     python ${LOFARSOFT}/release/share/pulsar/bin/par2inf.py -S ${PULSAR} -o test -n 248 -r 1 ./${OBSID}.parset && mv `ls test*.inf` ${STOKES}/RSPA/${PULSAR}_${OBSID}_RSPA.sub.inf &
fi

#Check when all 8 DONE files are available, then all processes have exited
#ii=1
#yy=0
#while [ $ii -ne $yy ]
#do
#  if [ -e $done_list ]
#  then
#     echo "All bf2presto tasks have completed!" 
#     yy=1
#     sleep 5
#  fi
#  sleep 10
#done
for ii in $num_dir
do
   echo "Waiting for loop $ii bf2presto to finish"
   wait $bf2presto_pid_$ii
done

#Make a master subband location with all subbands (run in the background, while other tasks are being done)
if [ $all -eq 1 ]
then 
     cd ${STOKES}/"RSPA"
     #master_counter=0
     echo "#!/bin/sh" > run.sh
  # the method below is too slow, so keeping it for reference, using awk/sed instead
#    for ii in $num_dir
#    do
#       sub_rsp_counter=0
#   
#       for ((jj=0; jj<=495; jj++))
#       do
#          old_num=`echo $jj | awk '{printf("%04d"),$1}'`
#          new_num=`echo $master_counter | awk '{printf("%04d"),$1}'`
#          echo "ln -s ../RSP${ii}/${PULSAR}_${OBSID}_RSP${ii}.sub$old_num ${PULSAR}_${OBSID}_RSPA.sub$new_num" >> run.sh
#          (( master_counter += 1 ))
#          (( sub_rsp_counter += 1 ))
#       done
#    done
      ls ../RSP0/*sub0??? ../RSP1/*sub0??? ../RSP2/*sub0??? ../RSP3/*sub0??? ../RSP4/*sub0??? ../RSP5/*sub0??? ../RSP6/*sub0??? ../RSP7/*sub0??? | sed 's/\// /g' | awk '{print $3}' | sed 's/RSP/ RSP /' | sed 's/ RSP/RSP/g' | sed 's/\.sub/ /' | awk '{printf("ln -s ../RSP%d/%s%d.sub%04d %sA.sub%04d\n"),$2,$1,$2,$3,$1,$2*496+$3}' >> run.sh
      
      status=$?

      if [ $status -ne 0 ]
      then
         echo "WARNING: Unable to successfully run creation of link file list for ALL subbands"
         echo "         Skipping the ALL processing"
         echo "WARNING: Unable to successfully run creation of link file list for ALL subbands"  >> $log
         echo "         Skipping the ALL processing" >> $log
         all=0
         break
     fi

     echo "Performing subband linking for all RPSs in one location"
     chmod 777 run.sh
     check_number=`wc -l run.sh | awk '{print $1}'`
     if [ $check_number -ne 3969 ]
     then
        all=0
        echo "Warning - problem running on ALL subbands;  master list is too short (is $check_number but should be 3969 rows)"
        echo "Warning - problem running on ALL subbands;  master list is too short (is $check_number but should be 3969 rows)" >> $log
      else
        ./run.sh &
      fi
      cd $location
fi

echo "Starting folding"
date

#Fold the data
for ii in $num_dir
do
   cd ${location}/${STOKES}/RSP${ii}
   prepfold -noxwin -psr ${PULSAR} -n 256 -fine -nopdsearch -o ${PULSAR}_${OBSID}_RSP${ii} ${PULSAR}_${OBSID}_RSP${ii}.sub[0-9]* >> ${PULSAR}_${OBSID}_RSP${ii}.prepout 2>&1 && touch "DONE" >> ${PULSAR}_${OBSID}_RSP${ii}.prepout 2>&1 &
   sleep 5
done

cd ${location}

#Check when all 8 DONE files are available, then all processes have exited
ii=1
yy=0
zz=0
while [ $ii -ne $yy ]
do
   #When the first folding task finishes, start the folding for the "all" directory
   if [ $all -eq 1 ] && [ -e ${STOKES}/RSP0/DONE ] && [ $zz -eq 0 ]
   then
      zz=1
      cd ${location}/${STOKES}/RSPA
      prepfold -noxwin -psr ${PULSAR} -n 256 -fine -nopdsearch -o ${PULSAR}_${OBSID}_RSPA ${PULSAR}_${OBSID}_RSPA.sub[0-9]* >> ${PULSAR}_${OBSID}_RSPA.prepout 2>&1 && touch "DONE" >> ${PULSAR}_${OBSID}_RSPA.prepout 2>&1 &
      cd ${location}
   fi
   
   if [ -e $done_list ]
   then
      echo "All prepfold RSP0-7 tasks have completed!" 
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
echo "Running plot summary script plot_profs8.py"
date
python ${LOFARSOFT}/release/share/pulsar/bin/plot_profs8.py
convert profiles.ps ${PULSAR}_${OBSID}_profiles.pdf
rm profiles.ps

#Make a .pdf version of the plots
echo "Running convert on ps to pdf of the plots"
date
for ii in $num_dir
do
   convert ${STOKES}/RSP${ii}/${PULSAR}_${OBSID}_RSP${ii}_PSR_${PULSAR}.pfd.ps ${STOKES}/RSP${ii}/${PULSAR}_${OBSID}_RSP${ii}_PSR_${PULSAR}.pfd.pdf
done

#RFI-Report
if [ $rfi -eq 1 ]
then 
   echo "Producing rfi report"
   date
   for ii in $num_dir
   do
      cd ${location}/${STOKES}/RSP${ii}
      python ${LOFARSOFT}/release/share/pulsar/bin/subdyn.py --saveonly -n `echo ${SAMPLES}*10 | bc` *.sub0???  && touch DONE &
      set subdyn_pid_$ii=$!
   done

   cd ${location}

   #Check when all 8 DONE files are available, then all processes have exited
   for ii in $num_dir
   do
      echo "Waiting for loop $ii subdyn to finish"
      wait $subdyn_pid_$ii
   done
fi

#Wait for the all prepfold to finish
if [ $all -eq 1 ] 
then
   ii=1
   yy=0
   echo "Wiating for prepfold on entire subband list to complete..." 
   while [ $ii -ne $yy ]
   do
      if [ -e ${STOKES}/RSPA/DONE ]
      then
         echo "prepfold on the total list has completed!" 
         yy=1
      fi
      sleep 15
   done
   convert ${STOKES}/RSPA/${PULSAR}_${OBSID}_RSPA_PSR_${PULSAR}.pfd.ps ${STOKES}/RSPA/${PULSAR}_${OBSID}_RSPA_PSR_${PULSAR}.pfd.pdf
fi

#Make a tarball of all the plots
echo "Creating tar file of plots"
date
tar cvf ${PULSAR}_${OBSID}_plots.tar *profiles.pdf ${STOKES}/RSP*/*pfd.ps ${STOKES}/RSP*/*pfd.pdf ${STOKES}/RSP*/*pfd.bestprof ${STOKES}/RSP*/*.sub.inf
gzip ${PULSAR}_${OBSID}_plots.tar

#Change permissions and move files
echo "Changing permissions of files"
date
chmod -R 774 * 
chgrp -R pulsar * 
mv *.tar.gz *.pdf ${STOKES}

date_end=`date`
echo "Start Time: " $date_start
echo "End Time: " $date_end

echo "End date: $date_end" >> $log

exit 0
