"""
Test beamforming and RFI excision for CR event

Run with
execfile(PYP+"/scripts/testcr4.py")
execfile('/Users/falcke/LOFAR/usg/src/CR-Tools/implement/Pypeline/scripts/testcr4.py')
"""
from pycrtools import *

"""
Event-1: LBA_OUTER
Azimuth (Eastwards from North) 232 degrees, Elevation 62.5 degrees
LORA estimate: 229.1 degrees, 62.4 degrees
"""

filename="lora-event-1-station-2.h5"
filedir="/Users/falcke/LOFAR/work/CR/"
lofarmode="LBA_OUTER"
block_with_peak=93

par=dict(
    plot_antenna=39,
    AverageSpectrum = dict(
        addantennas=False,
        filefilter=filedir+filename,
        lofarmode=lofarmode,
        antennas_start=1,
        antennas_stride=2,maxpeak=7,meanfactor=3,peak_rmsfactor=5,rmsfactor=2,spikeexcess=7,
        delta_nu=3000 # -> blocksize=2**16
        ),
    
    FitBaseline = dict(
        ncoeffs=80,
        numin=30,
        numax=85,
        doplot=3,
        fittype="BSPLINE",
        splineorder=3),
    
    BeamFormer = dict(
        pointings=rf.makeAZELRDictGrid(143.4092*deg, 81.7932*deg, 600.3),
        blocklen=2**17,
        nantennas_stride=2,
        nantennas_start=1
        ),
    
    ApplyBaseline=dict(
        rmsfactor=7,
        doplot=True
        )
    )


plt.hanging=False # Use true if exectuion hangs after plotting one window
#plt.EDP64bug=True # use True if your system crashes for plotting semilog axes


print "---> Calculating average spectrum of all (odd/even) antennas"
avspectrum=trun("AverageSpectrum",pardict=par,load_if_file_exists=True)

print "---> Fit a baseline to the average spectrum"
fitbaseline=trun("FitBaseline",avspectrum.power,extendfit=0.5,pardict=par)

print "---> Calculate a smooth version of the spectrum which is later used to set amplitudes."
calcbaseline1=trun("CalcBaseline",avspectrum.power,pardict=par,invert=False,HanningUp=False,normalize=False,doplot=0)
amplitudes=hArray(copy=calcbaseline1.baseline)
amplitudes.sqrt()

print "---> Calculate it again, but now to flatten the spectrum."
calcbaseline2=trun("CalcBaseline",avspectrum.power,pardict=par,doplot=True)

    
print "---> Flatten spectrum and find channels with RFI spikes"

#Averaging over all antennas to get a single station spectrum and gain curve for RFI detection
station_gaincurve=hArray(dimensions=calcbaseline2.baseline.shape()[-1],fill=0,properties=calcbaseline2.baseline,name="Station Spectrum")
calcbaseline2.baseline[...].addto(station_gaincurve)
station_gaincurve /= avspectrum.power.shape()[0]

station_spectrum=hArray(dimensions=avspectrum.power.shape()[-1],fill=0,properties=avspectrum.power,name="Station Spectrum")
avspectrum.power[...].addto(station_spectrum)
station_spectrum /= avspectrum.power.shape()[0]
station_spectrum.par.baseline=station_gaincurve

applybaseline=trun("ApplyBaseline",station_spectrum,baseline=station_gaincurve,pardict=par)
#End of calibration preparations

#Set some parameters for further processing the pulse
file=open(filedir+filename); file["ANTENNA_SET"]=lofarmode
blocksize=avspectrum.power.getHeader("blocksize")
selected_dipoles=list(avspectrum.power.getHeader("antennas_used"))
file["SELECTED_DIPOLES"]=selected_dipoles
ndipoles=len(selected_dipoles)
sample_interval=file["SAMPLE_INTERVAL"][0]

#Getting original cabledelays
cabledelays_full=metadata.get("CableDelays",file["CHANNEL_ID"],file["ANTENNA_SET"])  # Obtain cabledelays
cabledelays_full-=cabledelays_full[0] # Correct w.r.t. referecence antenna
cabledelays=cabledelays_full % sample_interval # Only sub-sample correction has not been appliedcabledelays=cabledelays_full % 5e-9  # Only sub-sample correction has not been applied
cabledelays=hArray(cabledelays)


print "---> Load the block with the peak"
file["BLOCKSIZE"]=blocksize #2**16
file["BLOCK"]=block_with_peak #93
file["SELECTED_DIPOLES"]=selected_dipoles #odd
antenna_positions=file["ANTENNA_POSITIONS"]

