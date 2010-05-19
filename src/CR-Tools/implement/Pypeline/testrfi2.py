filename_lofar_onesecond=LOFARSOFT+"/data/lofar/RS307C-readfullsecond.h5"
filename_lofar_onesecond=LOFARSOFT+"/data/lofar/RS307C-readfullsecondtbb1.h5"
filename=LOFARSOFT+"/data/lofar/rw_20080701_162002_0109.h5"
filename_lopes=LOFARSOFT+"/data/lopes/example.event"
filename_lopes_test=LOFARSOFT+"/data/lopes/test.event"
#------------------------------------------------------------------------
#Input section
#------------------------------------------------------------------------
filename=LOFARSOFT+"/data/lofar/rw_20080701_162002_0109.h5"
filename=filename_lopes_test
filename=filename_lofar_onesecond
blocksize=2**16
rfi_nbins=2**8
#------------------------------------------------------------------------
#Setting Workspace
#------------------------------------------------------------------------

ws=CRMainWorkSpace(filename=filename,doplot=True,verbose=True,modulename="ws")  

ws["datafile"]=crfile(ws["filename"])
ws["blocksize"]=2**16
ws["datafile"]["blocksize"]=min(ws["blocksize"],ws["datafile"].filesize)
ws["max_nblocks"]=3
ws["maxorder"]=17
nsigma=7

ws.makeFitBaseline(ws,nbins=2**8)
ws.makeAverageSpectrum(ws)
ws.initParameters()

datafile=ws["datafile"]
#ncoeffs=maxorder+1
t0=time.clock()
#------------------------------------------------------------------------
#Array definitions
#------------------------------------------------------------------------
if ws["verbose"]: print time.clock()-t0,"s: Setting up work arrays."
"""
Define the basic arrays to work with
"""
ws["frequency"].setUnit("M","")
#ws["spectrum"]=hArray(float,[ws["nofAntennas"],ws["datafile"].fftLength],fill=0,name="Spectrum",units="a.u.",xvalues=frequency,par=[("logplot","y")])

"""
Define binned spectrum arrays
"""
rfi_cleanspec=hArray(properties=ws["spectrum"])
rfi_cleanspec2=hArray(properties=ws["spectrum"])
"""
Define arrays to calculate the polynomial fit
"""
rfi_coeffs=hArray(float,[ws["nofAntennas"],ws.FitBaseline["ncoeffs"]])
rfi_baseline=hArray(properties=ws["spectrum"],xvalues=ws["frequency"])

"""
Define arrays used to select and hold the clean bins
"""
rfi_bad_channels=hArray(int,ws["spectrum"],name="Bad Channels")


#------------------------------------------------------------------------
#Begin Calculations
#------------------------------------------------------------------------
"""
Calculate a spectrum averaged over all blocks
"""
ws["spectrum"].craveragespectrum(datafile,ws)

"""
Fit a polynomial to baseline of the average spectrum and return the coefficients.
"""
rfi_meanrms=rfi_coeffs.crfitbaseline(ws["frequency"],ws["spectrum"],ws)

#Calculate a smooth baseline for the full (large) spectrum
rfi_baseline.fill(0.0)
rfi_baseline[...].polynomial(ws["frequency"],rfi_coeffs[...],ws.FitBaseline["powers"][...])

#if doplot:
#    rfi_clean_bins_y[0,0:nselected_bins[0]].plot(xvalues=rfi_clean_bins_x[0,0:nselected_bins[0]],logplot=False)
#    rfi_baseline[0].plot(clf=False)
#    raw_input("Plotted logarithmic spectrum and baseline - Press Enter to continue...")

if ws["verbose"]: print time.clock()-t0,"s: Straightening out spectrum."

rfi_baseline.exp()
rfi_cleanspec.copy(ws["spectrum"])
rfi_cleanspec /= rfi_baseline

rfi_meanrms *= nsigma

nbad_channels=rfi_bad_channels[...].findgreaterthan(rfi_cleanspec[...],rfi_meanrms)
rfi_cleanspec2.copy(rfi_cleanspec)
rfi_cleanspec2[...].set(rfi_bad_channels[...,[0]:nbad_channels],1.0)
if ws["doplot"]: 
    rfi_cleanspec[0].plot()
    rfi_cleanspec2[0].plot(clf=False)
if ws["verbose"]: print time.clock()-t0,"s: Done."
