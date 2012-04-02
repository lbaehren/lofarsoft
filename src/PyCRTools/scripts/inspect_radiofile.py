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
plotlen = 2048
    
f = open(fname)
antenna_positions = f["ANTENNA_POSITIONS"]
sample_interval = 5.0e-9
f["BLOCKSIZE"] = 131072
y = f["TIMESERIES_DATA"]
y2 = y.toNumpy()

y3 = y2[:, (65536 - plotlen/2):(65536 + plotlen/2)]

data = hArray(float, dimensions=[96, plotlen])
data[...].copy(y[..., (66048 - plotlen/2):(66048 + plotlen/2)])

for i in range(8):
    plt.plot(y3[i])

# try crosscorrelation and see how that looks 
crosscorr=trerun('CrossCorrelateAntennas',"crosscorr",data,oversamplefactor=10)
plt.figure()

cc = crosscorr.crosscorr_data.toNumpy() / 20480
plt.plot(cc[0])
plt.plot(cc[1])
plt.plot(cc[2])


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

