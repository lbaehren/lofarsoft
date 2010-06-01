#! /usr/bin/env python

from pycrtools import *
#------------------------------------------------------------------------
#Input section
#------------------------------------------------------------------------
filename=LOFARSOFT+"/data/lofar/rw_20080701_162002_0109.h5"
filename=filename_lopes_test
filename=filename_lofar_onesecond
#------------------------------------------------------------------------
#Creating Workspaces
#------------------------------------------------------------------------

ws=CRMainWorkSpace(filename=filename,doplot=True,verbose=True,modulename="ws")  
ws.makeFitBaseline(ws,logfit=True,fittype="BSPLINE",nbins=256) #fittype="POLY" or "BSPLINE"
ws.makeAverageSpectrum(ws)
#------------------------------------------------------------------------
#Setting some additional parameters
#------------------------------------------------------------------------
ws["blocksize"]=2**16
ws["max_nblocks"]=3
ws["ncoeffs"]=45
if ws["datafile"]["Observatory"]=='LOFAR':
    ws["numin"]=12 #MHz
    ws["numax"]=82 #MHz
elif ws["datafile"]["Observatory"]=='LOPES':
    ws["numin"]=43 #MHz
    ws["numax"]=73 #MHz

#------------------------------------------------------------------------
#Begin Calculations
#------------------------------------------------------------------------

"""
Calculate a spectrum averaged over all blocks
"""
ws["spectrum"].craveragespectrum(ws["datafile"],ws)

"""
Fit a polynomial to baseline of the (binned) average spectrum and return the coefficients.
"""
ws["meanrms"]=ws["coeffs"].crfitbaseline(ws["frequency"],ws["spectrum"],ws)

"""
Calculcate the baseline from the coefficients on the actual frequency grid.
"""
ws["baseline"].crcalcbaseline(ws["frequency"],ws["numin_i"],ws["numax_i"],ws["coeffs"],ws)

if ws["verbose"]: print time.clock()-ws["t0"],"s: Applying gain calibration - flattening spectrum."

"""
Apply the baseline and do a gain calibration on the spectrum
"""
ws["cleanspec"].copy(ws.spectrum)
ws.cleanspec /= ws.baseline
if ws["doplot"]: ws.cleanspec[0].plot()

"""
Identify spiky channels and replace them by the mean value (which is unity here).
"""
ws["meanspec"]=ws["cleanspec"][...,ws["numin_i"]:ws["numax_i"]].meaninverse()
ws["rfithreshold"] = (ws["meanrms"] * ws["rfi_nsigma"]) + ws["meanspec"]
ws["nbad_channels"]=ws["bad_channels"][...].findgreaterthan(ws["cleanspec"][...],ws["rfithreshold"])

ws["cleanspec"][...].set(ws["bad_channels"][...,[0]:ws["nbad_channels"]],ws["meanspec"])

if ws["verbose"]: print time.clock()-ws["t0"],"s: Done calculating clean spectrum."
if ws["doplot"]:
    ws.cleanspec[0].plot(clf=False)
    plt.savefig("testrfi2-spectrum.pdf",format="pdf")
    raw_input("Plotted clean spectrum - press Enter to continue...")

#--------------------------------------------------------------------------------
# The following is an example how one can use the above for cleaning of a single times series block
#--------------------------------------------------------------------------------

print "First let's see where there is a block sith spiky data and use the quality checking for that...."
ws["qualitycriteria"]={"mean":(-15,15),"rms":(5,15),"spikyness":(-7,7)}
ws["flaglist"]=CRQualityCheck(ws["qualitycriteria"],datafile=ws["datafile"],blocklist=[577,578,579],dataarray=None,blocksize=ws["blocksize"],nsigma=5,verbose=True)


print "Now doing RFI suppression and back transformation for a single block"
ws["baseline"].crcalcbaseline(ws["frequency"],ws["numin_i"],ws["numax_i"],ws["coeffs"],ws,doplot=False)
ws["datafile"]["block"]=578

if ws["verbose"]: print time.clock()-ws["t0"],"s: Reading in block #578 and invfft dirty spectrum"
rfitime=ws["datafile"]["Time"]
rfitime.setUnit("m","")
ws["fx"].read(ws["datafile"],"Fx")
ws["fft"][...].fftw(ws["fx"][...])
ws["fft"] /= ws["baseline"]

#Calculating renormalization factors for the invfft
factor=1.0/pi/ws["datafile"]["blocksize"]
scrtfft=hArray(copy=ws["fft"]) # Note that invfftw destroy the input vector!! Hence need copy here.

dirty=ws["datafile"]["emptyFx"]
dirty[...].invfftw(scrtfft[...])
dirty[...] *= factor

if ws["verbose"]: print time.clock()-ws["t0"],"s: Cleaning spectrum and invfft clean spectrum"
cleanfft=ws["datafile"]["emptyFFT"]
cleanfft.copy(ws["fft"])
cleanfft[...].set(ws["bad_channels"][...,[0]:ws["nbad_channels"]],1.0j)

clean=ws["datafile"]["emptyFx"]
scrtfft.copy(cleanfft)
clean[...].invfftw(scrtfft[...])
clean[...] *= factor

phaseonly=ws["datafile"]["emptyFx"]
scrtfft.copy(ws["fft"])
scrtfft.setamplitude(1)
scrtfft[...,0:3500].fill(0j)
scrtfft[...,25000:].fill(0j)
phaseonly[...].invfftw(scrtfft[...])
phaseonly *= (clean[0,2000:5000].stddev()/phaseonly[0,2000:5000].stddev()).val()

#phaseonly /= ws["datafile"]["blocksize"]

if ws["doplot"]: 
    rfitime-=rfitime[0].val()
    dirty[10].plot(xvalues=rfitime)
    clean[10].plot(xvalues=rfitime,clf=False)
    phaseonly[10].plot(clf=False,xvalues=rfitime)
    plt.savefig("testrfi2-timeseries.pdf",format="pdf")


#--------------------------------------------------------------------------------
#End of Calculations
#--------------------------------------------------------------------------------

