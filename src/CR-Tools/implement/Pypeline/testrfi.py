filename_lofar_onesecond=LOFARSOFT+"/data/lofar/RS307C-readfullsecond.h5"
filename_lofar_onesecond=LOFARSOFT+"/data/lofar/RS307C-readfullsecondtbb1.h5"
filename=LOFARSOFT+"/data/lofar/rw_20080701_162002_0109.h5"
#------------------------------------------------------------------------
#Input section
#------------------------------------------------------------------------
filename=filename_lofar_onesecond
blocksize=2**16
nbins_small=2**10
max_nblocks=3
maxorder=17
nsigma=7
doplot=True
verbose=True
#------------------------------------------------------------------------


datafile=crfile(filename)
datafile["blocksize"]=min(blocksize,datafile.filesize)
nblocks=min(datafile.filesize/datafile.blocksize,max_nblocks)
ncoeffs=maxorder+1
t0=time.clock()
#------------------------------------------------------------------------
#Array definitions
#------------------------------------------------------------------------
if verbose: print time.clock()-t0,"s: Setting up work arrays."
"""
Define the basic arrays to work with
"""
frequency=datafile["Frequency"]
frequency.setUnit("M","")
spectrum=hArray(float,[datafile.nofAntennas,datafile.fftLength],fill=0,name="Spectrum",units="a.u.",xvalues=frequency,par=[("logplot","y")])

"""
Define binned spectrum arrays
"""
rfi_freqs=hArray(float,dimensions=[1,nbins_small],name="Frequency",units=("M","Hz"))
rfi_spectrum=hArray(float,[datafile.nofAntennas,nbins_small],name="Binned Spectrum",units="a.u.",xvalues=rfi_freqs,par=("logplot","y"))
rfi_rms=hArray(properties=rfi_spectrum, name="RMS of Spectrum")
rfi_rms2=hArray(properties=rfi_rms)

"""
Define arrays to calculate the polynomial fit
"""
powers=hArray(int,[datafile.nofAntennas,ncoeffs],range(ncoeffs))
covariance=hArray(float,[datafile.nofAntennas,ncoeffs,ncoeffs])
coeffs=hArray(float,[datafile.nofAntennas,ncoeffs])
ratio=hArray(properties=rfi_spectrum,name="RMS/Ampltude",par=("logplot",False))
xpowers=hArray(float,[datafile.nofAntennas,nbins_small,ncoeffs],name="Powers of Frequency")
baseline=hArray(properties=spectrum,xvalues=frequency)
baseline_binned=hArray(properties=rfi_spectrum,xvalues=rfi_freqs)

"""
Define arrays used to select and hold the clean bins
"""
bad_channels=hArray(int,spectrum,name="Bad Channels")
selected_bins=hArray(int,rfi_spectrum,name="Selected bins")
clean_bins_x=hArray(properties=rfi_freqs,name="Clean Frequencies")
clean_bins_y=hArray(properties=rfi_spectrum,xvalues=clean_bins_x)

#------------------------------------------------------------------------
#Begin Calculations
#------------------------------------------------------------------------
"""
Calculate a spectrum averaged over all blocks
"""
spectrum.craveragespectrum(datafile,blocks=range(nblocks),verbose=verbose)

"""
Downsample spectrum and frequencies to lower resolution, obtain rms and select good bins.
"""
if verbose:
    print "Downsampling spectrum to ",nbins_small,"bins."
    rfi_freqs.downsample(frequency[1:])
    rfi_spectrum[...].downsamplespikydata(rfi_rms[...],spectrum[...,1:],-0.01)

if doplot:
    spectrum[0].plot(title="RFI Downsampling")
    rfi_spectrum[0].plot(clf=False)
    raw_input("Plotted downsampled spectrum - press Enter to continue...")

ratio.div(rfi_spectrum,rfi_rms)
mratio=hArray(ratio[...].mean())
ratio[...] /= mratio[...]
ratio.square()

#Now select bins where the ratio between RMS and amplitude is within a factor 2 of the mean value
nselected_bins=selected_bins[...].findbetween(ratio[...],0.5,2)

#Now copy only those bins with average RMS, i.e. likely with little RFI and take the log
clean_bins_x[...].copy(rfi_freqs,selected_bins[...],nselected_bins)
clean_bins_y[...].copy(rfi_spectrum,selected_bins[...],nselected_bins)
clean_bins_y[...,[0]:nselected_bins].log()

if verbose: print time.clock()-t0,"s: Fitting baseline."

#Create the nth powers of the x value, i.e. the frequency, for the fitting
xpowers[...,[0]:nselected_bins].linearfitpolynomialx(clean_bins_x[...,[0]:nselected_bins],powers[...])

#Fit an nth order polynomial to the log data
coeffs[...].linearfit(covariance[...],xpowers[...],clean_bins_y[...],nselected_bins)

#Caluclate a smooth baseline for the full (large) spectrum
baseline.fill(0.0)
baseline[...].polynomial(frequency,coeffs[...],powers[...])
baseline_binned[...].polynomial(rfi_freqs,coeffs[...],powers[...])

#clean_bins_y[0,0:nselected_bins[0]].plot(xvalues=rfi_freqs[0,0:nselected_bins[0]])
if doplot:
    clean_bins_y[0,0:nselected_bins[0]].plot(xvalues=clean_bins_x[0,0:nselected_bins[0]],logplot=False)
    baseline[0].plot(clf=False)
    raw_input("Plotted logarithmic spectrum and baseline - Press Enter to continue...")

if verbose: print time.clock()-t0,"s: Straightening out spectrum."

baseline.exp()
baseline_binned.exp()
cleanspec=hArray(copy=spectrum)
cleanspec /= baseline

rfi_rms /= baseline_binned
rfi_rms2[...].copy(rfi_rms,selected_bins[...],nselected_bins)
rfi_meanrms=rfi_rms2[...,[0]:nselected_bins].mean()

rfi_meanrms *= nsigma

nbad_channels=bad_channels[...].findgreaterthan(cleanspec[...],rfi_meanrms)
cleanspec2=hArray(copy=cleanspec)
cleanspec2[...].set(bad_channels[...,[0]:nbad_channels],1.0)
if doplot: 
    cleanspec[0].plot()
    cleanspec2[0].plot(clf=False)
if verbose: print time.clock()-t0,"s: Done."
