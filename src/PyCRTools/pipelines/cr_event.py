#! /usr/bin/env python

"""
**Usage:**
execfile(PYP+"pipelines/cr_event.py")

This is the basic LOFAR VHECR event processing script, including gain
calibration, RFI excision, pulse finding, direction finding,
beamforming.

**Parameters:**
======================= =================================================================
*filename*              filename of raw data to be processed (can include directory)

*lofarmode*             'LBA_INNER' or 'LBA_OUTER'

*polarization*          either 0 or 1 for selecting even or odd antennas

*outputdir*             directory where to store the final results (will be stored in a
                        subdirectory filename.dir/ within that directory) 
            
*block*                 (or block_with_peak) - in which block do you expect the peak

*plotpause*             pause and display each figure interactively?

*refresh*               refresh plotting window after each plot (e.g., during batch processing).

*maximum_allowed_delay* maximum differential mean cable delay that the
                        expected positions can differ rom the measured ones, before we
                        consider something to be wrong
======================= =================================================================

**Results:**

Data and results are stored in an hArray and its header. This can be
read back with ``event=hArrayRead(filename-polN.results)``, (filename
without the .h5 ending) in the ``outputdir/filename.dir`` directory
where all output can be found.

Some basic results are stored in the ``event.par.results`` dict. The
quality information dict is in ``event.par.quality``.  The shifted
time series data (corrected for time travel and cable delay) of all
antennas is in ``event.par.timeseries_data``. A summary of the ouput
with all figures can be viwed with a web browser from 
outputdir/filename.dir/index.html.

The results dict can also be read stand-alone from the file
results.py. Just use execfile(os.path.join(outputdir,"results.py")) and look for ``results``.



**Example: **

Command line use:

$PYP/pipelines/cr_event.py  ~/LOFAR/work/CR/LORAweekdump--2-15.h5 --lofarmode=LBA_INNER --outputdir=/Users/falcke/LOFAR/work/ --block=93 --polarization=0
------------------------------------------------------------------------

Test event: Event-1, LBA_OUTER
filename="lora-event-1-station-2.h5"; lofarmode="LBA_OUTER"
Azimuth (Eastwards from North) 232 degrees, Elevation 62.5 degrees
LORA estimate: 229.1 degrees, 62.4 degrees

Event-0, LBA_INNER: 
Azimuth (Eastwards from North) 9.4 degrees, Elevation 61.0 degrees
LORA estimate: 17.2 degrees, 62.9 degrees

                       Az=360-phi??

Event ID		Theta	Phi		Core pos.(X,Y) m        Energy (eV)                       Az?
-----------------------------------------------------------------------------------------------
1307920753		62.4	129.9	        (63.5, 176.4)       2.57e+17 (underestimated)     230    
1307923194		59.0	51.7	        (84.6, -61.5)		9.88e+15                      308
1307942115		62.9	342.8	        (-31.7 -114.5)		1.8e+17 (underestimated)      17.2
1309126700		74.2	335.7	        (-94.8, 34.7)		2.64e+16                      24
-----------------------------------------------------------------------------------------------

Revision History:
V1.0 created by H. Falcke, July 2011
"""

from pycrtools import *
from optparse import OptionParser

t0=time.clock()
#------------------------------------------------------------------------
#Command line options
#------------------------------------------------------------------------
usage = "usage: %prog [options] datafile.h5 "
parser = OptionParser(usage=usage)

parser.add_option("-o","--outputdir", type="str", default="",help="directory where to store the final results (will be stored in a subdirectory filename.dir/ within that directory)")
parser.add_option("-l","--lofarmode", type="str", default="LBA_OUTER",help="'LBA_INNER' or 'LBA_OUTER'")
parser.add_option("-p","--polarization", type="int", default=0,help="either 0 or 1 for selecting even or odd antennas")
parser.add_option("-b","--block", type="int", default=93,help="in which block do you expect the peak")
parser.add_option("-s","--blocksize", type="int", default=2**16,help="Blocksize with which to read blocks of data.")
parser.add_option("-q","--nopause", action="store_true",help="Do not pause after each plot and display each figure interactively")
parser.add_option("-R","--norefresh", action="store_true",help="Do not refresh plotting window after each plot.")
parser.add_option("-d","--maximum_allowed_delay", type="float", default=1e-8,help="maximum differential mean cable delay that the expected positions can differ rom the measured ones, before we consider something to be wrong")

