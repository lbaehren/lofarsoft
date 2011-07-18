"""
execfile("/Users/falcke/LOFAR/usg/src/CR-Tools/implement/Pypeline/scripts/testrfi4.py")
"""
from pycrtools import *
import pycrtools as cr

#Version with pardict
pardict=dict(doplot=True,plotlen=2**15,
             AverageSpectrum=dict(delta_nu=2000,nantennas_start=0,nantennas_stride=2,filefilter='$LOFARSOFT/data/lofar/oneshot_level4_CS017_19okt_no-9.h5',maxnantennas=99,maxnchunks=99,doplot=False),
             FitBaseline=dict(ncoeffs=20,numax=85,numin=11,fittype="BSPLINE",normalize=False,splineorder=3),
             ApplyBaseline=dict(doplot=False)
             )

plt.hanging=False # Use true if execution hangs after plotting one window
plt.EDP64bug=True # use True if your system crashes for plotting semilog axes

avspec=tasks.averagespectrum.AverageSpectrum(pardict=pardict); ws=avspec(); sp=ws.power

sp=hArrayRead('oneshot_level4_CS017_19okt_no-9.h5.spec.pcr')
fitb=tasks.fitbaseline.FitBaseline(pardict=pardict); fitb(sp);
calcb=tasks.fitbaseline.CalcBaseline(); calcb(sp,pardict=pardict);
apply_baseline=tasks.fitbaseline.ApplyBaseline(pardict=pardict); apply_baseline(sp,rmsfactor=3.5);
print "Kill window to continue"; plt.show()

#Example on how to apply this to individual - first set up a test data set
datafile=cr.open("$LOFARSOFT/data/lofar/oneshot_level4_CS017_19okt_no-9.h5")
blocksize=2**17
fftlength=blocksize/2+1
datafile["BLOCKSIZE"]=blocksize
#datafile["SELECTED_DIPOLES"]=[datafile["DIPOLE_NAMES"][0]]

#Set up work array
fx=hArray(float,[96,blocksize])
frequency=hArray(float,[fftlength])
fftc=hArray(complex,[96, fftlength])
power=hArray(float,[96, fftlength],xvalues=frequency,par=dict(logplot="y"))
totalPower = hArray(float, [fftlength], xvalues=frequency, par=dict(logplot="y"))
baseline=hArray(properties=power,fill=apply_baseline.baseline)

#Fill array with data and do FFT
frequency.read(datafile,"FREQUENCY_DATA")
frequency.setUnit("M","")
fx.read(datafile,"TIMESERIES_DATA")
fftc[...].fftw(fx[...])
power[...].spectralpower(fftc[...])

#Here the actual RFI excision starts for a power spectrum 
# First apply the amplitude gain curve
power[...] *= baseline
# Calculate the average value of the spectrum
mean=power[..., fitb.numin_i:fitb.numax_i].mean()
mean = mean.mean()
# and zap all frequencies, i.e. set to mean
power[...].set(apply_baseline.dirty_channels[0:apply_baseline.ndirty_channels[0]],mean)
power[0].plot(); print "Kill window to continue"; plt.show()

#And now we do the same for the FFT
fig = plt.figure()
 
ax = fig.add_subplot(1,1,1)
ax.set_yscale('log')
power.fill(0.0)
power[...].spectralpower(fftc[...])
totalPower.add(power[...])
toplot = totalPower.toNumpy() / float(blocksize)

fftc[...] *= baseline
mean=fftc[..., fitb.numin_i:fitb.numax_i].meanabs()
mean = mean.mean()
#fftc[...].randomizephase(apply_baseline.dirty_channels[0:apply_baseline.ndirty_channels[0]],mean)
fftc[..., 0]=0.0 # subtract DC offset!
fftc[..., apply_baseline.dirty_channels[0:apply_baseline.ndirty_channels[0]]] = 0.0
fftc[...] /= baseline

#To check: recalculate power spectrum
power.fill(0.0)
totalPower.fill(0.0)
power[...].spectralpower(fftc[...])
totalPower.add(power[...])
totalPower.div(float(blocksize))
#toplot = toplot[apply_baseline.dirty_channels[0:apply_baseline.ndirty_channels[0]].toNumpy()]
ax.plot(frequency.toNumpy(), toplot, color='r')
ax.plot(frequency.toNumpy(), totalPower.toNumpy(), color='b'); 
ax.set_title('Power spectrum with RFI lines')
ax.set_xlabel('Frequency (MHz)')
ax.set_ylabel('Spectral power (relative)')
#ax.title= 'Power spectrum with narrowband RFI lines'
#ax.ylabel = 'Spectral power'
#ax.xlabel = 'Frequency [MHz]'
raw_input("Plotted power spectrum with / without RFI cleaning")
newFx = datafile["EMPTY_TIMESERIES_DATA"]
hInvFFTw(newFx[...], fftc[...])
newFx /= blocksize

# get power from first 60000 samples (i.e. no pulse)
# for fx and newFx

fx[...] -= fx[...].sum() / float(blocksize) # subtract DC offset for RMS calculation!

newPower = hArray(float, dimensions=[96])
dirtyPower = hArray(float, dimensions=[96])

newPower[...] = newFx[..., 0:60000].sumsquare() / 60000.0
dirtyPower[...] = fx[..., 0:60000].sumsquare() / 60000.0

from numpy import sqrt
print 'new power = %f' % sqrt(newPower[3])
print 'old power = %f' % sqrt(dirtyPower[3])

newPower.sqrt()
dirtyPower.sqrt()

x = np.arange(96)
fig = plt.figure()
fig.clf() 
ax = fig.add_subplot(1,1,1)
ax.set_xlim([0, 96])

ax.set_ylabel('RMS voltage (ADC units)')
ax.set_xlabel('RCU number')
ax.set_xticks(range(0, 96, 16))
ax.bar(x, dirtyPower.toNumpy(), color='r')
ax.bar(x, newPower.toNumpy(), color = 'b')

#fx[0].plot()
#newFx[0].plot()
plt.show()


