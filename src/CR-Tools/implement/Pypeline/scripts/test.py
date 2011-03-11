#! /usr/bin/env python
dr = crfile(LOFARSOFT+'/data/lofar/trigger-2010-04-15/triggered-pulse-2010-04-15-RS205.h5')
allfx = dr["Fx"]
hFileSetParameter(dr,'SelectedAntennas', [0,1,10,54,60] )
selfx = dr["Fx"]
selVol = dr["Voltage"]

from pycrtools import *

ws=CRGlobalParameters(modulename="ws")
ws.createFitbaseline(nbins=2**8)
ws.initParameters()


rfi=CRParametersFitBaseline(nbins=2**8)
rfi_meanrms=hCRFitBaseline(rfi_coeffs,frequency,spectrum,verbose=True,doplot=True)


t0=time.clock()
for i in range(100000):
    y="c" in s

print time.clock()-t0

t0=time.clock()
for i in range(100000):
    y="c" in x


print time.clock()-t0
