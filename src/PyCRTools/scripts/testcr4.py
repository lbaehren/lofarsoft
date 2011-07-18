"""
Basic LOFAR VHECR event processing, including gain calibration, RFI
excision, pulse finding, direction finding, beamforming.

Run with:
execfile(PYP+"/scripts/testcr4.py")
execfile('/Users/falcke/LOFAR/usg/src/CR-Tools/implement/Pypeline/scripts/testcr4.py')

Test event: Event-1, LBA_OUTER
filename="lora-event-1-station-2.h5"; lofarmode="LBA_OUTER"
Azimuth (Eastwards from North) 232 degrees, Elevation 62.5 degrees
LORA estimate: 229.1 degrees, 62.4 degrees

Event-0, LBA_INNER: 
Azimuth (Eastwards from North) 9.4 degrees, Elevation 61.0 degrees
LORA estimate: 17.2 degrees, 62.9 degrees


Revision History:
V1.0 created by H. Falcke, July 2011
"""

from pycrtools import *

#------------------------------------------------------------------------
# Main input parameters
#------------------------------------------------------------------------
filename="lora-event-1-station-2.h5"; lofarmode="LBA_OUTER"
filename="LORAweekdump--2-15.h5"; lofarmode="LBA_INNER"

filedir="/Users/falcke/LOFAR/work/CR/"
block_with_peak=93
maximum_allowed_delay=1e-8 # maximum differential mean cable delay
                           # that the expected positions can differ
                           # from the measured ones, before we
                           # consisder something to be wrong
#------------------------------------------------------------------------

Pause=plotpause()

par=dict(
    plot_antenna=39,
    newfigure=False, #Don't create a new plotting window for some of the tasks, but use the old one.
    plot_pause=Pause,
    AverageSpectrum = dict(
        addantennas=False,
        filefilter=filedir+filename,
        lofarmode=lofarmode,
        antennas_start=1,
        antennas_stride=2,maxpeak=7,meanfactor=3,peak_rmsfactor=5,rmsfactor=2,spikeexcess=7,
        blocklen=4096, # only used for quality checking within one block
        delta_nu=3000, # -> blocksize=2**16
        rmsrange=(0.5,50) #Range of RMS that is allowed in timeseries
                          #before flagging. RMS is related to power,
                          #hence we check for power excesses or
                          #deficits in an antenna.
        ),
    
    FitBaseline = dict(
        ncoeffs=80,
        numin=30,
        numax=85,
        fittype="BSPLINE",
        splineorder=3),
    
    ApplyBaseline=dict(
        rmsfactor=7
        )
    )
#------------------------------------------------------------------------

#Note, that currently we keep all tasks and their workspaces. This
#means that not memory ever gets deallocated. That needs to be changed
#if memory ever becomes an issue. (In the other hand it needs not be
#reallocated, if eventually one loops over events - but that needs
#checking).

#The following are the parameters for the tasks (not all parameters
#are already included here, stil tbd)

########################################################################
#Getting the average spectrum and quality flags
########################################################################
print "---> Calculating average spectrum of all (odd/even) antennas"
avspectrum=trun("AverageSpectrum",pardict=par,load_if_file_exists=True,doplot=0 if Pause.doplot else False)
blocksize=avspectrum.power.getHeader("blocksize")
speclen=avspectrum.power.shape()[-1] # note: this is not blocksize/2+1 ... (the last channel is missing!)

#Allocating the memory for the fft, if it doesn't exist yet
if globals().has_key("fftplan") or locals().has_key("fftplan"):
    print "#fftplan exists - reusing old one!"
else:
    fftplan = FFTWPlanManyDftR2c(blocksize, 1, 1, 1, 1, 1, fftw_flags.ESTIMATE)

if globals().has_key("invfftplan") or locals().has_key("invfftplan"):
    print "#invfftplan exists - reusing old one!"
else:
    invfftplan = FFTWPlanManyDftC2r(blocksize, 1, 1, 1, 1, 1, fftw_flags.ESTIMATE)

