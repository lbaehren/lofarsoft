filename_lofar_onesecond=LOFARSOFT+"/data/lofar/RS307C-readfullsecond.h5"
filename_lofar_onesecond=LOFARSOFT+"/data/lofar/RS307C-readfullsecondtbb1.h5"
filename=LOFARSOFT+"/data/lofar/rw_20080701_162002_0109.h5"
datafile=crfile(filename)
datafile["blocksize"]=2**16
fx=datafile["Fx"]
fft=datafile["emptyFFT"]
fft[...].fftw(fx[...])
frequency=datafile["Frequency"]
frequency.setUnit("M","")
spectrum=hArray(float,[1,datafile.fftLength],fill=0,name="Spectrum",units="a.u.",xvalues=frequency,par=[("logplot","y")])
lgspectrum=hArray(float,[1,datafile.fftLength],fill=0,name="lg Spectrum",units="a.u.",xvalues=frequency)
spectrum.spectralpower(fft[...])
lgspectrum.log(spectrum)


RFIVectorLength=128
rfi_freqs=hArray(float,dimensions=[1,RFIVectorLength],name="Frequency",units="MHz")
rfi_freqs.downsample(frequency[1:])

rfi_amplitudes1=hArray(float,[datafile.nofAntennas,RFIVectorLength],xvalues=rfi_freqs,par=("logplot","y"))
rfi_amplitudes2=hArray(float,[datafile.nofAntennas,RFIVectorLength],xvalues=rfi_freqs,par=("logplot","y"))
rfi_amplitudes3=hArray(float,[datafile.nofAntennas,RFIVectorLength],xvalues=rfi_freqs)
rfi_rms1=hArray(properties=rfi_amplitudes1)
rfi_rms2=hArray(properties=rfi_amplitudes2)
rfi_rms3=hArray(properties=rfi_amplitudes3)

rfi_amplitudes1[...].downsample(rfi_rms1[...],spectrum[...,1:])
rfi_amplitudes2[...].downsamplespikydata(rfi_rms2[...],spectrum[...,1:],-0.1)
rfi_amplitudes3[...].downsamplespikydata(rfi_rms3[...],lgspectrum[...,1:],-0.1)

spectrum[0].plot(title="RFI Downsampling")
rfi_amplitudes1[0].plot(clf=False)
rfi_amplitudes2[0].plot(clf=False)
rfi_amplitudes3[0].plot(clf=False)
raw_input("Press Enter to continue...")

yvalues=rfi_amplitudes3
xvalues=lgspectrum
rms=rfi_rms3

maxorder=13
ncoeffs=maxorder+1
powers=hArray(int,[datafile.nofAntennas,ncoeffs],range(ncoeffs))
covariance=hArray(float,[datafile.nofAntennas,ncoeffs,ncoeffs])
coeffs=hArray(float,[datafile.nofAntennas,ncoeffs])
ratio=hArray(properties=yvalues)

#rfi_rms1[0,0]=rfi_rms[0,1].val()
#weights=hArray(properties=yvalues)
#weights.copy(rms)
#weights.errorstoweights(weights)
#weights[0,1]=0
#weights[0,-1,]=0
#weights.fill(1.0)
#ratio=weights.new()
ratio.div(rfi_amplitudes2,rfi_rms2)
mratio=hArray(ratio.mean())
ratio[...] /= mratio[...]
ratio.square()

selected_channels=hArray(int,spectrum)
clean_channels_x=hArray(properties=rfi_freqs,name="Clean Frequencies")
clean_channels_y=hArray(properties=yvalues,xvalues=clean_channels_x)
#nselected_channels=ratio[...].findbetween(0.3,selected_channels[...])
nselected_channels=selected_channels[...].findbetween(ratio[...],0.5,2.0)

#Now copy only the channels which have no strong RFI in the,
clean_channels_x[...].copy(rfi_freqs,selected_channels[...],nselected_channels)
clean_channels_y[...].copy(yvalues,selected_channels[...],nselected_channels)

xpowers=hArray(float,[datafile.nofAntennas,RFIVectorLength,ncoeffs])
xpowers[...].linearfitpolynomialx(clean_channels_x[...],powers[...])

#coeffs.linearfit(covariance,xpowers,yvalues,weights)
#coeffs.linearfit(covariance,xpowers,yvalues,ratio)
coeffs[...].linearfit(covariance[...],xpowers[...],clean_channels_y[...],nselected_channels)

baseline=hArray(properties=spectrum,xvalues=frequency)
baseline.fill(0.0)
baseline[...].polynomial(frequency,coeffs[...],powers[...])
clean_channels_y.plot()
baseline.plot(clf=False)
raw_input("Press Enter to continue...")
#baseline.exp()

xvalues.plot()
yvalues.plot(clf=False)
baseline.plot(clf=False)
raw_input("Press Enter to continue...")

baseline.exp()

cleanspec=hArray(copy=spectrum)

cleanspec /= baseline

cleanspec.plot()

