#! /usr/bin/env python

from pycrtools import *

def p_(var):
    if (type(var)==list): map(lambda x:p_(x),var)
    else: print " ",var,"=>",eval(var)

filename_sun=LOFARSOFT+"/data/lopes/example.event"
filename_biglofar=LOFARSOFT+"/data/lofar/rw_20080701_162002_0109.h5"
filename_lofar=LOFARSOFT+"/data/lofar/trigger-2010-02-11/triggered-pulse-2010-02-11-TBB1.h5"
figno=0

plt.clf()
#------------------------------------------------------------------------
filename_cr=LOFARSOFT+"/data/lopes/2004.01.12.00:28:11.577.event"
cr_direction=[41.9898208, 64.70544, 1750.]
XC=-25.44; YC=8.94; #shower core in KASCADE coordinates
antennalist=[0,1,2,3,4,5,6,7]

#	// Rotation by 15.25 degrees
#	// (GPS measurement of angle between KASCADE and LOPES coordinate system)
#	// remark: the value in the Gauss-Kuerger coordinate system is 15.7 Degree
#	// which would correspond to 15.23 Degree to geographic north
#	// Value used in the KRETA-evaluation is 15.0 Degree.
#	// cos 15.25 Degree = 0.964787323  ;  sin 15.25 Degree = 0.263031214
#	XCn = XC*0.964787323+YC*0.263031214;
#	YCn = XC*-0.263031214+YC*0.964787323;

cr_shower_core=[XC*0.964787323 + YC*0.263031214,-XC*0.263031214 + YC*0.964787323,0.0] # first version
#cr_shower_core=[-XC*0.263031214 + YC*0.964787323,XC*0.964787323 + YC*0.263031214,0.0]
#FarField=True
FarField=False

ws=CRMainWorkSpace(filename=filename_cr,fittype="POLY",ncoeffs=8,nbins=1024,doplot=False,verbose=False,modulename="ws")  
ws.makeFitBaseline(ws,logfit=True,fittype="BSPLINE",nbins=256) #fittype="POLY" or "BSPLINE"

if ws["datafile"]["Observatory"]=='LOFAR':
    ws["numin"]=12 #MHz
    ws["numax"]=82 #MHz
elif ws["datafile"]["Observatory"]=='LOPES':
    ws["numin"]=45 #MHz
    ws["numax"]=73 #MHz


cr=ws["datafile"]
"""

Now we read in the raw time series data as well as the Frequencies and
Time value arrays.

"""
cr_time=cr["Time"].setUnit("\\mu","s")

#ws["frequency"]=cr["Frequency"] 
#Attention don't do this here when later using it in caluclating
#phases that require seconds and Hz as units

ws["efield"]=cr["Fx"].setPar("xvalues",cr_time)
ws["efield_processed"] = hArray(properties=ws["efield"])
"""

As a next step we create an empty vector to hold the Fourierspectrum
of the data

"""
ws["fft"]=cr["emptyFFT"].setPar("xvalues",ws["frequency"])
"""

and then make the Fourier transform followed by a reordering of the
output, noting that the data was taken in the second Nyquist domain

"""
ws["fft"][...].fftw(ws["efield"][...])
ws["fft"][...].nyquistswap(cr["nyquistZone"])
"""
Let's create a spectrum that we can plot:
"""
ws["spectrum"]=hArray(float,ws["fft"],xvalues=ws["frequency"],fill=0,name="Power")
ws["spectrum"].par.logplot="y"
ws["spectrum"].spectralpower(ws["fft"])
"""

Now we fit a baseline and fight RFI
"""
ws["meanrms"]=ws["coeffs"].crfitbaseline(ws["frequency"],ws["spectrum"],ws)
ws["baseline"].crcalcbaseline(ws["frequency"],ws["numin_i"],ws["numax_i"],ws["coeffs"],ws)

ws["spectrum"] /= ws["baseline"]
ws["fft"] /= ws["baseline"]
ws["spectrum"][0].plot()

ws["meanspec"]=ws["spectrum"][...,ws["numin_i"]:ws["numax_i"]].meaninverse()
ws["rfithreshold"] = (ws["meanrms"] * ws["rfi_nsigma"]) + ws["meanspec"]
ws["nbad_channels"]=ws["bad_channels"][...].findgreaterthan(ws["spectrum"][...],ws["rfithreshold"])
ws["fft"][...].set(ws["bad_channels"][...,[0]:ws["nbad_channels"]],ws["meanspec"])
ws["spectrum"][...].set(ws["bad_channels"][...,[0]:ws["nbad_channels"]],ws["meanspec"])
ws["spectrum"][0].plot(clf=False)
raw_input("Plotted spectrum - press Enter to continue...")