#------------------------------------------------------------------------
# Main input parameters
#------------------------------------------------------------------------
if not parser.get_prog_name()=="cr_event.py":
#   Program was run from within python
    filename="~/LOFAR/work/CR/lora-event-1-station-2.h5"; lofarmode="LBA_OUTER"
    filename="~/LOFAR/work/CR/LORAweekdump--2-15.h5"; lofarmode="LBA_INNER"

    outputdir="/Users/falcke/LOFAR/work/"
    polarization=1 # either 1 or 0 for odd or even antennapolarization=1 # either 1 or 0 for odd or even antenna
    block_with_peak=93
    blocksize=2**16
    plotpause=False
    refresh=True
    maximum_allowed_delay=1e-8 # maximum differential mean cable delay
                               # that the expected positions can differ
                               # from the measured ones, before we
                               # consisder something to be wrong
else:
    #   Program was started from command line
    (options, args) = parser.parse_args()

    # Filenames
    if not args:
        print "Error: No HDF5 files specified for input"
        sys.exit(1)

    filename = args[0]
    lofarmode = options.lofarmode
    outputdir = options.outputdir
    polarization = options.polarization
    block_with_peak = options.block
    blocksize = options.blocksize
    plotpause = not options.nopause
    refresh = not options.norefresh
    maximum_allowed_delay = options.maximum_allowed_delay

#------------------------------------------------------------------------
plt.ioff()

########################################################################
#Setting filenames and directories
########################################################################

(filedir,filename)=os.path.split(os.path.expandvars(os.path.expanduser(filename)))
outputdir=os.path.expandvars(os.path.expanduser(outputdir))
outfilename=(filename[:-3] if filename[-3:].upper()==".H5" else filename)+"-pol"+str(polarization)

outputdir=os.path.join(outputdir,outfilename+".dir")
result_file=os.path.join(outputdir,outfilename+".results")
htmlfilename=os.path.join(outputdir,"index.html")

tasks.task_write_parfiles=True
tasks.task_outputdir=outputdir
nparfiles = len(tasks.task_parfiles)

#Removing old parfiles, if they exist
[os.remove(f) for f in listFiles([os.path.join(outputdir,"*.par"),os.path.join(outputdir,"*.par.txt"),os.path.join(outputdir,"*.gif")])]

print "filename    -->",filename
print "filedir     -->",filedir
print "outfilename -->",outfilename
print "outputdir   -->",outputdir
print "result_file -->",result_file
print "htmlfile    -->",htmlfilename
if tasks.task_write_parfiles:
    print "parfiles    -->",outputdir
else:
    print "# No parfiles will be written by tasks!"


#Create a directory containing the output files
if not os.path.exists(outputdir):
    print "# Creating output directory",outputdir
    os.mkdir(outputdir)
else:
    print "# Using existing output directory",outputdir

Pause=plotfinish(filename=os.path.join(outputdir,outfilename),plotpause=plotpause,refresh=refresh)

########################################################################
#Setting the parameter block with parameters for tasks
########################################################################

