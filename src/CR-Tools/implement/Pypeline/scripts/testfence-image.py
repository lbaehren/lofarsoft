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
azRange=[180., 360., 10.]
elRange=[0., 60., 5.]
distsRange=[10., 50., 100., 500., 1000.]
fixedDist=200.
FarField=False

#------------------------------------------------------------------------


ws=CRMainWorkSpace(filename=filename,fittype="POLY",ncoeffs=8,nbins=128,doplot=False,verbose=False,modulename="ws")  
ws.makeFitBaseline(ws,logfit=True,fittype="BSPLINE",nbins=128) #fittype="POLY" or "BSPLINE"

ws["numin"]=12 #MHz
ws["numax"]=82 #MHz

cr=ws["datafile"]

cr["blocksize"] = 512
cr["block"] = 128
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
#ws["meanrms"]=ws["coeffs"].crfitbaseline(ws["frequency"],ws["spectrum"],ws)
#ws["baseline"].crcalcbaseline(ws["frequency"],ws["numin_i"],ws["numax_i"],ws["coeffs"],ws)

#ws["spectrum"] /= ws["baseline"]
#ws["fft"] /= ws["baseline"]
#ws["spectrum"][0].plot()

#ws["meanspec"]=ws["spectrum"][...,ws["numin_i"]:ws["numax_i"]].meaninverse()
#ws["rfithreshold"] = (ws["meanrms"] * ws["rfi_nsigma"]) + ws["meanspec"]
#ws["nbad_channels"]=ws["bad_channels"][...].findgreaterthan(ws["spectrum"][...],ws["rfithreshold"])
#ws["fft"][...].set(ws["bad_channels"][...,[0]:ws["nbad_channels"]],ws["meanspec"])
#ws["spectrum"][...].set(ws["bad_channels"][...,[0]:ws["nbad_channels"]],ws["meanspec"])

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

(++) Imaging
---------------

Now let's start setting up the image:
"""
if FarField != True:
    print "Near-field imaging not supported yet! Switching to fixed distance."
    #FarField = True
else:
    fixedDist = 1.

n_az=int((azRange[1]-azRange[0])/azRange[2])
n_el=int((elRange[1]-elRange[0])/elRange[2])

n_pixels = n_az * n_el;
pixarray = np.zeros( (n_el,n_az) ) 

azel=hArray(float,dimensions=[3])
cartesian=azel.new()
delays=hArray(float,dimensions=[ws["nofAntennas"]])
weights=hArray(complex,dimensions=ws["fft"],name="Complex Weights")
phases=hArray(float,dimensions=ws["fft"],name="Phases",xvalues=ws["frequency"]) 
shifted_fft=hArray(complex,dimensions=ws["fft"])
beamformed_fft=hArray(complex,dimensions=ws["frequency"])
beamformed_efield=hArray(float,dimensions=cr_time)
ws["frequency"].setUnit("","")

el_values = np.arange(elRange[1], elRange[0], -elRange[2])
az_values = np.arange(azRange[0], azRange[1], azRange[2])

ant_indices =  range(1,96,2)

for el_ind in range(n_el):
    for az_ind in range(n_az):
        azel[0]=az_values[az_ind]
        azel[1]=el_values[el_ind]
        azel[2]= fixedDist
        
        
        hCoordinateConvert(azel,CoordinateTypes.AzElRadius,cartesian,CoordinateTypes.Cartesian,True)
        hGeometricDelays(delays,antenna_positions,cartesian,FarField)   
        #p_("delays")
        hDelayToPhase(phases,ws["frequency"],delays)
        hPhaseToComplex(weights,phases)
        shifted_fft.copy(ws["fft"])
        shifted_fft *= weights
        beamformed_fft = shifted_fft[ant_indices[0]]
        for n in ant_indices[1:]:
            shifted_fft[n].addto(beamformed_fft)
        hInvFFTw(beamformed_efield,beamformed_fft)
        #beamformed_efield.invfftw(beamformed_fft)
        beamformed_efield.abs()
        pixarray[el_ind,az_ind] = beamformed_efield.max()[0]/cr["blocksize"]

#pixarray[0,0]=np.max(pixarray)
plt.imshow(pixarray,cmap=plt.cm.hot,extent=((azRange[0]-azRange[2]/2),(azRange[1]-azRange[2]/2),(elRange[0]-elRange[2]/2),(elRange[1]-elRange[2]/2)))
plt.xlabel("Azimuth [deg]")
plt.ylabel("Elevation [deg]")
