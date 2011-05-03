#
from scipy.optimize import fmin

#$PYP/scripts/fix_metadata.py --antenna_set="LBA_OUTER" --nyquist_zone=1 oneshot_level4_CS017_19okt_no-9.h5 
#execfile('/Users/falcke/LOFAR/usg/src/CR-Tools/implement/Pypeline/scripts/testpulsecal.py')
file=open("$LOFARSOFT/data/lofar/oneshot_level4_CS017_19okt_no-9.h5")
file["ANTENNA_SET"]="LBA_OUTER"
file["BLOCKSIZE"]=2**17

evenantennas=["017000000","017000003","017000004","017000006","017001008","017001011","017001013","017001014","017002016","017002018","017002021","017002022","017003024","017003027","017003028","017003030","017004032","017004034","017004036","017004038","017005040","017005042","017005044","017005046","017006048","017006050","017006052","017006054","017007056","017007058","017007060","017007062","017008064","017008067","017008068","017008070","017009072","017009074","017009076","017009078","017010080","017010082","017010084","017010086","017011088","017011090","017011092","017011094"]

oddantennas=["017000001","017000002","017000005","017000007","017001009","017001010","017001012","017001015","017002017","017002019","017002020","017002023","017003025","017003026","017003029","017003031","017004033","017004035","017004037","017004039","017005041","017005043","017005045","017005047","017006049","017006051","017006053","017006055","017007057","017007059","017007061","017007063","017008065","017008066","017008069","017008071","017009073","017009075","017009077","017009079","017010081","017010083","017010085","017010087","017011089","017011091","017011093","017011095"]

file["SELECTED_DIPOLES"]=oddantennas
timeseries_data=file["TIMESERIES_DATA"]

########################################################################
# Use the following to determing xx and yy data sets, i.e. determin
# which are the real odd and even antennas. Unfortunately in this data
# set some polarizations are switched ...  This makes use of the fact
# that the pulse shape in the data set is different for the two
# polarisations (and hence one has two values for the
# crosscorrelation).
########################################################################

# file["SELECTED_DIPOLES"]=[f for f in file["DIPOLE_NAMES"] if int(f)%2==1] # select uneven antenna IDs
# file["SELECTED_DIPOLES"]=file["DIPOLE_NAMES"]
# timeseries_data=file["TIMESERIES_DATA"]
# (start,end)=trun("LocatePulseTrain",rf.TimeBeamIncoherent(timeseries_data),nsigma=7,maxgap=3)
# start-=16; end=start+int(2**ceil(log(end-start,2)));  
# timeseries_data_cut=hArray(float,[timeseries_data.shape()[-2],end-start])
# timeseries_data_cut[...].copy(timeseries_data[...,start:end])
# timeseries_data_cut[...]-=timeseries_data_cut[...].mean()
# timeseries_data_cut[...]/=timeseries_data_cut[...].stddev(0)


# crosscorr_data=tload("CrossCorrelateAntennas",False)(timeseries_data_cut,refant=0).crosscorr_data
# crosscorr_data.abs()
# crosscorr_data[...].runningaverage(15,hWEIGHTS.GAUSSIAN)
# mx=trun("FitMaxima",crosscorr_data,doplot=True,refant=0,plotend=96,sampleinterval=5)
# dividing_line=mx.maxy.mean()
# if not mx.maxy.countgreaterthan(dividing_line)==48:
#     print "This was not a good pulse - dividing line was not sharp" 

# evenantennas=list(hArray(file["DIPOLE_NAMES"])[mx.maxy.find_locations(">",dividing_line)])
# oddantennas=list(hArray(file["DIPOLE_NAMES"])[mx.maxy.find_locations("<",dividing_line)])
# file["SELECTED_DIPOLES"]=oddantennas
########################################################################
# END Polarization check
########################################################################

#First determine where the pulse is in a simple incoherent sum of all time series data

# trun loads the task LocatePulseTrain - see $LOFARSOFT/src/CR-Tools/implement/Pypeline/modules/tasks/pulsecal.py
# rf is a module that contains a few helper functions for radio interferometry
(start,end)=trun("LocatePulseTrain",rf.TimeBeamIncoherent(timeseries_data),nsigma=7,maxgap=3)
start-=16; end=start+int(2**ceil(log(end-start,2)));

#Cut the data around the main pulse
timeseries_data_cut=hArray(float,[timeseries_data.shape()[-2],end-start])
timeseries_data_cut[...].copy(timeseries_data[...,start:end])
timeseries_data_cut[...]-=timeseries_data_cut[...].mean()
timeseries_data_cut[...]/=timeseries_data_cut[...].stddev(0)