t1=cr_time.findlowerbound(-1.95).val()+3
t2=t1+32

#scrtfft=hArray(copy=ws["fft"])
#scrtfft.nyquistswap(ws["datafile"]["nyquistZone"])
#scrtfft /= ws["blocksize"]
#ws["efield_processed"][...].invfftw(scrtfft[...])

#ws["efield_processed"][...].saveinvfftw(ws["fft"][...],ws["datafile"]["nyquistZone"])
ws["efield_processed"][...].invfftcasa(ws["fft"][...],ws["datafile"]["nyquistZone"])

ws["efield_processed"][0:2,...,t1:t2].plot(xvalues=cr_time[t1:t2])
ws["efield"][0:2,...,t1:t2].plot(xvalues=cr_time[t1:t2],clf=False)
raw_input("Plotted inverse FFT - press Enter to continue...")


"""
Now we do a cross-correlation and calculate a few time delays.

First we calculate indices for intervals

"""
ws["timelags"]=ws["datafile"]["TimeLag"].setUnit("\\mu","")


ws["crosscorr"]=hArray(float,[ws["nofAntennas"],(t2-t1)],name="Cross Correlation")
ws["crosscorr_smooth"]=hArray(properties=ws["crosscorr"])

ws["crosscorr_cmplx"]=hArray(complex,[ws["nofAntennas"],(t2-t1)/2+1])
ws["crosscorr_cmplx"][...].fftcasa(ws["efield"][...,t1:t2],2)
#ws["crosscorr_cmplx"][...].fftw(ws["efield_processed"][...,t1:t2])
#ws["crosscorr_cmplx"][...].fftw(ws["efield"][...,t1:t2])
ws["crosscorr_cmplx"][1:,...].crosscorrelatecomplex(ws["crosscorr_cmplx"][[0],...])
ws["crosscorr_cmplx"][0].crosscorrelatecomplex(ws["crosscorr_cmplx"][0])
#ws["crosscorr"][...].saveinvfftw(ws["crosscorr_cmplx"][...],1)
ws["crosscorr"][...].invfftcasa(ws["crosscorr_cmplx"][...],2)
ws["crosscorr"].abs()
ws["crosscorr_smooth"].runningaverage(ws["crosscorr"],5,hWEIGHTS.GAUSSIAN) #or .FLAT)# .or LINEAR) # or .GAUSSIAN)
maxpos=hArray(float,8)
maxpos.vec().copy(ws["crosscorr_smooth"][...].maxpos())
maxpos -= (t2-t1)/2
maxpos *= 12.5/1000.

ws["crosscorr_smooth"][...].plot()
raw_input("Plotted cross-correlations - press Enter to continue...")
"""



(++) Coordinates
---------------

For doing actual beamforming we need to know the antenna
positions. For this we will use the method getCalData:

"""
antenna_positions=cr.getCalData("Position")
p_("antenna_positions")
"""

As a next step we need to put the antenna coordinates on a reference
frame that is relative to the phase center. Here we will choose the
location of the first antenna as our phase center (that makes life a
little easier for checking) and we simply subtract the reference
position from the antenna locations so that our phase center lies at
0,0,0.

"""
phase_center=hArray(cr_shower_core)
antenna_positions -= phase_center
"""

Now we read in instrumental delays for each antenna in a similar way
and store it for later use.

"""
cal_delays_caltable=cr.getCalData("Delay")

"""
However, we actually also need to take care of sample-jumps that
occured in LOPES, which were calculated using a calibration on a TV
transmitter signal. Asking Andreas Horneffer, we get the following
correct delays:
"""
horneffer_delays=hArray([0.00,  -1.87,  -0.22,  -0.30,  -2.02,  -2.06,   1.05,  -1.35])
horneffer_delays *= 12.5*10**-9
#cal_delays=cal_delays_caltable
cal_delays=horneffer_delays
p_("cal_delays")
"""

Now let us look at the actual beam forming:

We first turn the coordinates into a std vector and create a vector that is
supposed to hold the Cartesian coordinates. Note that the AzEL vector
is actually AzElRadius, where we need to set the radius to unity.

"""
azel=hArray(cr_direction,dimensions=[1,3])
cartesian=azel.new()
"""

We now convert the Azimuth-Elevation position into a vector in
Cartesian coordinates, which is what is used by the beamformer.
"""

