
ws=CRGlobalParameters(modulename="ws")  
ws.createFitbaseline(nbins=2**8)
ws.initParameters()


rfi=CRParametersFitBaseline(nbins=2**8)
rfi_meanrms=hCRFitBaseline(rfi_coeffs,frequency,spectrum,verbose=True,doplot=True)