par=dict(
    plot_antenna=0,
    newfigure=False, #Don't create a new plotting window for some of the tasks, but use the old one.
    plot_finish=Pause,
    output_dir=outputdir,
    AverageSpectrum = dict(
        calc_incoherent_sum=True,
        addantennas=False,
        output_subdir=outputdir,
        filefilter=os.path.join(filedir,filename),
        lofarmode=lofarmode,
        antennas_start=polarization,
        antennas_stride=2,maxpeak=7,meanfactor=3,peak_rmsfactor=5,rmsfactor=2,spikeexcess=7,
        blocklen=4096, # only used for quality checking within one block
#        delta_nu=3000, # -> blocksize=2**16
        chunksize_estimated=blocksize, # -> blocksize=2**16
        rmsrange=(0.5,50) #Range of RMS that is allowed in timeseries
                          #before flagging. RMS is related to power,
                          #hence we check for power excesses or
                          #deficits in an antenna.
        ),
    FitBaseline = dict(ncoeffs=80,numin=30,numax=85,fittype="BSPLINE",splineorder=3),
    LocatePulseTrain = dict(nsigma=6,maxgap=5,minlen=64,minpulselen=3),  #nisgma=7  
#    DirectionFitTriangles = dict(maxiter=2,rmsfactor=0,minrmsfactor=0), # only do one step,all atennas at once
    DirectionFitTriangles = dict(maxiter=6,rmsfactor=2,minrmsfactor=0), # determine delays iteratively
    ApplyBaseline=dict(rmsfactor=7)
    )
#------------------------------------------------------------------------

#Note, that currently we keep all tasks and their workspaces. This
#means that not memory ever gets deallocated. That needs to be changed
#if memory ever becomes an issue. (In the other hand it needs not be
#reallocated, if eventually one loops over events - but that needs
#checking).


########################################################################
#Getting the average spectrum and quality flags
########################################################################
print "---> Calculating average spectrum of all antennas"
avspectrum=trerun("AverageSpectrum","",pardict=par,load_if_file_exists=True,doplot=0 if Pause.doplot else False)
calblocksize=avspectrum.power.getHeader("blocksize")
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
quality=avspectrum.power.getHeader("quality")
antennalist=[i["antenna"] for i in quality if i["chunk"]==0] # just the names of all antennas
antenna_index={} # names of antennas and their position within the array 
for i in range(len(antennalist)): antenna_index[antennalist[i]]=i

# Now, select all atennas where the interesting chunk (where the peak
# is) has two flagged blocks (one flagged block is OK, since this is
# where the peak sits)

#Select bad antennas
flaglist=[i for i in quality if i["chunk"]==block_with_peak and i["nblocksflagged"]>1]
bad_antennas=[i["antenna"] for i in flaglist]
bad_antennas_index=[antenna_index[i] for i in bad_antennas]

#Select good antennas
good_antennas=antennalist[:]
[good_antennas.remove(i) for i in bad_antennas] # remove the bad antennas
good_antennas_index=[antenna_index[i] for i in good_antennas] # get the index numbers of good antennas pointing to position in array
ndipoles=len(good_antennas)

#Create new average spectrum with only good antennas

if len(bad_antennas)>0:
    print "# Antenna Flagging:",len(bad_antennas),"bad antennas!"
    print "# Bad Antennas:",bad_antennas
    averagespectrum_good_antennas=hArray(dimensions=[ndipoles,speclen],properties=avspectrum.power)
    averagespectrum_good_antennas[...].copy(avspectrum.power[good_antennas_index,...])
else:
    print "# Antenna Flagging: All antennas OK!"
    averagespectrum_good_antennas=avspectrum.power
    
#raise KeyboardInterrupt("Forced end of Execution!")

########################################################################
#Baseline Fitting 
########################################################################

print "---> Fit a baseline to the average spectrum"
fitbaseline=trerun("FitBaseline","",averagespectrum_good_antennas,extendfit=0.5,pardict=par,doplot=3 if Pause.doplot else 0)

print "---> Calculate a smooth version of the spectrum which is later used to set amplitudes."
calcbaseline1=trerun("CalcBaseline",1,averagespectrum_good_antennas,pardict=par,invert=False,HanningUp=False,normalize=False,doplot=0)
amplitudes=hArray(copy=calcbaseline1.baseline)

