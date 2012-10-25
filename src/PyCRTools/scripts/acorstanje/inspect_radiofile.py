# Test script to quickly inspect a radio-triggered file (i.e. assumed 131072 samples long, pulse in the middle)

from pycrtools import *
import numpy as np
import matplotlib.pyplot as plt
import sys

if len(sys.argv) > 1:
    fname = sys.argv[1]
else:
    print 'Supply filename on command line!'
    exit()

deg2rad = 2*np.pi / 360.0
plotlen = 1024
    
f = open(fname)
antenna_positions = f["ANTENNA_POSITIONS"]
sample_interval = 5.0e-9
f["BLOCKSIZE"] = 131072
y = f["TIMESERIES_DATA"]
y2 = y.toNumpy()

y3 = y2[:, (65536 - plotlen/2):(65536 + plotlen/2)]

data = hArray(float, dimensions=[96, plotlen])
data[...].copy(y[..., (66048 - plotlen/2):(66048 + plotlen/2)])
# zero padding... data[..., plotlen/2:plotlen] = 0.0
for i in range(2):
    plt.plot(y3[10+2*i])

# try crosscorrelation and see how that looks 
#data[..., 0:420] = 0.0
#data[..., 500:-1] = 0.0
crosscorr=trerun('CrossCorrelateAntennas',"crosscorr",data,oversamplefactor=2)
plt.figure()

cc = crosscorr.crosscorr_data.toNumpy() / 20480
for i in range(2):
    plt.plot(cc[10+2*i])


# try beamforming with optimization
"""maxpower = 0.0
for i in range(int(360.0 / 24)):
    azstart = i * 360.0 / 24

    bf=trerun("BeamFormer2","bf",data=data,maxnantennas=96,antpos=antenna_positions,FarField=True,sample_interval=sample_interval,pointings=rf.makeAZELRDictGrid(*(azstart*deg2rad, 0*deg2rad, 10000),nx=1,ny=1), calc_timeseries=True, doplot=False, doabs=False, dosquare=True, smooth_width=7, plotspec=False,verbose=False, store_spectrum=False)

    direction = bf.optimizeDirection(verbose=True)
    power = - bf.smoothedPulsePowerMaximizer(direction, verbose=True)
    
    if power > maxpower:
        print 'updating maxpower'
        print '****'
        maxpower = power
        bestDirection = direction

print bestDirection / deg2rad
print maxpower
    
"""
# make a crude image using beamformer
azsteps = 360.0 / 10
elsteps = 90.0 / 10

elevations = np.arange(elsteps) * (90.0 / elsteps)
azimuths = np.arange(azsteps) * (360.0 / azsteps)

imarray = np.zeros((elsteps, azsteps))
# get beamformer started
bf=trerun("BeamFormer2","bf",data=data,maxnantennas=96,antpos=antenna_positions,FarField=True,sample_interval=sample_interval,pointings=rf.makeAZELRDictGrid(*(0*deg2rad, 0*deg2rad, 10000),nx=1,ny=1), calc_timeseries=True, doplot=False, doabs=False, dosquare=True, smooth_width=7, plotspec=False,verbose=False, store_spectrum=False)

elstep = 0
for el in elevations:
    azstep = 0
    for az in azimuths:
        power = - bf.smoothedPulsePowerMaximizer((az*deg2rad, el*deg2rad), verbose=True)
        imarray[elstep, azstep] = power
        azstep += 1
    elstep += 1

plt.imshow(imarray, cmap=plt.cm.hot,extent=(0, 360.0, 0, 90.0))
plt.xlabel("Azimuth [deg]")
plt.ylabel("Elevation [deg]")

