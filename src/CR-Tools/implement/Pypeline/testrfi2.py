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
max_nblocks=3
maxorder=17
nsigma=7
doplot=True
verbose=True
#------------------------------------------------------------------------
#Setting Workspace
#------------------------------------------------------------------------
ws=CRGlobalParameters(filename=filename,modulename="ws")  
ws.createFitbaseline(nbins=2**8)
ws.initParameters()

datafile=ws.datafile
datafile["blocksize"]=min(ws.blocksize,ws.datafile.filesize)
nblocks=min(ws.datafile.filesize/ws.datafile.blocksize,max_nblocks)
ncoeffs=maxorder+1
t0=time.clock()
#------------------------------------------------------------------------
#Array definitions
#------------------------------------------------------------------------
if verbose: print time.clock()-t0,"s: Setting up work arrays."
"""
Define the basic arrays to work with
"""
frequency=ws.datafile["Frequency"]
frequency.setUnit("M","")
spectrum=hArray(float,[ws.nofAntennas,ws.datafile.fftLength],fill=0,name="Spectrum",units="a.u.",xvalues=frequency,par=[("logplot","y")])

"""
Define binned spectrum arrays
"""
rfi_cleanspec=hArray(properties=spectrum)
rfi_cleanspec2=hArray(properties=spectrum)
"""
Define arrays to calculate the polynomial fit
"""
rfi_coeffs=hArray(float,[ws.nofAntennas,ws.fitbaseline.ncoeffs])
rfi_baseline=hArray(properties=spectrum,xvalues=frequency)

"""
Define arrays used to select and hold the clean bins
"""
rfi_bad_channels=hArray(int,spectrum,name="Bad Channels")


#------------------------------------------------------------------------
#Begin Calculations
#------------------------------------------------------------------------
"""
Calculate a spectrum averaged over all blocks
"""
spectrum.craveragespectrum(datafile,blocks=range(nblocks),verbose=verbose)

"""
Fit a polynomial to baseline of the average spectrum and return the coefficients.
"""
rfi_meanrms=rfi_coeffs.crfitbaseline(frequency,spectrum,verbose=verbose,doplot=doplot)

#Calculate a smooth baseline for the full (large) spectrum
rfi_baseline.fill(0.0)
rfi_baseline[...].polynomial(frequency,rfi_coeffs[...],ws.fitbaseline.powers[...])

#if doplot:
#    rfi_clean_bins_y[0,0:nselected_bins[0]].plot(xvalues=rfi_clean_bins_x[0,0:nselected_bins[0]],logplot=False)
#    rfi_baseline[0].plot(clf=False)
#    raw_input("Plotted logarithmic spectrum and baseline - Press Enter to continue...")

if verbose: print time.clock()-t0,"s: Straightening out spectrum."

rfi_baseline.exp()
rfi_cleanspec.copy(spectrum)
rfi_cleanspec /= rfi_baseline

rfi_meanrms *= nsigma

nbad_channels=rfi_bad_channels[...].findgreaterthan(rfi_cleanspec[...],rfi_meanrms)
rfi_cleanspec2.copy(rfi_cleanspec)
rfi_cleanspec2[...].set(rfi_bad_channels[...,[0]:nbad_channels],1.0)
if doplot: 
    rfi_cleanspec[0].plot()
    rfi_cleanspec2[0].plot(clf=False)
if verbose: print time.clock()-t0,"s: Done."