hCoordinateConvert(azel[...],CoordinateTypes.AzElRadius,cartesian[...],CoordinateTypes.Cartesian,True)
"""

Then calculate geometric delays and add the instrumental delays.

"""
delays=hArray(float,dimensions=cal_delays)
hGeometricDelays(delays,antenna_positions,cartesian,FarField)
"""

To get the total delay we add the geometric and the calibration delays.

"""
delays += cal_delays
"""

The delays can be converted to phases of coplex weights (to be applied
in the Fourier domain).

"""
phases=hArray(float,dimensions=ws["fft"],name="Phases",xvalues=ws["frequency"])
ws["frequency"].setUnit("","")
phases.delaytophase(ws["frequency"],delays)
ws["frequency"].setUnit("M","")
"""
#hGeometricPhases(phases,ws["frequency"],antenna_positions,cartesian,FarField)

Similarly, the corresponding complex weights are calculated.

"""
weights=hArray(complex,dimensions=ws["fft"],name="Complex Weights")
weights.phasetocomplex(phases)
"""
#hGeometricWeights(weights,ws["frequency"],antenna_positions,cartesian,FarField)

To shift the time series data (or rather the FFTed time series data)
we multiply the fft data with the complex weights from above.

"""
cr_calfft_shifted=hArray(copy=ws["fft"])
cr_calfft_shifted *= weights
"""

Convert back into time domain

"""

ws["efield_shifted"] = cr["emptyFx"].setPar("xvalues",cr_time)

"""
However, before FFTing we need to swap the FFT back, to take account of possibly different Nyquistzones

"""
cr_calfft_shifted[...].nyquistswap(cr["nyquistZone"])
ws["efield_shifted"][...].invfftw(cr_calfft_shifted[...]) # is being destroyed here ....


"""
To make plotting quicker, select the time window you want to
plot. Unfortunately Mathplot plots everything first and then simply
does not display the things that are outside the plotting window -
this is very slow with 8x65000 points.

"""
tb1=cr_time.findlowerbound(-2.0)
tb2=cr_time.findlowerbound(-1.0)

ws["efield_shifted"][...,tb1:tb2].plot(xlim=(-1.95,-1.65),xvalues=cr_time[tb1:tb2],legend=antennalist)
plt.savefig("cr_efields.pdf")
raw_input("Plotted shifted efields - press Enter to continue...")

#ws["efield_shifted"][antennalist,...].plot(xlim=(-1.95,-1.6),legend=antennalist)
#raw_input("Plotted not flagged antennas - press Enter to continue...")


"""
The beamforming we here do by simply adding the data in the time domain 
"""
ws["efield_shifted_added"]=hArray(float,dimensions=cr_time,name="beamformed E-field",xvalues=cr_time)
ws["efield_shifted"][antennalist,...].addto(ws["efield_shifted_added"])
ws["efield_shifted_added"] /= ws["datafile"]["nofAntennas"] #normalize

"""
Finally, we produce a nice, smoothed data curve: first taking the abs
and then using running average.
"""

ws["efield_shifted_added_abs"]=hArray(copy=ws["efield_shifted_added"],xvalues=cr_time)
ws["efield_shifted_added_abs"].abs()
ws["efield_shifted_added_smoothed"]=hArray(float,dimensions=[cr.blocksize],xvalues=cr_time,name="E-Field")
ws["efield_shifted_added_smoothed"].runningaverage(ws["efield_shifted_added_abs"],2,hWEIGHTS.GAUSSIAN) #or .FLAT)# .or LINEAR) # or .GAUSSIAN)
ws["efield_shifted_added_smoothed"][tb1:tb2].plot(xlim=(-2,-1),title=cr.filename,xvalues=cr_time[tb1:tb2],clf=False)
raw_input("Overplotted smoothed beamformed pulse - press Enter to continue...")


ws["efield_shifted_added_abs"][tb1:tb2].plot(xlim=(-2,-1),xvalues=cr_time[tb1:tb2],title=cr.filename,clf=True)
ws["efield_shifted_added_smoothed"][tb1:tb2].plot(xlim=(-2,-1),xvalues=cr_time[tb1:tb2],title=cr.filename,clf=False)
plt.savefig("cr_pulse_zoom.pdf")
raw_input("Plotted final pulse - press Enter to continue...")

ws["efield_shifted_added_abs"].plot(xlim=(-400,400),title=cr.filename,clf=True)
ws["efield_shifted_added_smoothed"].plot(xlim=(-400,400),title=cr.filename,clf=False)
plt.savefig("cr_pulse.pdf")

#Voila ...