########################################################################
#Flagging antennas 
########################################################################
#Select from the quality list, produced by average spectrum, the ones
#which contain flags indicating a potential problem

#Define some convenience lists and dicts
qualitylist=avspectrum.power.getHeader("quality")
antennalist=[i["antenna"] for i in qualitylist if i["chunk"]==0] # just the names of all antennas
antenna_index={} # names of antennas and their position within the array 
for i in range(len(antennalist)): antenna_index[antennalist[i]]=i

# Now, select all atennas where the interesting chunk (where the peak
# is) has two flagged blocks (one flagged block is OK, since this is
# where the peak sits)

#Select bad antennas
flaglist=[i for i in qualitylist if i["chunk"]==block_with_peak and i["nblocksflagged"]>1]
bad_antennas=[i["antenna"] for i in flaglist]
bad_antennas_index=[antenna_index[i] for i in bad_antennas]

#Select good antennas
good_antennas=antennalist[:]
[good_antennas.remove(i) for i in bad_antennas] # remove the bad antennas
good_antennas_index=[antenna_index[i] for i in good_antennas] # get the index numbers of good antennas pointing to position in array
ndipoles=len(good_antennas)

#Create new average spectrum with only good antennas

if len(bad_antennas)>0:
    print "#Antenna Flagging:",len(bad_antennas),"bad antennas!"
    print "Bad Antennas:",bad_antennas
    averagespectrum_good_antennas=hArray(dimensions=[ndipoles,speclen],properties=avspectrum.power)
    averagespectrum_good_antennas[...].copy(avspectrum.power[good_antennas_index,...])
else:
    print "#Antenna Flagging: All antennas OK!"
    averagespectrum_good_antennas=avspectrum.power
    
#raise KeyboardInterrupt("Forced end of Execution!")

########################################################################
#Baseline Fitting 
########################################################################

print "---> Fit a baseline to the average spectrum"
fitbaseline=trun("FitBaseline",averagespectrum_good_antennas,extendfit=0.5,pardict=par,doplot=3 if Pause.doplot else 0)

print "---> Calculate a smooth version of the spectrum which is later used to set amplitudes."
calcbaseline1=trun("CalcBaseline",averagespectrum_good_antennas,pardict=par,invert=False,HanningUp=False,normalize=False,doplot=0)
amplitudes=hArray(copy=calcbaseline1.baseline)
amplitudes.sqrt()

print "---> Calculate it again, but now to flatten the spectrum."
calcbaseline2=trun("CalcBaseline",averagespectrum_good_antennas,pardict=par,doplot=Pause.doplot)
    
########################################################################
#RFI identification in sum of all antennas (incoherent station spectrum)
########################################################################
#Averaging over all antennas to get a single station spectrum and gain
#curve for RFI detection

print "---> Flatten spectrum and find channels with RFI spikes"
station_gaincurve=hArray(dimensions=calcbaseline2.baseline.shape()[-1],fill=0,properties=calcbaseline2.baseline,name="Station Spectrum")
calcbaseline2.baseline[...].addto(station_gaincurve)
station_gaincurve /= ndipoles

station_spectrum=hArray(dimensions=averagespectrum_good_antennas.shape()[-1],fill=0,properties=averagespectrum_good_antennas,name="Station Spectrum")
averagespectrum_good_antennas[...].addto(station_spectrum)
station_spectrum /= ndipoles
station_spectrum.par.baseline=station_gaincurve

applybaseline=trun("ApplyBaseline",station_spectrum,baseline=station_gaincurve,pardict=par,doplot=Pause.doplot)

#End of calibration preparations

########################################################################
#Open data file
########################################################################

print "---> Load the block with the peak"
datafile=open(filedir+filename); datafile["ANTENNA_SET"]=lofarmode
datafile["SELECTED_DIPOLES"]=good_antennas
sample_interval=datafile["SAMPLE_INTERVAL"][0]
antenna_positions=datafile["ANTENNA_POSITIONS"]
datafile["BLOCKSIZE"]=blocksize #2**16
datafile["BLOCK"]=block_with_peak #93