#raise TypeError("I don't like your type .... (just for debugging)")

print "---> Calculate it again, but now to flatten the spectrum."
calcbaseline_flat=trerun("CalcBaseline","flat",averagespectrum_good_antennas,pardict=par,invert=True,doplot=Pause.doplot)
    
########################################################################
#RFI identification in sum of all antennas (incoherent station spectrum)
########################################################################
#Averaging over all antennas to get a single station spectrum and gain
#curve for RFI detection

print "---> Flatten spectrum and find channels with RFI spikes"
station_gaincurve=hArray(dimensions=calcbaseline_flat.baseline.shape()[-1],fill=0,properties=calcbaseline_flat.baseline,name="Station Spectrum")
calcbaseline_flat.baseline[...].addto(station_gaincurve)
station_gaincurve /= ndipoles

station_spectrum=hArray(dimensions=averagespectrum_good_antennas.shape()[-1],fill=0,properties=averagespectrum_good_antennas,name="Station Spectrum")
averagespectrum_good_antennas[...].addto(station_spectrum)
station_spectrum /= ndipoles
station_spectrum.par.baseline=station_gaincurve

applybaseline=trerun("ApplyBaseline","",station_spectrum,baseline=station_gaincurve,pardict=par,doplot=Pause.doplot)

#End of calibration preparations

########################################################################
#Open data file
########################################################################

print "---> Load the block with the peak"
datafile=open(os.path.join(filedir,filename)); datafile["ANTENNA_SET"]=lofarmode
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
timeseries_data=datafile["EMPTY_TIMESERIES_DATA"]
timeseries_data.read(datafile,"TIMESERIES_DATA")
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

########################################################################
#Gain calibration of data
########################################################################
print "---> Calculate a baseline with Galactic powerlaw"
calcbaseline_galactic=trerun("CalcBaseline","galactic",averagespectrum_good_antennas,pardict=par,invert=True,powerlaw=0.5,doplot=Pause.doplot)

# and apply
fft_data.mul(calcbaseline_galactic.baseline)

#Plotting just for quality control
power=hArray(float,properties=fft_data)
power.spectralpower(fft_data)
if Pause.doplot: power[0:min(4,ndipoles),...].plot(title="Calibrated spectra of first 4 antennas")
Pause("Plotted corrected spectrum. ",name="calibrated-spectrum")

########################################################################
#Back to time domain
########################################################################
timeseries_data2=hArray(properties=timeseries_data)
fft_data[...,0]=0 # take out zero offset (-> offset/mean==0)
hFFTWExecutePlan(timeseries_data2[...], fft_data[...], invfftplan)

#timeseries_data2[...].invfftw(fft_data[...])
timeseries_data2 /= blocksize # normalize back to original value

if Pause.doplot: timeseries_data2[0:min(2,ndipoles),...].plot(title="Calibrated time series of first 2 antennas")
Pause("Plotted time series data. ",name="calibrated-imeseries")

########################################################################
#Locate pulse and cut data around it
########################################################################
#First determine where the pulse is in a simple incoherent sum of all time series data
print "---> Now add all antennas in the time domain, locate pulse and cut time series around it"
pulse=trerun("LocatePulseTrain","",timeseries_data2,pardict=par)

print "#LocatePulse: ",pulse.npeaks,"pulses found."
if pulse.npeaks==0:
    print "************************************************************************"
    print "********          ATTENTION: No pulses found          ******************"
    print "************************************************************************"
    raise ValueError("LocatePulseTrain: No pulses found!")
    
print "---> Get peaks in power of each antenna (Results in maxima_power.maxy/maxx)."
timeseries_power=hArray(copy=pulse.timeseries_data_cut)
timeseries_power.square()
timeseries_power.runningaverage(5,hWEIGHTS.GAUSSIAN)
maxima_power=trerun('FitMaxima',"Power",timeseries_power,pardict=par,doplot=Pause.doplot,refant=0,plotend=ndipoles,sampleinterval=sample_interval,peak_width=11,splineorder=3)
Pause(name="pulse-maxima-power")

