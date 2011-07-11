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
datafile=open(filedir+filename); datafile["ANTENNA_SET"]=lofarmode
blocksize=avspectrum.power.getHeader("blocksize")
#selected_dipoles=list(avspectrum.power.getHeader("antennas_used"))
#datafile["SELECTED_DIPOLES"]=selected_dipoles
datafile["SELECTED_DIPOLES"]=list(avspectrum.power.getHeader("antennas"))

#datafile["SELECTED_DIPOLES"]="odd"
selected_dipoles=datafile["SELECTED_DIPOLES"]
ndipoles=len(selected_dipoles)
sample_interval=datafile["SAMPLE_INTERVAL"][0]
antenna_positions=datafile["ANTENNA_POSITIONS"]

"""
['002000001',
 '002000003',
 '002000005',
 '002000007',
 '002001009',
 '002001011',
 '002001013',
 '002001015',
 '002002017',
 '002002019',
 '002002021',
 '002002023',
 '002003025',
 '002003027',
 '002003029',
 '002003031',
 '002004033',
 '002004035',
 '002004037',
 '002004039',
 '002005041',
 '002005043',
 '002005045',
 '002005047',
 '002006049',
 '002006051',
 '002006053',
 '002006055',
 '002007057',
 '002007059',
 '002007061',
 '002007063',
 '002008065',
 '002008067',
 '002008069',
 '002008071',
 '002009073',
 '002009075',
 '002009077',
 '002009079',
 '002010081',
 '002010083',
 '002010085',
 '002010087',
 '002011089',
 '002011091',
 '002011093',
 '002011095']
"""

#Getting original cabledelays
cabledelays_full=metadata.get("CableDelays",datafile["CHANNEL_ID"],datafile["ANTENNA_SET"])  # Obtain cabledelays
cabledelays_full-=cabledelays_full[0] # Correct w.r.t. referecence antenna
cabledelays=cabledelays_full % sample_interval # Only sub-sample correction has not been appliedcabledelays=cabledelays_full % 5e-9  # Only sub-sample correction has not been applied
cabledelays=hArray(cabledelays)


print "---> Load the block with the peak"
datafile["BLOCKSIZE"]=blocksize #2**16
datafile["BLOCK"]=block_with_peak #93

timeseries_data=datafile["TIMESERIES_DATA"]
timeseries_data.setUnit("","ADC Counts")
timeseries_data.par.xvalues=datafile["TIME_DATA"]
timeseries_data.par.xvalues.setUnit("","s")
timeseries_data.par.xvalues.setUnit("mu","")

print "---> FFT, take our RFI, and apply baseline (amplitude) calibration for each antenna"
#fft_data=file["EMPTY_FFT_DATA"]
fft_data=hArray(complex,[ndipoles,blocksize/2+1],name="FFT",par=dict(logplot="y"),units="arb. units")
fft_data.par.xvalues=datafile["FREQUENCY_DATA"]
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
maxima=trun('FitMaxima',crosscorr.crosscorr_data,doplot=True,refant=0,plotend=5,sampleinterval=sample_interval/crosscorr.oversamplefactor,peak_width=6,splineorder=2)
k=raw_input("Press 'return' to continue.")

#wrong ones:
#Vector(float, 48, fill=[0,-6.25e-10,2.23437e-08,4.92188e-08,5.45313e-08,5.51563e-08,4.04688e-08,3.3125e-08,1.20312e-08,5.9375e-09,-1.875e-09,-1.1875e-08,-2.96875e-09,1.03125e-08,1.29688e-08,3.10937e-08,5.35937e-08,6.15625e-08,6.73437e-08,5.84375e-08,4.85938e-08,3.29688e-08,8.59375e-09,-1.20312e-08,-2.625e-08,-1.64063e-08,-5.46875e-09,9.375e-09,3.78125e-08,6.29688e-08,7.4375e-08,7.23437e-08,7.8125e-08,6.3125e-08,5.28125e-08,3e-08,1.84375e-08,-1.20312e-08,-1.45312e-08,-2.3125e-08,-2.32813e-08,7.34375e-09,3.64062e-08,5.76563e-08,9e-08,7.14063e-08,5.75e-08,-9.53125e-09])

#Vector(float, 48, fill=[0,-7.8125e-10,2.21875e-08,4.89063e-08,5.45313e-08,5.51563e-08,4.03125e-08,3.29688e-08,1.1875e-08,5.78125e-09,-1.875e-09,-1.20312e-08,-3.28125e-09,1.0625e-08,1.29688e-08,3.09375e-08,5.32812e-08,6.125e-08,6.71875e-08,5.8125e-08,4.82812e-08,3.28125e-08,8.28125e-09,-1.20312e-08,-2.64063e-08,-1.64063e-08,-5.625e-09,9.375e-09,3.75e-08,6.28125e-08,7.42188e-08,7.21875e-08,7.79687e-08,6.29688e-08,5.25e-08,2.96875e-08,1.82813e-08,-1.21875e-08,-1.46875e-08,-2.32813e-08,-2.34375e-08,7.1875e-09,3.60937e-08,5.75e-08,8.98438e-08,7.10938e-08,5.73438e-08,-9.84375e-09])
print "Time lag [ns]: ", maxima.lags 
print " "

#Now fit the direction and iterate over cable delays to get a stable solution
delays=hArray(copy=cabledelays)
delays.fill(0)

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