#Getting original cabledelays
cabledelays_full=metadata.get("CableDelays",datafile["CHANNEL_ID"],datafile["ANTENNA_SET"])  # Obtain cabledelays
cabledelays_full-=cabledelays_full[0] # Correct w.r.t. referecence antenna

cabledelays=cabledelays_full % sample_interval #Only sub-sample correction has not been appliedcabledelays=cabledelays_full % 5e-9  # Only sub-sample correction has not been applied
cabledelays=hArray(cabledelays)


########################################################################
#Read block with peak and FFT
########################################################################
timeseries_data=datafile["TIMESERIES_DATA"]
timeseries_data.setUnit("","ADC Counts")
timeseries_data.par.xvalues=datafile["TIME_DATA"]
timeseries_data.par.xvalues.setUnit("","s")
timeseries_data.par.xvalues.setUnit("mu","")

print "---> FFT, take out RFI, and apply baseline (amplitude) calibration for each antenna"
#fft_data=file["EMPTY_FFT_DATA"]
fft_data=hArray(complex,[ndipoles,blocksize/2+1],name="FFT",par=dict(logplot="y"),units="arb. units")
fft_data.par.xvalues=datafile["FREQUENCY_DATA"]
fft_data.par.xvalues.setUnit("","Hz")
fft_data.par.xvalues.setUnit("M","")

#FFT
hFFTWExecutePlan(fft_data[...], timeseries_data[...], fftplan)

########################################################################
#RFI excision
########################################################################
fft_data[...].randomizephase(applybaseline.dirty_channels[...,[0]:applybaseline.ndirty_channels.vec()],amplitudes[...])
fft_data.mul(calcbaseline2.baseline)

#Plotting just for quality control
power=hArray(float,properties=fft_data)
power.spectralpower(fft_data)
if Pause.doplot: power[0:4,...].plot()
Pause("Plotted corrected spectrum. Press 'return' to continue.")

########################################################################
#Back to time domain
########################################################################
timeseries_data2=hArray(properties=timeseries_data)
fft_data[...,0]=0 # take out zero offset (-> offset/mean==0)
hFFTWExecutePlan(timeseries_data2[...], fft_data[...], invfftplan)

#timeseries_data2[...].invfftw(fft_data[...])
timeseries_data2 /= blocksize # normalize back to original value

if Pause.doplot: timeseries_data2[0:2,...].plot()
Pause("Plotted time series data. Press 'return' to continue.")

########################################################################
#Locate pulse and cut data around it
########################################################################
#First determine where the pulse is in a simple incoherent sum of all time series data
print "---> Now add all antennas in the time domain, locate pulse and cut time series around it"
pulse=trun("LocatePulseTrain",timeseries_data2,nsigma=7,maxgap=3,minlen=64)

print "---> Get peaks in power of each antenna (Results in maxima_power.maxy/maxx)."
timeseries_power=hArray(copy=pulse.timeseries_data_cut)
timeseries_power.square()
timeseries_power.runningaverage(5,hWEIGHTS.GAUSSIAN)
maxima_power=trun('FitMaxima',timeseries_power,doplot=Pause.doplot,refant=0,plotend=ndipoles,sampleinterval=sample_interval,peak_width=11,splineorder=3)
Pause("Press 'return' to continue.")

########################################################################
#Cross correlate and get time lags
########################################################################
print "---> Cross correlate pulses, get time lags, and determine direction of pulse."
#Now cross correlate all pulses with each other 
crosscorr=trun('CrossCorrelateAntennas',pulse.timeseries_data_cut,oversamplefactor=10)
#And determine the relative offsets between them
maxima=trun('FitMaxima',crosscorr.crosscorr_data,doplot=Pause.doplot,refant=0,plotend=5,sampleinterval=sample_interval/crosscorr.oversamplefactor,peak_width=11,splineorder=3)
Pause("Press 'return' to continue.")

print "Time lag [ns]: ", maxima.lags 
print " "