########################################################################
#Cross correlate and get time lags
########################################################################
print "---> Cross correlate pulses, get time lags, and determine direction of pulse."
#Now cross correlate all pulses with each other 
crosscorr=trerun('CrossCorrelateAntennas',"crosscorr",pulse.timeseries_data_cut,pardict=par,oversamplefactor=10)
#And determine the relative offsets between them
maxima=trerun('FitMaxima',"Lags",crosscorr.crosscorr_data,pardict=par,doplot=Pause.doplot,refant=0,plotend=5,sampleinterval=sample_interval/crosscorr.oversamplefactor,peak_width=11,splineorder=3)
Pause(name="pulse-maxima-crosscorr")

print "Time lag [ns]: ", maxima.lags 
print " "

########################################################################
#Direction fitting based on time lags
########################################################################
#Now fit the direction and iterate over cable delays to get a stable
#solution

#cabledelays.negate()
delays=hArray(copy=cabledelays)
#cabledelays *= 2
#delays.fill(0)

direction=trerun("DirectionFitTriangles","direction",pardict=par,positions=antenna_positions,timelags=hArray(maxima.lags),cabledelays=cabledelays,verbose=True,doplot=True)
print "========================================================================"
print "Triangle Fit Az/EL -> ", direction.meandirection_azel_deg,"deg"
print " "
print "Delays =",direction.total_delays * 1e9

print "#DirectionFitTriangles: delta delays =",direction.delta_delays_mean_history[0],"+/-",direction.delta_delays_rms_history[0]

(direction.total_delays*1e9).plot()
(direction.delays_history*1e9)[1].plot(clf=False)
(cabledelays*1e9).plot(clf=False,xlabel="Antenna",ylabel="Delay [n]",title="Fitted cable delays",legend=(["total delay","1st step","cable delay"]))
Pause(name="fitted-cable-delays")

if abs(direction.delta_delays_rms_history[0])>maximum_allowed_delay or abs(direction.delta_delays_mean_history[0])>maximum_allowed_delay:
    print "************************************************************************"
    print "********ATTENTION: Fitted delays deviate too strongly!******************"
    print "********            check antennna positions          ******************"
    print "************************************************************************"
    print "#DirectionFitTriangles: ERROR!"
else:
    print "#DirectionFitTriangles: OK!"

#plotdirection=trerun("PlotDirectionTriangles","plotdirection",pardict=par,centers=direction.centers,positions=direction.positions,directions=direction.directions,title="Sub-Beam directions")
#Pause(name="sub-beam-directions")

trerun("PlotAntennaLayout","TimeLags",pardict=par,positions=antenna_positions,colors=hArray(maxima.lags),sizes=hArray(maxima_power.maxy),sizes_min=0,names=good_antennas_index,title="Time Lags in Station",plotlegend=True)

trerun("PlotAntennaLayout","Delays",pardict=par,positions=antenna_positions,colors=direction.total_delays,sizes=100,names=good_antennas_index,title="Delay errors in station",plotlegend=True)

print "\n--->Beamforming"

########################################################################
#Beamforming
########################################################################

#Beamform short data set first for inspection (and possibly for
#maximizing later)
bf=trerun("BeamFormer2","bf",data=pulse.timeseries_data_cut,pardict=par,maxnantennas=ndipoles,antpos=antenna_positions,FarField=True,sample_interval=sample_interval,pointings=rf.makeAZELRDictGrid(*(direction.meandirection_azel+(10000,)),nx=3,ny=3),cable_delays=direction.total_delays,calc_timeseries=True,doplot=2 if Pause.doplot else False,doabs=True,smooth_width=5,plotspec=False,verbose=False)

#Use the above later also for maximizing peak Beam-formed timeseries
#is in ---> bf.tbeams[bf.mainbeam]

