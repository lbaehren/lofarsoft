#Convert raw LOFAR data
#Assumes 16 channels per subband and divides into 4 chunks of 62 subbands

#Set these parameters by hand
OBSID=L2009_16103
PARSET=/globalhome/lofarsystem/log/L2009_16102/RTCP.parset.1
DISK=data2
STOKES=incoherentstokes
#STOKES=stokes
COLLAPSE=              #To collapse put "-C" here

######################
# DON'T CHANGE BELOW #
######################


PULSAR=`cat $PARSET | grep "Observation.pulsarsrc" | awk -F "= " '{print$2}'`
ARRAY=`cat $PARSET | grep "Observation.bandFilter" | awk -F "= " '{print $2}' | awk -F "_" '{print $1}'`
CHAN=`cat $PARSET | grep "Observation.channelsPerSubband" | awk -F "= " '{print $2}'`
DOWN=`cat $PARSET | grep "OLAP.Stokes.integrationSteps" | awk -F "= " '{print $2}'`
MAGIC_NUM=`cat $PARSET | grep "OLAP.CNProc.integrationSteps" | awk -F "= " '{print $2}'`
SAMPLES=`echo ${MAGIC_NUM}/${DOWN}| bc`

echo "PULSAR:" $PULSAR 
echo "ARRAY:" $ARRAY 
echo "CHANNELS:" $CHAN 
echo "Number of SAMPLES:" $SAMPLES

alias bf2presto=/home/hassall/lofarsoft/release/share/pulsar/bin/bf2presto
#alias bf2presto=~hessels/bf2presto

echo "Starting Time"
date

#Create directories with appropriate permissions
for ii in 0 1 2 3
do
mkdir -p ${STOKES}/RSP$ii
chmod 774 ${STOKES}/RSP$ii
chgrp pulsar ${STOKES}/RSP$ii
done

#Create subband lists
#RSP0 subbands
RSP0='/net/sub[456]/lse*/'${DISK}'/'${OBSID}'/SB[0-9].MS.'${STOKES}' /net/sub[456]/lse*/'${DISK}'/'${OBSID}'/SB[0-5][0-9].MS.'${STOKES}' /net/sub[456]/lse*/'${DISK}'/'${OBSID}'/SB6[0-1].MS.'${STOKES}
#RSP1 subbands
RSP1='/net/sub[456]/lse*/'${DISK}'/'${OBSID}'/SB6[2-9].MS.'${STOKES}' /net/sub[456]/lse*/'${DISK}'/'${OBSID}'/SB[7-9][0-9].MS.'${STOKES}' /net/sub[456]/lse*/'${DISK}'/'${OBSID}'/SB1[0-1][0-9].MS.'${STOKES}' /net/sub[456]/lse*/'${DISK}'/'${OBSID}'/SB12[0-3].MS.'${STOKES}
#RSP2 subbands
RSP2='/net/sub[456]/lse*/'${DISK}'/'${OBSID}'/SB12[4-9].MS.'${STOKES}' /net/sub[456]/lse*/'${DISK}'/'${OBSID}'/SB1[3-7][0-9].MS.'${STOKES}' /net/sub[456]/lse*/'${DISK}'/'${OBSID}'/SB18[0-5].MS.'${STOKES}
#RSP3 subbands
RSP3='/net/sub[456]/lse*/'${DISK}'/'${OBSID}'/SB18[6-9].MS.'${STOKES}' /net/sub[456]/lse*/'${DISK}'/'${OBSID}'/SB19[0-9].MS.'${STOKES}' /net/sub[456]/lse*/'${DISK}'/'${OBSID}'/SB2[0-9][0-9].MS.'${STOKES}

echo "Starting bf2presto conversion"
date

#Convert the subbands with bf2presto
#RSP0 conversion
echo 'Converting subbands: '`ls $RSP0` >> ${STOKES}/RSP0/bf2presto_RSP0.out 2>&1
bf2presto ${COLLAPSE} -f 0 -c ${CHAN} -n ${DOWN} -N ${SAMPLES} -o ${STOKES}/RSP0/${PULSAR}_${OBSID}_RSP0 `ls $RSP0 | sort -t B -g -k 2` >> ${STOKES}/RSP0/bf2presto_RSP0.out 2>&1
echo `date` "RSP0 Done"
#RSP1 conversion
echo 'Converting subbands: '`ls $RSP1` >> ${STOKES}/RSP1/bf2presto_RSP1.out 2>&1
bf2presto ${COLLAPSE} -f 0 -c ${CHAN} -n ${DOWN} -N ${SAMPLES} -o ${STOKES}/RSP1/${PULSAR}_${OBSID}_RSP1 `ls $RSP1 | sort -t B -g -k 2` >> ${STOKES}/RSP1/bf2presto_RSP1.out 2>&1
echo `date` "RSP1 Done"
#RSP2 conversion
echo 'Converting subbands: '`ls $RSP2` >> ${STOKES}/RSP2/bf2presto_RSP2.out 2>&1
bf2presto ${COLLAPSE} -f 0 -c ${CHAN} -n ${DOWN} -N ${SAMPLES} -o ${STOKES}/RSP2/${PULSAR}_${OBSID}_RSP2 `ls $RSP2 | sort -t B -g -k 2` >> ${STOKES}/RSP2/bf2presto_RSP2.out 2>&1
echo `date` "RSP2 Done"
#RSP3 conversion
echo 'Converting subbands: '`ls $RSP3` >> ${STOKES}/RSP3/bf2presto_RSP3.out 2>&1
bf2presto ${COLLAPSE} -f 0 -c ${CHAN} -n ${DOWN} -N ${SAMPLES} -o ${STOKES}/RSP3/${PULSAR}_${OBSID}_RSP3 `ls $RSP3 | sort -t B -g -k 2` >> ${STOKES}/RSP3/bf2presto_RSP3.out 2>&1
echo `date` "RSP3 Done"

#Create .sub.inf files (OLD)
#for ii in 0 1 2 3
#do
#cp ~hessels/RSP0_${ARRAY}.sub.inf RSP${ii}/${PULSAR}_${OBSID}_RSP${ii}.sub.inf
#done

#Create .sub.inf files with par2inf.py
cp $PARSET ./${OBSID}.parset
cp ~hessels/default.inf .
python ~hessels/par2inf.py -S ${PULSAR} -o test -n 62 -r 4 ./${OBSID}.parset
jj=0
for ii in `ls test*.inf`
do
mv ${ii} ${STOKES}/RSP${jj}/${PULSAR}_${OBSID}_RSP${jj}.sub.inf
jj=`expr $jj + 1`
done

echo "Starting folding"
date

#Fold the data
for ii in 0 1 2 3
do
prepfold -noxwin -psr ${PULSAR} -n 256 -fine -nopdsearch -o ${STOKES}/RSP${ii}/${PULSAR}_${OBSID}_RSP${ii} ${STOKES}/RSP${ii}/${PULSAR}_${OBSID}_RSP${ii}.sub[0-9]* >> ${STOKES}/RSP${ii}/${PULSAR}_${OBSID}_RSP${ii}.prepout 2>&1
done

#Make a cumulative plot of the profiles
python ~hessels/plot_profs.py
convert profiles.ps ${PULSAR}_${OBSID}_profiles.pdf
rm profiles.ps

#Make a .pdf version of the plots
for ii in 0 1 2 3
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

echo "End Time"
date
