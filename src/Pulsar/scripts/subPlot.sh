#####################################################
# subPlot.sh: Divides observations up into subbands #
# and outputs a matrix so a greyscale freq-phase    #
# diagram can be made                               #
#####################################################

DIR="RS503HBA"
RSP="*"
FREQ="138.96484375"

mkdir -p subPlot_workings
rm subPlot_workings/*
rm subPlot.out

A=`ls $DIR/*/*.sub[0-9]* | grep "sub" -c`
A=`echo $A+1|bc`

for ii in `seq $A -16 0`
do
FREQ=`echo "scale=7; $FREQ+0.1953125" | bc`
echo "Data file name without suffix          =  1508+55" > subPlot_workings/B1508+55_L2009_16209.sub.inf
echo " Telescope used                         =  GBT" >> subPlot_workings/B1508+55_L2009_16209.sub.inf
echo " Instrument used                        =  STELLA">> subPlot_workings/B1508+55_L2009_16209.sub.inf
echo " Object being observed                  =  B1508+55">> subPlot_workings/B1508+55_L2009_16209.sub.inf
echo " J2000 Right Ascension (hh:mm:ss.ssss)  =  15:09:25.621200">> subPlot_workings/B1508+55_L2009_16209.sub.inf
echo " J2000 Declination     (dd:mm:ss.ssss)  =  55:31:32.329200">> subPlot_workings/B1508+55_L2009_16209.sub.inf
echo " Data observed by                       =  Mr. LOFAR">> subPlot_workings/B1508+55_L2009_16209.sub.inf
echo " Epoch of observation (MJD)             =  55192.323831">> subPlot_workings/B1508+55_L2009_16209.sub.inf
echo " Barycentered?           (1=yes, 0=no)  =  1">> subPlot_workings/B1508+55_L2009_16209.sub.inf
echo " Number of bins in the time series      =  999">> subPlot_workings/B1508+55_L2009_16209.sub.inf
echo " Width of each time series bin (sec)    =  0.00131072">> subPlot_workings/B1508+55_L2009_16209.sub.inf
echo " Any breaks in the data? (1=yes, 0=no)  =  0">> subPlot_workings/B1508+55_L2009_16209.sub.inf
echo " Type of observation (EM band)          =  Radio">> subPlot_workings/B1508+55_L2009_16209.sub.inf
echo " Beam diameter (arcsec)                 =  999.0">> subPlot_workings/B1508+55_L2009_16209.sub.inf
echo " Dispersion measure (cm-3 pc)           =  0.0">> subPlot_workings/B1508+55_L2009_16209.sub.inf
echo " Central freq of low channel (MHz)      =  " $FREQ>> subPlot_workings/B1508+55_L2009_16209.sub.inf
echo " Total bandwidth (MHz)                  =  0.1953125">> subPlot_workings/B1508+55_L2009_16209.sub.inf
echo " Number of channels                     =  16">> subPlot_workings/B1508+55_L2009_16209.sub.inf
echo " Channel bandwidth (MHz)                =  0.01220703125">> subPlot_workings/B1508+55_L2009_16209.sub.inf
echo " Data analyzed by                       =  subPlot">> subPlot_workings/B1508+55_L2009_16209.sub.inf

cp -v `ls $DIR/*/*.sub[0-9]* | tail -$ii | head -16`  subPlot_workings/
mv -v `ls subPlot_workings/*sub* | head -1`  subPlot_workings/B1508+55_L2009_16209.sub0000
SNR=`prepfold  subPlot_workings/B1508+55_L2009_16209.sub[0-9]* -psr B1508+55 -nopdsearch -fine -noxwin | grep "Maximum reduced chi-squared found" | awk -F "Maximum reduced chi-squared found  =  " '{print $1}'`
python ~/programs/rotate.py subPlot_workings/*.bestprof >> ${DIR}_subPlot.out
echo $FREQ $SNR >> ${DIR}_subPlot.snr
rm subPlot_workings/*sub*
done