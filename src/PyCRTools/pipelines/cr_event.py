#! /usr/bin/env python

"""
**Usage:**
execfile(PYP+"pipelines/cr_event.py")

This is the basic LOFAR VHECR event processing script, including gain
calibration, RFI excision, pulse finding, direction finding,
beamforming.

**Parameters:**
======================= =================================================================
*filename*              filename(s) of raw data to be processed (can include directory,
                        evironment vaiables, asterisk, etc.)

*lofarmode*             'LBA_INNER' or 'LBA_OUTER'

*polarization*          either 0 or 1 for selecting even or odd antennas

*outputdir*             directory where to store the final results (will be stored in a
                        subdirectory filename.dir/ within that directory) 
            
*block*                 (or block_number) - in which block do you expect the peak

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

$PYP/pipelines/cr_event.py '~/LOFAR/work/data/VHECR_LORA-20110716T094509.665Z-*.h5' --lofarmode=LBA_OUTER --outputdir=/Users/falcke/LOFAR/work/results --loradir /Users/falcke/LOFAR/work/data/ --lora_logfile LORAtime4 --search_window_width=5000 --nsigma=3 -q -R
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
from pycrtools import lora

t0=time.clock()
tasks.task_logger=[]

#------------------------------------------------------------------------
#Command line options
#------------------------------------------------------------------------
usage = "usage: %prog [options] datafile.h5 "
parser = OptionParser(usage=usage)

parser.add_option("-o","--outputdir", type="str", default="",help="directory where to store the final results (will be stored in a subdirectory filename.dir/ within that directory)")
parser.add_option("-L","--lora_logfile", type="str", default="LORAtime4",help="LORA logfile (LORAtime4)")
parser.add_option("--loradir", type="str", default="/data/VHECR/LORAtriggered/LORA/",help="Directory to find LORA information")
parser.add_option("-l","--lofarmode", type="str", default="LBA_OUTER",help="'LBA_INNER' or 'LBA_OUTER'")
parser.add_option("-p","--polarization", type="int", default=-1,help="either 0 or 1 for selecting even or odd antennas, or -1 for both")
parser.add_option("-t","--nsigma", type="float", default=4.0,help="Threshold for identifying peaks")
parser.add_option("-b","--block", type="int", default=-1,help="in which block do you expect the peak")
parser.add_option("-w","--search_window_width", type="int", default=-1,help="Width of search window for peak around sample_number (in samples)")
parser.add_option("-n","--sample_number", type="int", default=-1,help="Sample number wherer peak is expected")
parser.add_option("-s","--blocksize", type="int", default=2**16,help="Blocksize with which to read blocks of data.")
parser.add_option("-T","--timestamp", type="str", default="",help="Timestamp of observation for creating output directory (if not to be deduced from filename)")
parser.add_option("-S","--station", type="str", default="",help="Station name for data set for creating output directory (if not to be deduced from filename)")
parser.add_option("-q","--nopause", action="store_true",help="Do not pause after each plot and display each figure interactively")
parser.add_option("-R","--norefresh", action="store_true",help="Do not refresh plotting window after each plot.")
parser.add_option("-d","--maximum_allowed_delay", type="float", default=1e-8,help="maximum differential mean cable delay that the expected positions can differ rom the measured ones, before we consider something to be wrong")

flag_delays = False

#------------------------------------------------------------------------
# Main input parameters
#------------------------------------------------------------------------
if not parser.get_prog_name()=="cr_event.py":
#   Program was run from within python
    filefilter="~/LOFAR/work/CR/lora-event-1-station-2.h5"; lofarmode="LBA_OUTER"
    filefilter="~/LOFAR/work/CR/LORAweekdump--2-15.h5"; lofarmode="LBA_INNER"
    filefilter="~/LOFAR/work/data/VHECR_LORA-20110716T094509.665Z-002.h5"; lofarmode="LBA_OUTER"


    outputdir="/Users/falcke/LOFAR/work/results"
    lora_logfile="/Users/falcke/LOFAR/work/data/LORAtime4"
    polarization=1 # either 1 or 0 for odd or even antennapolarization=1 # either 1 or 0 for odd or even antenna
    block_number=93
    blocksize=2**16
    nsigma=4
    timestamp=""
    station=""
    plotpause=False
    refresh=True
    search_window=False
    search_window_width=-1
    sample_number=-1
    
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

    filefilter = args
    lofarmode = options.lofarmode
    lora_logfile=options.lora_logfile
    loradir=options.loradir
    outputdir = options.outputdir
    polarization = options.polarization
    timestamp = options.timestamp
    station = options.station
    block_number = options.block
    nsigma = options.nsigma
    sample_number = options.sample_number
    blocksize = options.blocksize
    plotpause = not options.nopause
    refresh = not options.norefresh
    maximum_allowed_delay = options.maximum_allowed_delay
    search_window=False
    search_window_width=options.search_window_width
    sample_number=options.sample_number

#The Pause instance will pause (or not) after each plot and write the plotfiles
Pause=plotfinish(plotpause=plotpause,refresh=refresh)
timeseries_data_cut_pulse_width=12

#------------------------------------------------------------------------
plt.ioff()

########################################################################
#Loop over all files and polarizations
########################################################################

polarizations=[0,1] if polarization<0 else [polarization]

for current_polarization in polarizations:
    files = listFiles(filefilter)
    for full_filename in files:

        ########################################################################
        #Setting filenames and directories
        ########################################################################
    #    full_filename=os.path.expandvars(os.path.expanduser(filename))
        (filedir,filename)=os.path.split(full_filename)

        if not lora_logfile or not os.path.isfile(lora_logfile):
            lora_logfile=os.path.join(loradir,"LORAtime4")

        (rootfilename,fileextensions)=os.path.splitext(filename)

        filename_split=rootfilename.split("-")
        projectname="-".join(filename_split[:-2]) if len(filename_split)>2 else filename_split[0]

        station_name=filename_split[-1] if not station else station      
        time_stamp=filename_split[-2] if not timestamp else timestamp

        outputdir_expanded=os.path.expandvars(os.path.expanduser(outputdir))
        outputdir_with_subdirectories=os.path.join(outputdir_expanded,projectname+"-"+time_stamp,"pol"+str(current_polarization),station_name)
        outfilename=rootfilename+"-pol"+str(current_polarization)

        result_file=os.path.join(outputdir_with_subdirectories,outfilename+".results")
        htmlfilename=os.path.join(outputdir_with_subdirectories,"index.html")

        tasks.task_write_parfiles=True
        tasks.task_outputdir=outputdir_with_subdirectories
        nparfiles = len(tasks.task_parfiles)

        #Removing old parfiles, if they exist
        [os.remove(f) for f in listFiles([os.path.join(outputdir_with_subdirectories,"*.par"),os.path.join(outputdir_with_subdirectories,"*.par.txt"),os.path.join(outputdir_with_subdirectories,"*.gif"),os.path.join(outputdir_with_subdirectories,"*.png")])]

        print "filename    -->",filename
        print "filedir     -->",filedir
        print "outfilename -->",outfilename
        print "outputdir   -->",outputdir_with_subdirectories
        print "result_file -->",result_file
        print "htmlfile    -->",htmlfilename
        if tasks.task_write_parfiles:
            print "parfiles    -->",outputdir_with_subdirectories
        else:
            print "# No parfiles will be written by tasks!"

        #Create a directory containing the output files
        if not os.path.exists(outputdir_with_subdirectories):
            print "# Creating output directory",outputdir_with_subdirectories
            os.makedirs(outputdir_with_subdirectories)
        else:
            print "# Using existing output directory",outputdir_with_subdirectories

        #Reset the Pause instance, which will pause (or not) after each plot and write the plotfiles
        Pause.plotfiles=[]
        Pause.counter=0
        Pause.filename=os.path.join(outputdir_with_subdirectories,outfilename)


        ########################################################################
        #Open the data file
        ########################################################################
        print "---> Open data file",full_filename
        datafile=open(full_filename); datafile["ANTENNA_SET"]=lofarmode
        tbb_starttime=datafile["TIME"][0]
        tbb_samplenumber=datafile["SAMPLE_NUMBER"][0]
        sample_interval=datafile["SAMPLE_INTERVAL"][0]

        ########################################################################
        #Getting information from LORA if present
        ########################################################################
        print "---> Reading information from LORA, if present"
        if os.path.exists(lora_logfile):
            (tbb_starttime_sec,tbb_starttime_nsec)=lora.nsecFromSec(tbb_starttime,logfile=lora_logfile)
            if tbb_starttime_sec:
                (block_number_lora,sample_number_lora)=lora.loraTimestampToBlocknumber(tbb_starttime_sec,tbb_starttime_nsec,tbb_starttime,tbb_samplenumber,blocksize=blocksize)
                print "---> Estimated block number from LORA: block =",block_number_lora,"sample =",sample_number_lora
                if block_number<0:
                    block_number=block_number_lora
                if sample_number<0:
                    sample_number=sample_number_lora
                print "---> Taking as initial guess: block =",block_number,"sample =",sample_number
        else:
            print "WARNING: No LORA logfile found - ",lora_logfile

        lora_event_info=lora.loraInfo(tbb_starttime_sec,datadir=loradir,checkSurroundingSecond=True,silent=False)

        lora_direction=False; lora_energy=False; lora_core=False
        if lora_event_info:
                lora_direction=(lora_event_info["Az"],lora_event_info["El"])
                lora_core=(lora_event_info["Core(X)"],lora_event_info["Core(Y)"])
                lora_energy=lora_event_info["Energy(eV)"]

        ########################################################################
        #Setting the parameter block with parameters for tasks
        ########################################################################

        par=dict(
            plot_antenna=0,
            newfigure=False, #Don't create a new plotting window for some of the tasks, but use the old one.
            plot_finish=Pause,
            output_dir=outputdir_with_subdirectories,
            AverageSpectrum = dict(
                calc_incoherent_sum=True,
                addantennas=False,
                output_subdir=outputdir_with_subdirectories,
                filefilter=os.path.join(filedir,filename),
                root_filename=outfilename,
                lofarmode=lofarmode,
                antennas_start=current_polarization,
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
            CalcBaseline = dict(baseline=False), # Make sure baseline is recreated when the task is run a second time
            LocatePulseTrain = dict(nsigma=nsigma,maxgap=5,minlen=128,minpulselen=3,maxlen=128,prepulselen=32),  #nisgma=7  
        #    DirectionFitTriangles = dict(maxiter=2,rmsfactor=0,minrmsfactor=0), # only do one step,all atennas at once
            DirectionFitTriangles = dict(maxiter=6,rmsfactor=2,minrmsfactor=0,direction_guess=lora_direction,direction_guess_label="LORA"), # determine delays iteratively
            ApplyBaseline=dict(rmsfactor=7)
            )
        #------------------------------------------------------------------------

        #Note, that currently we keep all tasks and their workspaces. This
        #means that not memory ever gets deallocated. That needs to be changed
        #if memory ever becomes an issue. (In the other hand it needs not be
        #reallocated, if eventually one loops over events - but that needs
        #checking).

        ########################################################################
        #Allocating the memory for the fft, if it doesn't exist yet
        ########################################################################
        if globals().has_key("fftplan") or locals().has_key("fftplan"):
            print "#fftplan exists - reusing old one!"
        else:
            fftplan = FFTWPlanManyDftR2c(blocksize, 1, 1, 1, 1, 1, fftw_flags.ESTIMATE)

        if globals().has_key("invfftplan") or locals().has_key("invfftplan"):
            print "#invfftplan exists - reusing old one!"
        else:
            invfftplan = FFTWPlanManyDftC2r(blocksize, 1, 1, 1, 1, 1, fftw_flags.ESTIMATE)

        ########################################################################
        #Calculating the average spectrum and quality flags
        ########################################################################
        print "---> Calculating average spectrum of all antennas"
        avspectrum=trerun("AverageSpectrum","cr_event",pardict=par,load_if_file_exists=True,doplot=0 if Pause.doplot else False)
        calblocksize=avspectrum.power.getHeader("blocksize")
        speclen=avspectrum.power.shape()[-1] # note: this is not blocksize/2+1 ... (the last channel is missing!)

        #Make sure baseline is not left over from previous file, we want to recalculate this here
        if hasattr(avspectrum.power.par,"baseline"):
            del avspectrum.power.par.baseline

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
        # is) has two flagged (sub-)blocks (one flagged (sub-)block is OK,
        # since this is where the peak sits)

        #Select bad antennas
        flaglist=[i for i in quality if i["chunk"]==block_number and i["nblocksflagged"]>1]
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

        #raise KeyboardInterrupt("Forced end of execution!")

        ########################################################################
        #Baseline Fitting 
        ########################################################################

        print "---> Fit a baseline to the average spectrum"
        fitbaseline=trerun("FitBaseline","",averagespectrum_good_antennas,extendfit=0.5,pardict=par,doplot=3 if Pause.doplot else 0)


        #Create new average spectrum with only good antennas, based on quality checking from fitbaseline ...
        if fitbaseline.nbadantennas>0:
            print "# Additional antenna flagging:",
            new_bad_antennas = [good_antennas[i] for i in fitbaseline.badantennas] # i is the position in the old good_antenna list
            bad_antennas.extend(new_bad_antennas)
            print "# Bad Antennas:",fitbaseline.badantennas,"->",new_bad_antennas

            #This is a repeat from above ....
            bad_antennas_index=[antenna_index[i] for i in bad_antennas]
            good_antennas=antennalist[:]
            [good_antennas.remove(i) for i in bad_antennas] # remove the bad antennas
            good_antennas_index=[antenna_index[i] for i in good_antennas] # get the index numbers of good antennas pointing to position in array
            ndipoles=len(good_antennas)

            #Create new average spectrum with only good antennas
            print "# Antenna Flagging:",len(bad_antennas),"bad antennas!"
            print "# Bad Antennas:",bad_antennas
            averagespectrum_good_antennas=hArray(dimensions=[ndipoles,speclen],properties=avspectrum.power)
            averagespectrum_good_antennas[...].copy(avspectrum.power[good_antennas_index,...])

        good_antennas_IDs=hArray(good_antennas_index)*2+polarization

        print "---> Calculate a smooth version of the spectrum which is later used to set amplitudes."
        calcbaseline1=trerun("CalcBaseline",1,averagespectrum_good_antennas,pardict=par,invert=False,HanningUp=False,normalize=False,doplot=0)
        amplitudes=hArray(copy=calcbaseline1.baseline)

        #raise TypeError("I don't like your type .... (just for debugging)")

        print "---> Calculate it again, but now to flatten the spectrum."
        calcbaseline_flat=trerun("CalcBaseline","flat",averagespectrum_good_antennas,pardict=par,invert=True,normalize=False,doplot=Pause.doplot)

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
        #Set parameters of data file
        ########################################################################

        print "---> Load the block with the peak"
        datafile["BLOCKSIZE"]=blocksize #2**16
        datafile["BLOCK"]=block_number #93

        datafile["SELECTED_DIPOLES"]=good_antennas
        antenna_positions=datafile["ANTENNA_POSITIONS"]

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
        calcbaseline_galactic=trerun("CalcBaseline","galactic",averagespectrum_good_antennas,pardict=par,invert=True,normalize=False,powerlaw=0.5,doplot=Pause.doplot)

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
        if sample_number>=0 and search_window_width>0:
            search_window=(sample_number-search_window_width/2,sample_number+search_window_width/2)

        if lora_direction:
            print "---> Now make an incoherent beam in the LORA direction, locate pulse, and cut time series around it."
            beamformer_full=trerun("BeamFormer2","bf_full",data=timeseries_data2,fftdata=fft_data,dofft=False,pardict=par,maxnantennas=ndipoles,antpos=antenna_positions,FarField=True,sample_interval=sample_interval,pointings=rf.makeAZELRDictGrid(lora_direction[0]*deg,lora_direction[1]*deg,1,nx=1,ny=1),cable_delays=cabledelays,calc_timeseries=True,doplot=False,doabs=True,smooth_width=5,plotspec=False,verbose=False,calc_tbeams=False)
            tbeam_incoherent=hArray(beamformer_full.tbeam_incoherent.vec(),[blocksize])#make this a one-dimensional array to not confuse LocatePulseTrain ...
        else:
            print "---> Now add all antennas in the time domain, locate pulse, and cut time series around it."
            tbeam_incoherent=None

        pulse=trerun("LocatePulseTrain","",timeseries_data2,timeseries_data_sum=tbeam_incoherent,pardict=par,doplot=Pause.doplot,search_window=search_window)

        print "#LocatePulse: ",pulse.npeaks,"pulses found."
        if pulse.npeaks==0:
            print "************************************************************************"
            print "********          ATTENTION: No pulses found          ******************"
            print "************************************************************************"
            print "ERROR: LocatePulseTrain: No pulses found!"
            break

        print "---> Get peaks in power of each antenna (Results in maxima_power.maxy/maxx)."
        timeseries_power=hArray(copy=pulse.timeseries_data_cut)
        timeseries_power.square()
        timeseries_power.runningaverage(5,hWEIGHTS.GAUSSIAN)
        maxima_power=trerun('FitMaxima',"Power",timeseries_power,pardict=par,doplot=Pause.doplot,refant=0,plotend=ndipoles,sampleinterval=sample_interval,peak_width=11,splineorder=3)
        Pause(name="pulse-maxima-power")

        timeseries_data_cut_to_pulse=hArray(float,[ndipoles,timeseries_data_cut_pulse_width],properties=pulse.timeseries_data_cut)
        timeseries_data_cut_to_pulse_offsets=hArray(int,ndipoles,fill=(maxima_power.maxx+0.5-timeseries_data_cut_pulse_width/2))
        timeseries_data_cut_to_pulse_delays=hArray(float,copy=timeseries_data_cut_to_pulse_offsets)
        timeseries_data_cut_to_pulse_delays *= sample_interval
        timeseries_data_cut_to_pulse[...].copy(pulse.timeseries_data_cut[..., timeseries_data_cut_to_pulse_offsets:timeseries_data_cut_to_pulse_offsets+timeseries_data_cut_pulse_width])
        

        ########################################################################
        #Now refine this by cross correlating around the peak maxima
        ########################################################################
        print "---> Cross correlate pulses, get time lags, and determine direction of pulse."
        #Now cross correlate all pulses with each other 
        crosscorr=trerun('CrossCorrelateAntennas',"crosscorr",timeseries_data_cut_to_pulse,pardict=par,oversamplefactor=10)
        #And determine the relative offsets between them
        maxima_cc=trerun('FitMaxima',"Lags",crosscorr.crosscorr_data,pardict=par,doplot=Pause.doplot,refant=0,plotend=5,sampleinterval=sample_interval/crosscorr.oversamplefactor,peak_width=11,splineorder=3)
        Pause(name="pulse-maxima-crosscorr")

        #The actual delays are those from the (rounded) peak locations plus the delta form the cross correlation
        time_lags=(timeseries_data_cut_to_pulse_delays.vec()+maxima_cc.lags)
        
        print "Time lag [ns]: ", time_lags 
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

        direction=trerun("DirectionFitTriangles","direction",pardict=par,positions=antenna_positions,timelags=hArray(time_lags),cabledelays=cabledelays,verbose=True,doplot=True)
        print "========================================================================"
        print "Triangle Fit Az/EL -> ", direction.meandirection_azel_deg,"deg"
        print " "
        print "Delays =",direction.total_delays * 1e9

        print "#DirectionFitTriangles: delta delays =",direction.delta_delays_mean_history[0],"+/-",direction.delta_delays_rms_history[0]

        
        (direction.total_delays*1e9).plot(xvalues=good_antennas_IDs)
        (direction.delays_history*1e9)[1].plot(clf=False,xvalues=good_antennas_IDs)
        (cabledelays*1e9).plot(clf=False,xlabel="Antenna",ylabel="Delay [ns]",xvalues=good_antennas_IDs,title="Fitted cable delays",legend=(["total delay","1st step","cable delay"]))
        Pause(name="fitted-cable-delays")

        if abs(direction.delta_delays_rms_history[0])>maximum_allowed_delay or abs(direction.delta_delays_mean_history[0])>maximum_allowed_delay:
            print "************************************************************************"
            print "********ATTENTION: Fitted delays deviate too strongly!******************"
            print "********            check antennna positions          ******************"
            print "************************************************************************"
            print "#DirectionFitTriangles: ERROR!"
        else:
            print "#DirectionFitTriangles: OK!"
            
        print "---> Checking and flagging delays which are larger than +/-"+str(maximum_allowed_delay)
        flagged_residual_delays=hArray(copy=direction.residual_delays)
        flagged_delays=direction.residual_delays.Find('outside',-maximum_allowed_delay,maximum_allowed_delay)
        if len(flagged_delays)>0 and flag_delays:
            flagged_residual_delays.set(flagged_delays,0)
            print "#Delay flagging - flagged these delays:",flagged_delays
        else:
            print "#Delay flagging - all delays seem OK."
        flagged_cable_delays=flagged_residual_delays+direction.cabledelays


        #plotdirection=trerun("PlotDirectionTriangles","plotdirection",pardict=par,centers=direction.centers,positions=direction.positions,directions=direction.directions,title="Sub-Beam directions")
        #Pause(name="sub-beam-directions")

        trerun("PlotAntennaLayout","Delays",pardict=par,positions=antenna_positions,colors=direction.total_delays,sizes=100,names=good_antennas_IDs,title="Delay errors in station",plotlegend=True)

        print "\n--->Beamforming"

        ########################################################################
        #Beamforming
        ########################################################################

        #Beamform short data set first for inspection (and possibly for
        #maximizing later)
        bf=trerun("BeamFormer2","bf",data=pulse.timeseries_data_cut,pardict=par,maxnantennas=ndipoles,antpos=antenna_positions,FarField=True,sample_interval=sample_interval,pointings=rf.makeAZELRDictGrid(*(direction.meandirection_azel+(10000,)),nx=3,ny=3),cable_delays=flagged_cable_delays,calc_timeseries=True,doplot=2 if Pause.doplot else False,doabs=True,smooth_width=5,plotspec=False,verbose=False)

        #Use the above later also for maximizing peak Beam-formed timeseries
        #is in ---> bf.tbeams[bf.mainbeam]

        print "---> Plotting mosaic of beams around central direction"
        if Pause.doplot:
            bf.tplot(smooth=9)
            Pause(name="beamformed-multiple-directions")

        timeseries_power_shifted=hArray(fill=bf.data_shifted,properties=pulse.timeseries_data_cut,name="Power(t)")
        timeseries_power_shifted.square()
        timeseries_power_shifted[...].runningaverage(5,hWEIGHTS.GAUSSIAN)
        if Pause.doplot:
            timeseries_power_shifted[...].plot(title="E-Field^2 corrected for delays in beam direction per antenna",xlabel="Samples")
            plt.legend(good_antennas,ncol=2,title="Antennas",prop=matplotlib.font_manager.FontProperties(family="Helvetica",size=7))
            Pause(name="shifted-power-per-antenna")

        #matplotlib.font_manager.FontProperties(self, family=None, style=None, variant=None, weight=None, stretch=None, size=None, fname=None, _init=None)


        print "---> Plotting full beam-formed data set"
        #Beamform full data set (not really necessary, but fun).
        beamformed=trerun("BeamFormer2","beamformed",data=pulse.timeseries_data,pardict=par,maxnantennas=ndipoles,antpos=antenna_positions,FarField=True,sample_interval=sample_interval,pointings=rf.makeAZELRDictGrid(*(direction.meandirection_azel+(10000,)),nx=1,ny=1),cable_delays=flagged_cable_delays,calc_timeseries=False,doabs=False,smooth_width=0,doplot=False,plotspec=False,verbose=False)

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
        print "# Peak height:",beam_maxima.maxy

        ########################################################################
        #Determinig the pulse strength in the shifted time series
        #at the main peak location and plot
        ########################################################################
        pulses_strength=timeseries_power_shifted[...].elem(min(max(int(round(beam_maxima.maxx.val()-pulse.start)),0),pulse.cutlen-1))
        
        trerun("PlotAntennaLayout","TimeLags",pardict=par,positions=antenna_positions,colors=hArray(time_lags),sizes=pulses_strength,sizes_min=0,names=good_antennas_IDs,title="Time Lags in Station",plotlegend=True)


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
            TIME=tbb_starttime,
            SAMPLE_NUMBER=tbb_samplenumber,
            BLOCK=block_number,
            pulse_predicted=sample_number,
            pulse_offset=sample_number-beam_maxima.maxx.val(),
            BLOCKSIZE=blocksize,
            polarization=current_polarization,
            plotfiles=Pause.plotfiles,
            filedir=filedir,
            ndipoles=ndipoles,
            antennas=good_antennas,
            antenna_positions_XYZ_m=list(antenna_positions.vec()),
            antenna_positions_ITRF_m=list(datafile["ITRFANTENNA_POSITIONS"].vec()),
            flagged_delays=list(flagged_delays.vec()),
            flagged_cable_delays=list(flagged_cable_delays.vec()),
            bad_antennas=bad_antennas,
            pulse_location=beam_maxima.maxx.val(),
            pulses_strength=list(pulses_strength.vec()),
            pulses_maxima_x=list(maxima_power.maxx),
            pulses_maxima_y=list(maxima_power.maxy),
            npeaks_found=pulse.npeaks,
            pulses_timelags_ns=list(time_lags),
            pulse_start_sample=pulse.start,
            pulse_end_sample=pulse.end,
            pulse_time_ms=pulse_time_ms,
            pulse_height=beam_maxima.maxy.val(),
            pulse_direction=direction.meandirection_azel_deg,
            pulse_direction_lora=lora_direction,
            pulse_energy_lora=lora_energy,
            pulse_core_lora=lora_core,
            pulse_direction_delta_delays_start=direction.delta_delays_mean_history[0],
            pulse_direction_delta_delays_final=direction.delta_delays_mean_history[-1]
            )

        event.par.quality=quality
        event.par.timeseries_data=pulse.timeseries_data_cut
        event.par.results=results
        event.write(result_file)

        #Writing results.py
        f=open(os.path.join(outputdir_with_subdirectories,"results.py"),"w")
        f.write("#"+outfilename+"\nresults="+str(results))
        f.close()

        ########################################################################
        #Writing summary output to html file
        ########################################################################
        htmlfile=open(htmlfilename,"w")
        htmlfile.write("<html><head><title>{0:s}</title></head><body>\n".format(outfilename))
        htmlfile.write("<h1>{0:s}</h1>\n".format(outfilename))
        htmlfile.write("<i>File processed on {0:s}, by user {1:s}. Processing time = {2:5.2f}s</i><p>\n".format(
            time.strftime("%A, %Y-%m-%d at %H:%M:%S"),os.getlogin(),time.clock()-t0))


        htmlfile.write("<a href=#{0:s}>{0:s}</a><br>\n".format("Parameters"))
        htmlfile.write("<a href=#{0:s}>{0:s}</a><br>\n".format("Parfiles"))
        htmlfile.write("<a href=#{0:s}>{0:s}</a><br>\n".format("Logger"))
        htmlfile.write("<a href=#{0:s}>{0:s}</a><p>\n".format("Plotfiles"))
        
        htmlfile.write("<h2><a name={0:s}>{0:s}</a></h2>\n".format("Parameters"))

        l=results.items(); l.sort()
        htmlfile.write('<table border="1">\n'.format())
        for k,v in l:
            htmlfile.write("<tr><td><b>{0:s}</b></td><td>{1:s}</td></tr>\n".format(k,str(v)))
        htmlfile.write("</table>\n".format())

        htmlfile.write("<h2><a name={0:s}>{0:s}</a></h2>\n".format("Parfiles"))
        for i in range(nparfiles,len(tasks.task_parfiles)):
            if os.path.exists(tasks.task_parfiles[i]):
                os.rename(tasks.task_parfiles[i],tasks.task_parfiles[i]+".txt")
                htmlfile.write('<a type="text/http" href="{0:s}.txt">{1:s}</a><br>\n'.format(tasks.task_parfiles[i],os.path.split(tasks.task_parfiles[i])[-1]))

        htmlfile.write("<h2><a name={0:s}>{0:s}</a></h2>\n".format("Logger"))
        htmlfile.write("<PRE>\n"+tlog(doprint=False)+"</PRE>\n")

        htmlfile.write("<h2><a name={0:s}>{0:s}</a></h2>\n".format("Plotfiles"))
        for f in results["plotfiles"]:
            htmlfile.write('<a href="{0:s}">{0:s}</a>:<br><a href="{0:s}"><img src="{0:s}" width=500></a><p>\n'.format(os.path.split(f)[-1]))

        htmlfile.write("</body></html>\n")
        htmlfile.close()    

        #print logfile
        tlog()
        
        print "Data and results written to file. Read back with event=hArrayRead('"+result_file+"')"
        print "Basic parameters and results are in the dicts ``results`` or ``event.par.results``,"
        print "which can be found in `results.py`(use, e.g., execfile)."
        print "Shifted time series data of all antennas is in event.par.time_series."
        print "Open",htmlfilename,"in your browser to get a summary."
        print "-----------------------------------------------------------------------------------------------------------"
        print "Finished cr_event after",time.clock()-t0,"seconds."
        print "-----------------------------------------------------------------------------------------------------------\n"
        plt.ion()
    
