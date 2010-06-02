from pycrtools import *
import numpy as np
#import scipy as sp
import matplotlib.pyplot as plt

def p_(var):
    if (type(var)==list): map(lambda x:p_(x),var)
    else: print " ",var,"=>",eval(var)

#------------------------------------------------------------------------
"""
(++) Parameters
---------------
Set some parameters "by hand", such as:
 - filename
 - station name
 - phase_center_in
 - def_pointing
 - FarField

"""
filename=LOFARSOFT+"/data/lofar/trigger-2010-04-15/triggered-pulse-2010-04-15-RS205.h5"
stationname="RS205"
phase_center_in=[0., 0., 0.]
def_pointing=[245., 12.5, 200.]
FarField=False

#------------------------------------------------------------------------


ws=CRMainWorkSpace(filename=filename,fittype="POLY",ncoeffs=8,nbins=1024,doplot=False,verbose=False,modulename="ws")  
ws.makeFitBaseline(ws,logfit=True,fittype="BSPLINE",nbins=256) #fittype="POLY" or "BSPLINE"

ws["numin"]=12 #MHz
ws["numax"]=82 #MHz

cr=ws["datafile"]

cr["blocksize"] = cr["filesize"]

"""

Now we read in the raw time series data as well as the Frequencies and
Time value arrays.

"""
cr_time=cr["Time"].setUnit("\\mu","s")

#ws["frequency"]=cr["Frequency"] 
# Attention don't do this here when later using it in caluclating
#phases that require seconds and Hz as units

cr_efield=cr["Fx"].setPar("xvalues",cr_time)
"""

As a next step we create an empty vector to hold the Fourierspectrum
of the data

"""
ws["fft"]=cr["emptyFFT"].setPar("xvalues",ws["frequency"])
"""

and then make the Fourier transform followed by a reordering of the
output, noting that the data was taken in the second Nyquist domain

"""
ws["fft"][...].fftw(cr_efield[...])
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
#ws["fft"] /= ws["baseline"]
#ws["spectrum"][0].plot()

ws["meanspec"]=ws["spectrum"][...,ws["numin_i"]:ws["numax_i"]].meaninverse()
ws["rfithreshold"] = (ws["meanrms"] * ws["rfi_nsigma"]) + ws["meanspec"]
ws["nbad_channels"]=ws["bad_channels"][...].findgreaterthan(ws["spectrum"][...],ws["rfithreshold"])
#ws["fft"][...].set(ws["bad_channels"][...,[0]:ws["nbad_channels"]],ws["meanspec"])
ws["spectrum"][...].set(ws["bad_channels"][...,[0]:ws["nbad_channels"]],ws["meanspec"])

#ws["spectrum"][0].plot(clf=False)
#raw_input("Plotted spectrum - press Enter to continue...")
"""



(++) Coordinates
---------------

For doing actual beamforming we need to know the antenna
positions. For this we will use the method getCalData:

"""
antennaset="LBA_OUTER"
antennafilename=LOFARSOFT+"/data/calibration/AntennaArrays/"+stationname+"-AntennaArrays.conf"
antfile = open(antennafilename,'r') 
antfile.seek(0)
str = ''
while antennaset not in str:
    str = antfile.readline()

str = antfile.readline()
str = antfile.readline()
nrantennas = str.split()[0]
nrdir = str.split()[4]
ant = []
for i in range(0,int(nrantennas)):
    str=antfile.readline()
    ant.extend([float(str.split()[0]),float(str.split()[1]),float(str.split()[2])])  #X polarization
    ant.extend([float(str.split()[3]),float(str.split()[4]),float(str.split()[5])])  #Y polarization