########################################################################
#Direction fitting based on time lags
########################################################################
#Now fit the direction and iterate over cable delays to get a stable
#solution

delays=hArray(copy=cabledelays)
delays.fill(0)

direction=trun("DirectionFitTriangles",positions=antenna_positions,timelags=hArray(maxima.lags),delays=delays,maxiter=9,verbose=True,doplot=False)
print "========================================================================"
print "Triangle Fit Az/EL -> ", direction.meandirection_azel_deg,"deg"
print " "
print "Delays =",direction.delays * 1e9

print "#DirectionFitTriangles: delta delays =",direction.delta_delays_mean_history[0],"+/-",direction.delta_delays_rms_history[0]

if abs(direction.delta_delays_rms_history[0])>maximum_allowed_delay or abs(direction.delta_delays_mean_history[0])>maximum_allowed_delay:
    print "************************************************************************"
    print "********ATTENTION: Fitted delays deviate too strongly!******************"
    print "********            check antennna positions          ******************"
    print "************************************************************************"
    print "#DirectionFitTriangles: ERROR!"
else:
    print "#DirectionFitTriangles: OK!"
    
print "\n--->Beamforming"

########################################################################
#Beamforming
########################################################################

#Beamform short data set first for inspection (and possibly for
#maximizing later)
bf=trun("BeamFormer2",data=pulse.timeseries_data_cut,maxnantennas=ndipoles,antpos=antenna_positions,FarField=True,sample_interval=sample_interval,pointings=rf.makeAZELRDictGrid(*(direction.meandirection_azel+(10000,)),nx=3,ny=3),cable_delays=direction.delays,calc_timeseries=True,doplot=2 if Pause.doplot else False,doabs=True,smooth_width=5,plotspec=False,verbose=False)
Pause("Press 'return' to continue.")

#Use the above later also for maximizing peak Beam-formed timeseries
#is in ---> bf.tbeams[bf.mainbeam]

print "---> Plotting mosaic of beams around central direction"
if Pause.doplot: bf.tplot()
Pause("Press 'return' to continue.")

print "---> Plotting full beam-formed data set"
#Beamform full data set (not really necessary, but fun).
beamformed_long=trun("BeamFormer2",data=pulse.timeseries_data,maxnantennas=ndipoles,antpos=antenna_positions,FarField=True,sample_interval=sample_interval,pointings=rf.makeAZELRDictGrid(*(direction.meandirection_azel+(10000,)),nx=1,ny=1),cable_delays=direction.delays,calc_timeseries=False,doabs=False,smooth_width=0,doplot=False,plotspec=False,verbose=False)

########################################################################
#Data Analysis ... (to be expanded)
########################################################################

#Produce nice looking plot of peak
smooth_beam=hArray(dimensions=[blocksize],copy=beamformed_long.tbeams)
smooth_beam.abs()
smooth_beam.runningaverage(7,hWEIGHTS.GAUSSIAN)
smooth_beam.plot()

print "# The pulse is expected between samples ",pulse.start,"and",pulse.end
print "# This corresponds to the time frame ",timeseries_data.par.xvalues[pulse.start]/1000.,"-",timeseries_data.par.xvalues[pulse.end]/1000.,"ms (i.e., {0:d} ms + {1:6.3f}-{2:6.3f} mus)".format(int(timeseries_data.par.xvalues[pulse.start]/1000),round(timeseries_data.par.xvalues[pulse.start] % 1000.,3),round(timeseries_data.par.xvalues[pulse.end] % 1000.,3))

smooth_beam_dummy=hArray(smooth_beam.vec(),dimensions=[1,blocksize])
beam_maxima=trun('FitMaxima',smooth_beam_dummy,doplot=Pause.doplot,refant=0,sampleinterval=sample_interval,peak_width=11,splineorder=3)
t=(timeseries_data.par.xvalues[int(floor(beam_maxima.maxx.val()))]+beam_maxima.maxx.val()%1*sample_interval*1e6)/1000.
print "# 
