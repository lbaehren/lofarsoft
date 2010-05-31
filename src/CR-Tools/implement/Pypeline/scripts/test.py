
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