#Now use the beamformer task (here loaded with 'tload' and then
#immediately executed, the return value is the beamformer task itself
#containing all the variables as bf.var...)

# Beamform into the direction found by Arthur of the pulse
bf=tload("BeamFormer2",False)(data=timeseries_data_cut,antennas=file["SELECTED_DIPOLES"],antpos=file["ANTENNA_POSITIONS"],FarField=False,sample_interval=file["SAMPLE_INTERVAL"][0],pointings=[dict(az=143.4092*deg,el= 81.7932*deg, r=600.3)],calc_timeseries=True,doplot=2,plotspec=False,verbose=False)
raw_input("Press key")

##Note at this point one should use the new function
##"hDirectionTriangulations" - see mRF.cc or the help on this function
##to get an example how to use it and to get a direction estimate
##right away.

#bf.fftdata will contain the FFT data of the time series with the
#phase gradient of the beamformer applied, i.e. the shifted data which
#in principle should be perfectly alliged if the telescope is
#well-calibrated

#Now cross-correlate the time series data to get the residual delays

crosscorr_data=tload("CrossCorrelateAntennas",False)(None,fft_data=bf.fftdata).crosscorr_data
crosscorr_data.abs()
crosscorr_data[...].runningaverage(15,hWEIGHTS.GAUSSIAN)

#Determine the maxima of the cross-correlation (note that I smooth
#here over several peaks in the cross-correlation - that needs to be
#tested.

mx=trun("FitMaxima",crosscorr_data,doplot=True,refant=0,plotend=48,sampleinterval=5)
print "Lags =",list(mx.lags),"nanoseconds."
raw_input("Press key")


#Lags = [0.0, -2.5, -2.5, -4.0625, -5.625, -4.0625, -1.5625, -1.875, -4.0625, -4.375, -1.25, -5.3125, -5.3125, -1.875, -3.125, -2.5, -10.0, -4.0625, -5.0, -1.25, -3.125, -3.125, -4.6875, -6.875, -5.625, -5.9375, -5.625, -3.125, -4.0625, -7.5, -6.5625, -5.3125, -10.625, -4.375, -4.0625, -6.5625, -10.0, -4.6875, -6.25, -8.4375, -13.125, -5.9375, -6.25, -7.8125, -13.125, -4.6875, -4.0625, -5.625] nanoseconds.

#Put the lags into units of seconds
lags=mx.lags * 10**-9

#Beam-form again with the new delays applied
bf=tload("BeamFormer2",False)(data=timeseries_data_cut,antennas=file["SELECTED_DIPOLES"],antpos=file["ANTENNA_POSITIONS"],FarField=False,sample_interval=file["SAMPLE_INTERVAL"][0],pointings=[dict(az=143.4092*deg,el= 81.7932*deg, r=600.3)],cable_delays=lags,calc_timeseries=True,doplot=2,plotspec=False,verbose=False)
raw_input("Press key")


#Cross correlate and check if the alignment is better now ...
crosscorr_data=tload("CrossCorrelateAntennas",False)(None,fft_data=bf.fftdata).crosscorr_data
crosscorr_data.abs()
crosscorr_data[...].runningaverage(10,hWEIGHTS.GAUSSIAN)
mx=trun("FitMaxima",crosscorr_data,doplot=True,refant=0,plotstart=2,plotend=4,sampleinterval=5)
print "Lags =",list(mx.lags),"nanoseconds."
raw_input("Press key")


#etc.,etc. - this can be done iteratively ...
lags2=mx.lags * 10**-9

bf=tload("BeamFormer2",False)(data=timeseries_data_cut,antennas=file["SELECTED_DIPOLES"],antpos=file["ANTENNA_POSITIONS"],FarField=False,sample_interval=file["SAMPLE_INTERVAL"][0],pointings=[dict(az=143.4092*deg,el= 81.7932*deg, r=600.3)],cable_delays=lags+lags2,calc_timeseries=True,doplot=2,plotspec=False,verbose=False)

raw_input("Press key")
bf_fit=task("BeamFormer2",data=timeseries_data_cut,antennas=file["SELECTED_DIPOLES"],antpos=file["ANTENNA_POSITIONS"],FarField=False,sample_interval=file["SAMPLE_INTERVAL"][0],cable_delays=lags+lags2,calc_timeseries=False,calc_tbeams=True,doplot=False,doabs=True,smooth_width=5,verbose=False)
mx_fit=task("FitMaxima",doplot=False,peak_width=10)

