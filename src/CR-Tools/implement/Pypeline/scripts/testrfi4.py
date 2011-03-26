"""
execfile("/Users/falcke/LOFAR/usg/src/CR-Tools/implement/Pypeline/scripts/testrfi4.py")
"""

#Version with pardict
pardict=dict(doplot=False,plotlen=2**15,
             AverageSpectrum=dict(delta_nu=2000,nantennas_start=0,nantennas_stride=2,filefilter='~/data/Pulses/*-0.h5',maxnantennas=99,maxnchunks=99,doplot=False),
             FitBaseline=dict(ncoeffs=20,numax=85,numin=11,fittype="BSPLINE",normalize=False,splineorder=3),
             ApplyBaseline=dict(doplot=True)
             )

plt.hanging=True
#avspec=tasks.averagespectrum.AverageSpectrum(pardict=pardict); ws=avspec(); sp=ws.power

sp=hArrayRead('/Users/falcke/data/Pulses/oneshot_level4_CS017_19okt_no-0.h5.spec.pcr')
fitb=tasks.fitbaseline.FitBaseline(pardict=pardict); fitb(sp);
calcb=tasks.fitbaseline.CalcBaseline(); calcb(sp,pardict=pardict);
apply_baseline=tasks.fitbaseline.ApplyBaseline(pardict=pardict); apply_baseline(sp,rmsfactor=3.5);

#Example on how to apply this to individual - first set up a test data set
datafile=crfile("/Users/falcke/data/Pulses/oneshot_level4_CS017_19okt_no-0.h5")
blocksize=2**17
fftlength=blocksize/2+1
datafile["blocksize"]=blocksize

#Set up work array
fx=hArray(float,[blocksize])
frequency=hArray(float,[fftlength])
fftc=hArray(complex,[fftlength])
power=hArray(float,[fftlength],xvalues=frequency,par=dict(logplot="y"))
baseline=hArray(properties=power,fill=apply_baseline.baseline)

#Fill array with data and do FFT
frequency.read(datafile,"Frequency")
frequency.setUnit("M","")
fx.read(datafile,"Fx")
fftc.fftw(fx)
power.spectralpower(fftc)


#Here the actual RFI excision starts for a power spectrum 
# First apply the amplitude gain curve
power *= baseline
# Calculate the average value of the spectrum
mean=power[fitb.numin_i:fitb.numax_i].mean()
# and zap all frequencies, i.e. set to mean
power.set(apply_baseline.dirty_channels[0:apply_baseline.ndirty_channels[0]],mean.val())
power.plot(); plt.show()

#And now we do the same for the FFT
fftc *= baseline
mean=fftc[fitb.numin_i:fitb.numax_i].meanabs()
fftc.randomizephase(apply_baseline.dirty_channels[0:apply_baseline.ndirty_channels[0]],mean.val())

#To check: recalculate power spectrum
power.spectralpower(fftc)
power.plot(); plt.show()