antenna_positions=hArray(ant,[int(nrantennas)*2,int(nrdir)])
"""

As a next step we need to put the antenna coordinates on a reference
frame that is relative to the phase center. Here we will choose the
location of the first antenna as our phase center (that makes life a
little easier for checking) and we simply subtract the reference
position from the antenna locations so that our phase center lies at
0,0,0.

"""
phase_center=hArray(phase_center_in)
antenna_positions -= phase_center
"""



Now let's look at the beam forming:

We first turn the cooridnates into a std vector and create a vector that is
supposed to hold the Cartesian coordinates. Note that the AzEL vector
is actually AzElRadius.

"""
azel=hArray(def_pointing,dimensions=[1,3])
cartesian=azel.new()
"""

We then do the conversion, using

"""
hCoordinateConvert(azel[...],CoordinateTypes.AzElRadius,cartesian[...],CoordinateTypes.Cartesian,True)
"""

Then calculate geometric delays and add the instrumental delays.

"""
delays=hArray(float,dimensions=[ws["nofAntennas"]])
hGeometricDelays(delays,antenna_positions,cartesian,FarField)
p_("delays")
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

cr_efield_shifted = cr["emptyFx"].setPar("xvalues",cr_time)

"""
However, after fftw'ing the input array is destroyed!
#### Keep that in mind! ####

"""
cr_efield_shifted[...].invfftw(cr_calfft_shifted[...]) # is being destroyed here ....
cr_efield_shifted /= cr["blocksize"]
"""
To make plotting quicker, select the time window you want to
plot. Unfortunately Mathplot plots everything first and then simply
does not display the things that are outside the plotting window -
this is very slow with 8x65000 points.

"""
t1=cr_time.findlowerbound(328.0)
t2=cr_time.findlowerbound(329.0)

t1w=cr_time.findlowerbound(328.0-5.)
t2w=cr_time.findlowerbound(329.0+5.)

cr_efield[1,t1:t2].plot(xvalues=cr_time[t1:t2])
for ant in range(3,96,2):
  cr_efield[ant,t1:t2].plot(xvalues=cr_time[t1:t2],clf=False)
raw_input("Plotted unshifted timeseries - press Enter to continue...")

cr_efield_shifted[1,t1:t2].plot(xvalues=cr_time[t1:t2])
for ant in range(3,96,2):
  cr_efield_shifted[ant,t1:t2].plot(xvalues=cr_time[t1:t2],clf=False)
raw_input("Plotted shifted timeseries - press Enter to continue...")

"""
The beamforming we here do by simply adding the data in the time domain 
"""
antennalist = range(1,96,2)
cr_efield_shifted_added=hArray(float,dimensions=cr_time,name="beamformed E-field",xvalues=cr_time)
cr_efield_shifted[antennalist,...].addto(cr_efield_shifted_added)
cr_efield_shifted_added /= len(antennalist)

"""
To test, let's also calculate the incoherent beam:
"""
cr_efield_abs_shifted_added=hArray(float,dimensions=cr_time,name="incoherent beam",xvalues=cr_time)
cr_efield_shifted_abs=hArray(copy=cr_efield_shifted)
cr_efield_shifted_abs.abs()
cr_efield_shifted_abs[antennalist,...].addto(cr_efield_abs_shifted_added)
cr_efield_abs_shifted_added /= len(antennalist)
"""
And the incoherent beam of the unshifted data:
"""
cr_efield_abs_added=hArray(float,dimensions=cr_time,name="incoherent beam",xvalues=cr_time)
cr_efield_abs=hArray(copy=cr_efield)
cr_efield_abs.abs()
cr_efield_abs[antennalist,...].addto(cr_efield_abs_added)
cr_efield_abs_added /= len(antennalist)

cr_efield_shifted_added_abs=hArray(copy=cr_efield_shifted_added,xvalues=cr_time)
cr_efield_shifted_added_abs.abs()

cr_efield_abs_added[t1w:t2w].plot(xvalues=cr_time[t1w:t2w])
cr_efield_abs_shifted_added[t1w:t2w].plot(xvalues=cr_time[t1w:t2w],clf=False)
cr_efield_shifted_added_abs[t1w:t2w].plot(xvalues=cr_time[t1w:t2w],clf=False)