x0=np.array([143.4092,81.7932,log10(600.3)])
fitfunc=lambda x0:-asval(mx_fit(bf_fit(pointings=[dict(az=x0[0]*deg,el=x0[1]*deg, r=10**x0[2])]).tbeams).maxy)
(xmin,ymin,niter,neval,success)=fmin(fitfunc,x0,maxiter=100,full_output=True)

bf=tload("BeamFormer2",False)(data=timeseries_data_cut,antennas=file["SELECTED_DIPOLES"],antpos=file["ANTENNA_POSITIONS"],FarField=False,sample_interval=file["SAMPLE_INTERVAL"][0],pointings=[dict(az=xmin[0]*deg,el=xmin[1]*deg, r=10**xmin[2])],cable_delays=lags+lags2,calc_timeseries=True,doplot=2,plotspec=False,verbose=False)

crosscorr_data=tload("CrossCorrelateAntennas",False)(None,fft_data=bf.fftdata).crosscorr_data
crosscorr_data.abs()
crosscorr_data[...].runningaverage(10,hWEIGHTS.GAUSSIAN)
mx=trun("FitMaxima",crosscorr_data,doplot=True,refant=0,plotstart=2,plotend=4,sampleinterval=5)
print "Lags =",list(mx.lags),"nanoseconds."
raw_input("Press key")

lags3=mx.lags * 10**-9

bf=tload("BeamFormer2",False)(data=timeseries_data_cut,antennas=file["SELECTED_DIPOLES"],antpos=file["ANTENNA_POSITIONS"],FarField=False,sample_interval=file["SAMPLE_INTERVAL"][0],pointings=[dict(az=xmin[0]*deg,el=xmin[1]*deg, r=10**xmin[2])],cable_delays=lags+lags2+lags3,calc_tbeams=True,doabs=True,smooth_width=3,calc_timeseries=True,doplot=2,plotspec=False,verbose=False)
raw_input("Press key")

lags0=Vector(properties=lags)
asval(mx_fit(bf_fit(pointings=[dict(az=x0[0]*deg,el=x0[1]*deg, r=10**x0[2])],cable_delays=lags0).tbeams).maxy)
asval(mx_fit(bf_fit(pointings=[dict(az=x0[0]*deg,el=x0[1]*deg, r=10**x0[2])],cable_delays=lags).tbeams).maxy)
asval(mx_fit(bf_fit(pointings=[dict(az=x0[0]*deg,el=x0[1]*deg, r=10**x0[2])],cable_delays=lags+lags2).tbeams).maxy)
asval(mx_fit(bf_fit(pointings=[dict(az=xmin[0]*deg,el=xmin[1]*deg, r=10**xmin[2])],cable_delays=lags+lags2).tbeams).maxy)
asval(mx_fit(bf_fit(pointings=[dict(az=xmin[0]*deg,el=xmin[1]*deg, r=10**xmin[2])],cable_delays=lags+lags2+lags3).tbeams).maxy)

bf_fit=task("BeamFormer2",data=timeseries_data_cut,antennas=file["SELECTED_DIPOLES"],antpos=file["ANTENNA_POSITIONS"],FarField=False,sample_interval=file["SAMPLE_INTERVAL"][0],cable_delays=lags,calc_timeseries=False,calc_tbeams=True,doplot=False,doabs=True,smooth_width=5,verbose=False)
(xmin2,ymin2,niter2,neval2,success2)=fmin(fitfunc,xmin,maxiter=100,full_output=True,disp=True)

# bf.data_shifted.abs()
# bf.data_shifted[...].runningaverage(7,hWEIGHTS.GAUSSIAN)
# mx=tload("FitMaxima",0); mx(bf.data_shifted,doplot=True,refant=0,sampleinterval=5,plotend=96,peak_width=10,legend=file["SELECTED_DIPOLES"])



# bf.fftdata.mul(bf.weights[bf.mainbeam])

# ccd=tload("CrossCorrelateAntennas",0); ccd(None,fft_data=bf.fftdata)

# ccd.crosscorr_data.abs()
# ccd.crosscorr_data[...].runningaverage(15,hWEIGHTS.GAUSSIAN)
# mx=trun("FitMaxima",ccd.crosscorr_data,doplot=True,refant=0,sampleinterval=5,plotend=6)
# print "Lags =",list(mx.lags),"nanoseconds."