print "---> Plotting mosaic of beams around central direction"
if Pause.doplot:
    bf.tplot(smooth=9)
    Pause(name="beamformed-multiple-directions")

timeseries_power_shifted=hArray(fill=bf.data_shifted,properties=pulse.timeseries_data_cut,name="Power(t)")
timeseries_power_shifted.square()
timeseries_power_shifted[...].runningaverage(9,hWEIGHTS.GAUSSIAN)
if Pause.doplot:
    timeseries_power_shifted[...].plot(title="E-Field^2 corrected for delays in beam direction per antenna",xlabel="Samples")
    plt.legend(good_antennas,ncol=2,title="Antennas",prop=matplotlib.font_manager.FontProperties(family="Helvetica",size=7))
    Pause(name="shifted-power-per-antenna")

#matplotlib.font_manager.FontProperties(self, family=None, style=None, variant=None, weight=None, stretch=None, size=None, fname=None, _init=None)


print "---> Plotting full beam-formed data set"
#Beamform full data set (not really necessary, but fun).
beamformed=trerun("BeamFormer2","beamformed",data=pulse.timeseries_data,pardict=par,maxnantennas=ndipoles,antpos=antenna_positions,FarField=True,sample_interval=sample_interval,pointings=rf.makeAZELRDictGrid(*(direction.meandirection_azel+(10000,)),nx=1,ny=1),cable_delays=direction.total_delays,calc_timeseries=False,doabs=False,smooth_width=0,doplot=False,plotspec=False,verbose=False)

########################################################################
#Data Analysis ... (to be expanded)
########################################################################

#Produce nice looking plot of peak
event=hArray(dimensions=[blocksize],copy=beamformed.tbeams,name="Beamformed Power",xvalues=timeseries_data.par.xvalues)
event.abs()
event.runningaverage(7,hWEIGHTS.GAUSSIAN)
if Pause.doplot:
    plt.ioff()
    event.plot(title="Beamformed Time Series")
    Pause(name="pulse-beamformed")

if Pause.doplot:
    plt.ioff()
    event[pulse.start:pulse.end].plot(title="Beamformed Time Series")
    Pause(name="pulse-beamformed-zoomed")

event_dummy=hArray(event.vec(),dimensions=[1,blocksize])
beam_maxima=trerun('FitMaxima',"Beam",event_dummy,doplot=Pause.doplot,pardict=par,refant=0,sampleinterval=sample_interval,peak_width=11,splineorder=3)
pulse_time_ms=(timeseries_data.par.xvalues[int(floor(beam_maxima.maxx.val()))]+beam_maxima.maxx.val()%1*sample_interval*1e6)/1000.

print "# The pulse is expected between samples ",pulse.start,"and",pulse.end
print "# This corresponds to the time frame ",timeseries_data.par.xvalues[pulse.start]/1000.,"-",timeseries_data.par.xvalues[pulse.end]/1000.,"ms (i.e., {0:d} ms + {1:6.3f}-{2:6.3f} mus)".format(int(timeseries_data.par.xvalues[pulse.start]/1000),round(timeseries_data.par.xvalues[pulse.start] % 1000.,3),round(timeseries_data.par.xvalues[pulse.end] % 1000.,3))
print "# Peak found at sample {0:8.2f} ({1:11.8f} ms = {2:d} ms + {3:9.5f} mus) with height {4:8.2f} units.".format(beam_maxima.maxx.val(),pulse_time_ms,int(pulse_time_ms),(pulse_time_ms%1)*1000,beam_maxima.maxy.val())

########################################################################
#Preparing and writing results record
########################################################################

