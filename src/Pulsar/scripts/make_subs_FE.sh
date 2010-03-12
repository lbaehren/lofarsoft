#Convert raw LOFAR data
#Assumes 16 channels per subband and divides into 4 chunks of 62 subbands

#Set these parameters by hand
OBSID=L2009_16268
PARSET=/globalhome/lofarsystem/log/L2009_16268/RTCP.parset.0
DISK=data3
STOKES=stokes
#STOKES=stokes
COLLAPSE=              #To collapse put "-C" here

######################
# DON'T CHANGE BELOW #
######################

A=`cat $PARSET | grep "OLAP.storageStationNames" | awk -F '[' '{print $2}' | awk -F ']' '{print $1}'| awk '{print length}'`
NBEAMS=`echo "($A+1)/9" | bc`
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

rm -rf flysEye
mkdir flysEye
chmod 774 flysEye
chgrp pulsar flysEye
cd flysEye

alias bf2presto=/home/hassall/lofarsoft/release/share/pulsar/bin/bf2presto
#alias bf2presto=~hessels/bf2presto

echo "Starting Time"
date

#Create subband lists
#RSP0 subbands
RSPs='/net/sub[456]/lse*/'${DISK}'/'${OBSID}'/SB*.MS.'${STOKES} 
echo "Starting bf2presto conversion"
date

#Convert the subbands with bf2presto
echo 'Converting subbands: '`ls $RSPs` >> bf2presto.out 2>&1
bf2presto ${COLLAPSE} -b ${NBEAMS} -f 0 -c ${CHAN} -n ${DOWN} -N ${SAMPLES} -o ${PULSAR}_${OBSID} `ls $RSPs | sort -t B -g -k 2` >> bf2presto.out 2>&1
echo 'bf2presto: complete'

for ii in `ls -d beam_?`
do
A=`ls $ii/${PULSAR}_${OBSID}.sub???? | grep "sub" -c`
B=`echo "($A/4)-1" | bc`
for iii in 0 1 2 3
do
mkdir -p ${ii}/RSP${iii}
mv `ls $ii/${PULSAR}_${OBSID}.sub???? | head -1` ${ii}/RSP${iii}/${PULSAR}_${OBSID}.sub0000
mv `ls $ii/${PULSAR}_${OBSID}.sub???? | head -$B` ${ii}/RSP${iii}
done
done

#Create .sub.inf files with par2inf.py
cp $PARSET ./${OBSID}.parset
cp ~hessels/default.inf .
python ~hessels/par2inf.py -S ${PULSAR} -o test -n 62 -r 4 ./${OBSID}.parset
jj=0
for ii in `ls test*.inf`
do
mv ${ii} beam_0/RSP${jj}/${PULSAR}_${OBSID}.sub.inf
jj=`expr $jj + 1`
done

for ii in `ls -d beam_?`
do
if [ "$ii" != "beam_0" ]
then
cp beam_0/RSP0/${PULSAR}_${OBSID}.sub.inf ${ii}/RSP0/${PULSAR}_${OBSID}.sub.inf
cp beam_0/RSP1/${PULSAR}_${OBSID}.sub.inf ${ii}/RSP1/${PULSAR}_${OBSID}.sub.inf
cp beam_0/RSP2/${PULSAR}_${OBSID}.sub.inf ${ii}/RSP2/${PULSAR}_${OBSID}.sub.inf
cp beam_0/RSP3/${PULSAR}_${OBSID}.sub.inf ${ii}/RSP3/${PULSAR}_${OBSID}.sub.inf
fi
done

for ii in `ls -d beam_?`
do
N=`echo $ii | awk -F "_" '{print $2}'`
N=`echo "$N+1" | bc`
NAME=`cat $PARSET| grep "OLAP.storageStationNames" | awk -F '[' '{print $2}' | awk -F ']' '{print $1}'| awk -F "," '{print $'$N'}'`
mv $ii $NAME
done

echo "Starting folding"
date

#Fold the data
for ii in `ls -d *${ARRAY}/RSP?`
do
prepfold -noxwin -psr ${PULSAR} -n 256 -fine -nopdsearch -o ${ii}/${PULSAR}_${OBSID} ${ii}/${PULSAR}_${OBSID}.sub[0-9]* >> ${ii}/${PULSAR}_${OBSID}.prepout 2>&1
echo "folded" $ii
done

#Make a cumulative plot of the profiles
echo python ~hassall/programs/plot_profs.py `cat $PARSET| grep "OLAP.storageStationNames" | awk -F '[' '{print $2}' | awk -F ']' '{print $1}'`
python ~hassall/programs/plot_profs.py `cat $PARSET| grep "OLAP.storageStationNames" | awk -F '[' '{print $2}' | awk -F ']' '{print $1}'`
convert profiles.ps ${PULSAR}_${OBSID}_profiles.pdf
rm profiles.ps

#Make a tarball of all the plots
tar cvf ${PULSAR}_${OBSID}_plots.tar *profiles.pdf *${ARRAY}/RSP*/*pfd.ps *${ARRAY}/RSP*/*pfd.bestprof *${ARRAY}/RSP*/*.sub.inf
gzip ${PULSAR}_${OBSID}_plots.tar

#Change permissions and move files
cd ..
chmod -R 774 flysEye/
chgrp -R pulsar  flysEye/

echo "End Time"
date