timeseries_data=file["TIMESERIES_DATA"]
timeseries_data.setUnit("","ADC Counts")
timeseries_data.par.xvalues=file["TIME_DATA"]
timeseries_data.par.xvalues.setUnit("","s")
timeseries_data.par.xvalues.setUnit("mu","")

print "---> FFT, take our RFI, and apply baseline (amplitude) calibration for each antenna"
#fft_data=file["EMPTY_FFT_DATA"]
fft_data=hArray(complex,[ndipoles,blocksize/2+1],name="FFT",par=dict(logplot="y"),units="arb. units")
fft_data.par.xvalues=file["FREQUENCY_DATA"]
fft_data.par.xvalues.setUnit("","Hz")
fft_data.par.xvalues.setUnit("M","")

#FFT
fftplan = FFTWPlanManyDftR2c(blocksize, 1, 1, 1, 1, 1, fftw_flags.ESTIMATE)
hFFTWExecutePlan(fft_data[...], timeseries_data[...], fftplan)

#RFI excision step
fft_data[...].randomizephase(applybaseline.dirty_channels[...,[0]:applybaseline.ndirty_channels.vec()],amplitudes[...])
fft_data.mul(calcbaseline2.baseline)

#Plotting just for quality control
power=hArray(float,properties=fft_data)
power.spectralpower(fft_data)
power[0:4,...].plot()
k=raw_input("Plotted corrected spectrum. Press 'return' to continue.")


#inverse FFT back to timeseries data
timeseries_data2=hArray(properties=timeseries_data)
timeseries_data2[...].invfftw(fft_data[...])
timeseries_data2[0:2,...].plot()
k=raw_input("Plotted time series data. Press 'return' to continue.")

#First determine where the pulse is in a simple incoherent sum of all time series data
print "---> Now add all antennas in the time domain, locate pulse and cut time series around it"
pulse=trun("LocatePulseTrain",timeseries_data2,nsigma=7,maxgap=3,minlen=64)


print "---> Cross correlate pulses, get time lags, and determine direction of pulse."
#Now cross correlate all pulses with each other 
crosscorr=trun('CrossCorrelateAntennas',pulse.timeseries_data_cut,oversamplefactor=2)

#And determine the relative offsets between them
maxima=trun('FitMaxima',crosscorr.crosscorr_data,doplot=False,refant=0,plotend=10,sampleinterval=sample_interval/crosscorr.oversamplefactor,peak_width=6,splineorder=2)

print "Time lag [ns]: ", maxima.lags 
print " "

#Now fit the direction and iterate over cable delays to get a stable solution
delays=hArray(copy=cabledelays)
#delays.fill(0)
direction=trun("DirectionFitTriangles",positions=antenna_positions,timelags=hArray(maxima.lags),delays=delays,maxiter=9,verbose=True,doplot=False)
print "========================================================================"
print "Triangle Fit Az/EL -> ", direction.meandirection_azel_deg,"deg"
print " "
print "Delays =",direction.delays * 1e9


print "\n--->Beamforming"

#Beamform short data set first for inspection (and possibly for maximizing later)
bf=trun("BeamFormer2",data=pulse.timeseries_data_cut,maxnantennas=ndipoles,antpos=antenna_positions,FarField=True,sample_interval=sample_interval,pointings=rf.makeAZELRDictGrid(*(direction.meandirection_azel+(10000,)),nx=3,ny=3),cable_delays=direction.delays,calc_timeseries=True,doplot=2,doabs=True,smooth_width=5,plotspec=False,verbose=False)
#Use this also for maximizing peak
#Beam-formed timeseries is in ---> bf.tbeams[bf.mainbeam]
k=raw_input("Press 'return' to continue.")

print "---> Plotting mosaic of beams around central direction"
bf.tplot()
k=raw_input("Press 'return' to continue.")

print "---> Plotting full beam-formed data set"
#Beamform full data set (not really necessary, but fun).
beamformed_long=trun("BeamFormer2",data=pulse.timeseries_data,maxnantennas=ndipoles,antpos=antenna_positions,FarField=True,sample_interval=sample_interval,pointings=rf.makeAZELRDictGrid(*(direction.meandirection_azel+(10000,)),nx=1,ny=1),cable_delays=direction.delays,calc_timeseries=False,doabs=False,smooth_width=0,doplot=False,plotspec=False,verbose=False)



#Produce nice looking peak
smooth_beam=hArray(dimensions=[blocksize],copy=beamformed_long.tbeams)
smooth_beam.abs()
smooth_beam.runningaverage(7,hWEIGHTS.GAUSSIAN)
smooth_beam.plot()