results=dict(
    TELESCOPE=datafile["TELESCOPE"],
    ANTENNA_SET=datafile["ANTENNA_SET"],
    NYQUIST_ZONE=datafile["NYQUIST_ZONE"][0],
    SAMPLE_FREQUENCY=datafile["SAMPLE_FREQUENCY"][0],
    FREQUENCY_RANGE=datafile["FREQUENCY_RANGE"][0],
    NOF_DIPOLE_DATASETS=datafile["NOF_DIPOLE_DATASETS"],
    DATA_LENGTH=datafile["DATA_LENGTH"][0],
    SAMPLE_INTERVAL=sample_interval,
    FILENAME=filename,
    BLOCK=block_with_peak,
    BLOCKSIZE=blocksize,
    polarization=polarization,
    plotfiles=Pause.plotfiles,
    filedir=filedir,
    ndipoles=ndipoles,
    antennas=good_antennas,
    antenna_positions_XYZ_m=list(antenna_positions.vec()),
    bad_antennas=bad_antennas,
    pulses_maxima_x=list(maxima_power.maxx),
    pulses_maxima_y=list(maxima_power.maxy),
    npeaks_found=pulse.npeaks,
    pulses_timelags_ns=list(maxima.lags),
    pulse_start_sample=pulse.start,
    pulse_end_sample=pulse.end,
    pulse_time_ms=pulse_time_ms,
    pulse_height=beam_maxima.maxy.val(),
    pulse_direction=direction.meandirection_azel_deg,
    pulse_direction_delta_delays_start=direction.delta_delays_mean_history[0],
    pulse_direction_delta_delays_final=direction.delta_delays_mean_history[-1],
    )

event.par.quality=quality
event.par.timeseries_data=pulse.timeseries_data_cut
event.par.results=results
event.write(result_file)

#Writing results.py
f=open(os.path.join(outputdir,"results.py"),"w")
f.write("#"+outfilename+"\nresults="+str(results))
f.close()

########################################################################
#Writing summary output to html file
########################################################################
htmlfile=open(htmlfilename,"w")
htmlfile.write("<html><head><title>{0:s}</title></head><body>\n".format(outfilename))
htmlfile.write("<h1>{0:s}</h1>\n".format(outfilename))
htmlfile.write("<h2>Parameters</h2>\n".format())
htmlfile.write("<i>File processed on {0:s}, by user {1:s}. Processing time = {2:5.2f}s</i><br>\n".format(
    time.strftime("%A, %Y-%m-%d at %H:%M:%S"),os.getlogin(),time.clock()-t0))

l=results.items(); l.sort()
htmlfile.write('<table border="1">\n'.format())
for k,v in l:
    htmlfile.write("<tr><td><b>{0:s}</b></td><td>{1:s}</td></tr>\n".format(k,str(v)))
htmlfile.write("</table>\n".format())

htmlfile.write("<h2>Parfiles</h2>\n".format())
for i in range(nparfiles,len(tasks.task_parfiles)):
    if os.path.exists(tasks.task_parfiles[i]):
        os.rename(tasks.task_parfiles[i],tasks.task_parfiles[i]+".txt")
        htmlfile.write('<a type="text/http" href="{0:s}.txt">{1:s}</a><br>\n'.format(tasks.task_parfiles[i],os.path.split(tasks.task_parfiles[i])[-1]))

htmlfile.write("<h2>Plotfiles</h2>\n".format())
for f in results["plotfiles"]:
    htmlfile.write('<a href="{0:s}">{0:s}</a>:<br><a href="{0:s}"><img src="{0:s}" width=500></a><p>\n'.format(os.path.split(f)[-1]))

htmlfile.write("</body></html>\n")
htmlfile.close()    
    
print "Data and results written to file. Read back with event=hArrayRead('"+result_file+"')"
print "Basic parameters and results are in the dicts ``results`` or ``event.par.results``,"
print "which can be found in `results.py`(use, e.g., execfile)."
print "Shifted time series data of all antennas is in event.par.time_series."
print "Open",htmlfilename,"in your browser to get a summary."
print "-----------------------------------------------------------------------------------------------------------"
print "Finished cr_event after",time.clock()-t0,"seconds."
plt.ion()
