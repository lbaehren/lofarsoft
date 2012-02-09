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

*block*                 (or blocknumber) - in which block do you expect the peak

*plotpause*             pause and display each figure interactively?

*refresh*               refresh plotting window after each plot (e.g., during batch processing).

*maximum_allowed_delay* maximum differential mean cable delay that the
                        expected positions can differ rom the measured ones, before we
                        consider something to be wrong
======================= =================================================================

For more help on more parameters run ``cr_event.py --help``.

**Results:**

Data and results are stored in an hArray and its header. This can be
read back with ``event=hArrayRead(filename-polN.results)``, (filename
without the .h5 ending) in the ``outputdir/project-timestamp/polN/NNNN`` directory where all
output for station NNN can be found.

Some basic results are stored in the ``event.par.results`` dict. The
quality information dict is in ``event.par.quality``.  The shifted
time series data (corrected for time travel and cable delay) of all
antennas is in ``event.par.timeseries_data``. A summary of the ouput
with all figures can be viwed with a web browser from
outputdir/project-timestamp/polN/NNNN/index.html.

The results dict can also be read stand-alone from the file
results.py. Just use execfile(os.path.join(outputdir,"results.py")) and look for ``results``.

There is also a summary of the results ``summary.html`` and
``summary-good.html``in the top level results directory.

**Example:**

*Command line use:*
::
    chmod a+x $LOFARSOFT/src/PyCRTools/pipelines/cr_event.py

    $LOFARSOFT/src/PyCRTools/pipelines/cr_event.py ~/LOFAR/work/data/VHECR_LORA-20110716T094509.665Z-002.h5 --lofarmode=LBA_OUTER --outputdir=/Users/falcke/LOFAR/work/results --loradir /Users/falcke/LOFAR/work/data/ --lora_logfile LORAtime4 --search_window_width=4000 --nsigma=3 --max_data_length=12289024 -R -p0

run  $LOFARSOFT/src/PyCRTools/pipelines/cr_event.py ~/LOFAR/work/data/VHECR_LORA-*.h5 --lofarmode=LBA_OUTER --outputdir=/Users/falcke/LOFAR/work/results --loradir /Users/falcke/LOFAR/work/data/ --lora_logfile LORAtime4 --search_window_width=4000 --nsigma=3 --max_data_length=12289024 -R

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
from optparse import OptionParser

#------------------------------------------------------------------------
#Command line options
#------------------------------------------------------------------------
usage = "usage: %prog [options] datafile.h5 "
parser = OptionParser(usage=usage)

parser.add_option("-o","--outputdir", type="str", default="",help="directory where to store the final results (will be stored in a subdirectory filename.dir/ within that directory)")
parser.add_option("-L","--lora_logfile", type="str", default="LORAtime4",help="LORA logfile (LORAtime4)")
parser.add_option("--loradir", type="str", default="/data/VHECR/LORAtriggered/LORA/",help="Directory to find LORA information")
parser.add_option("-d","--datadir", type="str", default="",help="Will be added to each of the filename arguments (hence, use quotations for the main arguments if you use this option and have asterisks in the argument!)")
parser.add_option("-l","--lofarmode", type="str", default="LBA_OUTER",help="'LBA_INNER' or 'LBA_OUTER'")
parser.add_option("-p","--polarization", type="int", default=-1,help="either 0 or 1 for selecting even or odd antennas, or -1 for both")
parser.add_option("-t","--nsigma", type="float", default=3.0,help="Threshold for identifying peaks")
parser.add_option("-b","--block", type="int", default=0,help="in which block do you expect the peak  (use LORA guess if -1 or central block if no guess is present)")
parser.add_option("-w","--search_window_width", type="int", default=2048,help="Width of search window for peak around sample_number (in samples) - full block if not given")
parser.add_option("-n","--sample_number", type="int", default=65536 + 512,help="Sample number within block where peak is expected (use LORA guess if -1 or center if no guess is present)")
parser.add_option("-s","--blocksize", type="int", default=2**17,help="Blocksize with which to read blocks of data.")
parser.add_option("--max_data_length", type="int", default=-1,help="Maximum length the file is allowed to have (avoid hangups for corrupted files)")
parser.add_option("--min_data_length", type="int", default=-1,help="Minimum length the file should have (sanity check)")
parser.add_option("-T","--timestamp", type="str", default="",help="Timestamp of observation for creating output directory (if not to be deduced from filename)")
parser.add_option("-S","--station", type="str", default="",help="Station name for data set for creating output directory (if not to be deduced from filename)")
parser.add_option("-q","--nopause", action="store_true",help="Do not pause after each plot and display each figure interactively")
parser.add_option("-k","--skip_existing_files", action="store_true",help="Skip file if results directory already exists")
parser.add_option("-R","--norefresh", action="store_true",help="Do not refresh plotting window after each plot, don't stop, no plotting window in command line mode (use for batch operation).")
parser.add_option("-D","--maximum_allowed_delay", type="float", default=1e-8,help="maximum differential mean cable delay that the expected positions can differ rom the measured ones, before we consider something to be wrong")
parser.add_option("-C","--checksum", action="store_true", help="Calculate checksums used for debugging; default OFF")
parser.add_option("-O", "--max_outliers", type="int", default=5, help="Maximum allowed number of outliers in calculated cable delays")
parser.add_option("-Y","--cabledelays", action="store_true", help="Apply metadata cable delays if present. Default OFF")
#parser.add_option("-F", "--filelist", type="string", help="Process the files listed in a list file with this name")
(options, args) = parser.parse_args()
if options.norefresh:
    nogui=True
    import matplotlib
    matplotlib.use('Agg')


from pycrtools import *
from pycrtools import lora
from pycrtools import xmldict
import pickle

def finish_file(filename, status):
    # write to output file, to 'good' list if status= 'OK', to 'bad' list if not, with reason
    goodlist = 'goodfiles.txt'
    badlist = 'badfiles.txt'
    
    goodlistfile = os.path.join(outputdir, goodlist)
    badlistfile = os.path.join(outputdir, badlist)
    
    if status == 'OK':
        #if not os.path.exists(goodlistfile):
        #    f = open(goodlistfile, "w")
        #else:
        f = open(goodlistfile, "a")
        time_of_file = float(tbb_starttime) + float(tbb_samplenumber) / 200.0e6
        f.write(filename + ' ' + str(time_of_file) + ' ' + str(directionPlaneWave.meandirection_azel_deg[0]) + ' ' + str(directionPlaneWave.meandirection_azel_deg[1]) + ' ' + str(delay_quality_error) + ' ' + str(delay_outliers) + '\n')
    else:
        #if not os.path.exists(badlistfile):
        #    f = open(badlistfile, "w")
        #else:
        f = open(badlistfile, "a")
        f.write(filename + ' ' + status + '\n')
    f.close()
    
#plt.figure(num=1, figsize=(8*2, 6*2), dpi=300, facecolor='w', edgecolor='k')

#------------------------------------------------------------------------
# Main input parameters
#------------------------------------------------------------------------
flag_delays = False
minimum_number_good_antennas=8
timeseries_data_cut_pulse_width=12
min_number_of_antennas_with_pulses=8
start_time=time.strftime("%A, %Y-%m-%d at %H:%M:%S")

#    search_window_width=1024
#    sample_number=65536 + 512 # LOFAR radio-triggered specific! NB gets overridden by options.sample_number etc...

#    maximum_allowed_delay=1e-8 # maximum differential mean cable delay
                               # that the expected positions can differ
                               # from the measured ones, before we
                               # consisder something to be wrong
#else:
#   Program was started from command line

# Filenames
if not args:
    print "Error: No HDF5 files specified for input"
    sys.exit(1)

datadir = options.datadir
filefilter = [os.path.join(datadir,f) for f in args]
lofarmode = options.lofarmode
lora_logfile=options.lora_logfile
loradir=options.loradir
outputdir = options.outputdir
polarization = options.polarization
timestamp = options.timestamp
station = options.station
blocknumber = options.block
nsigma = options.nsigma
pulses_sigma = options.nsigma
samplenumber = options.sample_number
blocksize = options.blocksize
skip_existing_files = options.skip_existing_files
refresh = not options.norefresh
plotpause = (not options.nopause and refresh)
maximum_allowed_delay = options.maximum_allowed_delay
max_outliers = options.max_outliers
search_window_width=options.search_window_width
sample_number=options.sample_number
max_data_length=options.max_data_length
min_data_length=options.min_data_length
maxnchunks=max_data_length/blocksize
do_checksums = options.checksum
#The Pause instance will pause (or not) after each plot and write the plotfiles
Pause=plotfinish(plotpause=plotpause,refresh=refresh)
do_cabledelays = options.cabledelays

plt.ioff()
#------------------------------------------------------------------------

good_old_time_stamp = "None"


########################################################################
#Loop over all files and polarizations
########################################################################

polarizations=[0] 
files = listFiles(filefilter)
if not files:
    print "ERROR: No files found - ",filefilter
else:
    print "Processing the following files:",files

for full_filename in files:
    for current_polarization in polarizations:
        t0=time.clock()
        file_time=time.strftime("%A, %Y-%m-%d at %H:%M:%S")
        file_time_short=time.strftime("%Y-%m-%d  %H:%M:%S")
        #------------------------------------------------------------------------
        # Initialization of some parameters
        #------------------------------------------------------------------------
        delay_quality_error=99.
        delay_outliers = 99
        pulse_normalized_height=-1.0
        pulse_height=-1.0
        pulse_direction=(-99.,-99.)
        pulse_npeaks=-1
        tasks.task_logger=[]
        statuslist=[]
        block_number=blocknumber
        sample_number=samplenumber

        checksums = []
        results = {}
        search_window = True
        ########################################################################
        #Open the data file
        ########################################################################
        filesize = os.path.getsize(full_filename)
        print "---> Check data file size: %d" % filesize 
        if filesize < 24 * 1048576 or filesize > 29 * 1048576:
            print "File size wrong, file marked as BAD!"
            status = "File too small" if filesize < 24 * 1048576 else "File too large"
            finish_file(full_filename, status)
            continue
            
        print "---> Open data file",full_filename
        try:
            datafile=open(full_filename);
            if datafile["ANTENNA_SET"]=="UNDEFINED":
                datafile["ANTENNA_SET"]="LBA_OUTER"
        except RuntimeError as e:
            print "ERROR opening file - skipping this file!"
            status = "OPEN FAILED "+str(e)
            finish_file(full_filename, status)
            continue
        

        (filedir,filename)=os.path.split(full_filename)
        outputdir_expanded=os.path.expandvars(os.path.expanduser(outputdir))
        (rootfilename,fileextensions)=os.path.splitext(filename)
#        topdir_name=projectname+"-"+time_stamp
        topdir_name = rootfilename

        #Create output dir if needed
        if not os.path.exists(outputdir_expanded):
            print "# Creating output directory", outputdir_expanded
            os.makedirs(outputdir_expanded)

        ########################################################################
        #Continued opening the data file
        ########################################################################

        tbb_starttime=datafile["TIME"][0]
        tbb_samplenumber=max(datafile["SAMPLE_NUMBER"])
        sample_interval=datafile["SAMPLE_INTERVAL"][0]
        data_length=datafile["DATA_LENGTH"][0]

        block_number = 0 # Radio-triggered specific!
        if "HBA" in datafile["ANTENNA_SET"]:
            finish_file(full_filename, "HBA skipped")
            continue
             
#        if max_data_length>0 and max(datafile["DATA_LENGTH"])>max_data_length:
#            print "ERROR: Data file size is too large (",max(datafile["DATA_LENGTH"]),") - skipping this file!"
#            statuslist.append("DATA_LENGTH BAD")
#            finish_file(status="FILE TOO LARGE")
#            continue

 #       if min(datafile["DATA_LENGTH"])<min_data_length or min(datafile["DATA_LENGTH"]) < blocksize:
 #           print "ERROR: Data file size is too small (",max(datafile["DATA_LENGTH"]),") - skipping this file!"
 #           finish_file(laststatus="FILE TOO SMALL")
 #           continue

#        if blocknumber<0:
#       Only use sample_number to indicate a 'good' pulse position
#        if samplenumber<0:
#            sample_number=blocksize/2
        

        ########################################################################
        #Getting information from LORA if present
        ########################################################################
        ########################################################################
        #Setting the parameter block with parameters for tasks
        ########################################################################

        par=dict(
            plot_antenna=0,
            newfigure=False, #Don't create a new plotting window for some of the tasks, but use the old one.
            plot_finish=Pause,
            LocatePulseTrain = dict(nsigma=nsigma,maxgap=5,minlen=128,minpulselen=3,maxlen=128,prepulselen=32),  #nisgma=7
        #    DirectionFitTriangles = dict(maxiter=2,rmsfactor=0,minrmsfactor=0), # only do one step,all atennas at once
 #           DirectionFitTriangles = dict(maxiter=6,rmsfactor=2,minrmsfactor=0,direction_guess=lora_direction,direction_guess_label="LORA"), # determine delays iteratively
            DirectionFitTriangles = dict(maxiter=6,rmsfactor=2,minrmsfactor=0), # determine delays iteratively
#            DirectionFitTriangles = dict(maxiter=2,rmsfactor=0,minrmsfactor=0,direction_guess=lora_direction,direction_guess_label="LORA"), # determine delays iteratively
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

        antennalist=datafile["CHANNEL_ID"] # just the names of all antennas
        str_antennalist = [format('%09d' % int(id)) for id in antennalist]
        
        indexlist = range(0, len(antennalist), 2)
        
        
        #Select antennas to be used, impose polarisation
        good_antennas = [id for id in str_antennalist if int(id) % 2 == current_polarization]
        ndipoles = len(good_antennas)
        
        print "---> Load the block with the peak", block_number, blocksize
        datafile["BLOCKSIZE"]=blocksize #2**16
        datafile["BLOCK"]=block_number #93

        datafile["SELECTED_DIPOLES"]=good_antennas
        antenna_positions=datafile["ANTENNA_POSITIONS"]
        
        print 'selected dipoles'
        print datafile["SELECTED_DIPOLES"]

        timeseries_data=datafile["EMPTY_TIMESERIES_DATA"]
        try:
            timeseries_data.read(datafile,"TIMESERIES_DATA")
        except RuntimeError:
            print "Error reading file - skipping this file"
            finish_file(full_filename, "READ ERROR")
            continue

        if do_checksums:
            checksum = timeseries_data.checksum()
            checksums.append('Checksum after reading in timeseries data: ' + checksum)
            print checksums[-1]

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

        # subtract DC offset
        timeseries_data[...] -= timeseries_data[...].mean()

        #### 
        # Apply metadata cable delays if present, and if desired
        ####
        if do_cabledelays:
            
            #FFT only when used for cable delay stuffs
            hFFTWExecutePlan(fft_data[...], timeseries_data[...], fftplan)
            fft_data[...,0]=0 # take out zero (DC) offset (-> offset/mean==0)

            if do_checksums:
                checksum = fft_data.checksum()
                checksums.append('Checksum after fft_data: ' + checksum)
                print checksums[-1]
            
            cabledelays = hArray(dimensions=datafile["NOF_SELECTED_DATASETS"], Type=float, fill=0)
            try:
                # apply cable delays 
                cabledelays = hArray(datafile["DIPOLE_CALIBRATION_DELAY"])
                weights = hArray(complex,dimensions = fft_data,name="Complex Weights")
                freqs = hArray(datafile["FREQUENCY_DATA"]) # a FloatVec comes out, so put it into hArray
                phases = hArray(float,dimensions=fft_data,name="Phases",xvalues=datafile["FREQUENCY_DATA"])

                hDelayToPhase(phases, freqs, cabledelays)
                hPhaseToComplex(weights, phases)
                
                fft_data.mul(weights)

                # back to time domain to get calibrated timeseries data
                hFFTWExecutePlan(timeseries_data[...], fft_data[...], invfftplan)
                timeseries_data /= blocksize
                print 'Cable delays applied'
                results.update(dict(
                    DIPOLE_CALIBRATION_DELAY_APPLIED=True
                    ))
                
            except IOError:
                print 'NO cable delays found in file'
                results.update(dict(
                    DIPOLE_CALIBRATION_DELAY_APPLIED=False
                    ))
        else:
            print 'No cable delay calibration used (add --cabledelays if needed)'
            
        ########################################################################
        #Back to time domain
        ########################################################################
        timeseries_calibrated_data=hArray(properties=timeseries_data)
        timeseries_calibrated_data.copy(timeseries_data)
        #fft_data[...,0]=0 # take out zero offset (-> offset/mean==0)
#        hFFTWExecutePlan(timeseries_calibrated_data[...], fft_data[...], invfftplan)

        timeseries_calibrated_data /= blocksize # normalize back to original value

        timeseries_calibrated_data_rms=timeseries_calibrated_data.stddev(0.0).val()
        results.update(dict(
            pulse_height_rms=timeseries_calibrated_data_rms
            ))

        if do_checksums:
            checksum = timeseries_calibrated_data.checksum()
            checksums.append('Checksum after getting timeseries_calibrated_data: ' + checksum)
            print checksums[-1]

        # Note: to finish calibration, we have to know the pulse location first
        # Then we divide out by the rms per antenna in this block, while excluding the pulse region.
        # so we use [0:pulse.start] ( and [pulse.end:] to be implemented).

        ########################################################################
        #Locate pulse and cut data around it
        ########################################################################
        #First determine where the pulse is in a simple incoherent sum of all time series data
#        if (search_window_width > 0) and (sample_number > 0):
#            # only narrow the search window if we have a 'good' guess for sample_number...
#            search_window=(max(0, sample_number-search_window_width/2), min(timeseries_calibrated_data.shape()[1] - 1, sample_number+search_window_width/2))
#        else:
#        search_window=
        y = timeseries_calibrated_data.toNumpy()
        plt.plot(y[0])
        plt.plot(y[1])
        plt.plot(y[23])
        print "---> Now add all antennas in the time domain, locate pulse, and cut time series around it."
        tbeam_incoherent=None

        search_window=(max(0, sample_number-search_window_width/2), min(timeseries_calibrated_data.shape()[1] - 1, sample_number+search_window_width/2))
        print search_window
        #If we had a pre-determined direction use this to find the dominant peak and cut out the time series around it
        # Otherwise take the previously found window with the most pulses
        print "searching per antenna"
        pulse=trerun("LocatePulseTrain","separate",timeseries_calibrated_data,pardict=par,doplot=Pause.doplot,search_window=search_window,search_per_antenna=True) # want window here, it's welldefined radio triggered data!
        pulse_npeaks=pulse.npeaks

        #How many antennas have pulses? Also: determine a finer window where the pulses are
        if pulse.npeaks>0:
            antennas_with_peaks=list(asvec(hArray(good_antennas)[pulse.peaks_found_list]))
            print "#LocatePulses - ",pulse.npeaks,"antennas with a pulse found. Pulse window start=",pulse.start,"+/-",pulse.start_rms,"pulse range:",pulse.start_min,"-",pulse.start_max
            print "#LocatePulses - antennas with pulse:",antennas_with_peaks

        print "#LocatePulse: ",pulse_npeaks,"pulses found."

        if pulse_npeaks==0:
            print "************************************************************************"
            print "********          ATTENTION: No pulses found          ******************"
            print "************************************************************************"
            print "ERROR: LocatePulseTrain: No pulses found!"
            finish_file(full_filename, "NO PULSE")
            continue

        if Pause.doplot: timeseries_calibrated_data[0:min(2,ndipoles),...].plot(title="Calibrated time series of first 2 antennas")
        Pause("Plotted time series data. ",name="calibrated-imeseries")

        #a = timeseries_calibrated_data.toNumpy()
        #plt.plot(a[:][pulse.start:pulse.end])

        print 'PULSE START = %d ' % pulse.start

        ########################################################################
        #Determine antennas that have pulses with a high enough SNR for beamforming
        ########################################################################

        antennas_with_strong_pulses=pulse.peaks_found_list
        nantennas_with_strong_pulses=len(antennas_with_strong_pulses)

        if nantennas_with_strong_pulses<min_number_of_antennas_with_pulses:
            print "ERROR: LocatePulseTrain: Not enough pulses found for beam forming!"
            finish_file(full_filename, "TOO FEW PULSES")
            continue

        # NB. Used to cut out pulses in a small window for each antenna... Slight complication but faster, maybe
        print "---> Get peaks in power of each antenna (Results in maxima_power.maxy/maxx)."
        timeseries_power=hArray(copy=pulse.timeseries_data_cut)
        timeseries_power.square()
#        import pdb; pdb.set_trace()
        timeseries_power[...].runningaverage(7,hWEIGHTS.GAUSSIAN) # NB. [...] put in, want to ensure average per antenna... (AC)
#        timeseries_power *= 10 # parameter...
        maxima_power=trerun('FitMaxima',"Power",timeseries_power,pardict=par,doplot=Pause.doplot,refant=0,plotend=ndipoles,sampleinterval=sample_interval,peak_width=11,splineorder=3)
        timeseries_power_mean=timeseries_power[...,0:pulse.start].mean()
        timeseries_power_rms=timeseries_power[...,0:pulse.start].stddev(timeseries_power_mean)
        Pause(name="pulse-maxima-power")

        pulses_snr=maxima_power.maxy/timeseries_power_rms
        pulses_refant=pulses_snr.maxpos()


        ########################################################################
        #Now refine pulse location by cross correlating around the peak maxima
        ########################################################################
        timeseries_data_cut_to_pulse=hArray(float,[ndipoles,timeseries_data_cut_pulse_width],properties=pulse.timeseries_data_cut)
        timeseries_data_cut_to_pulse_offsets=hArray(int,ndipoles,fill=(maxima_power.maxx+0.5-timeseries_data_cut_pulse_width/2))
        timeseries_data_cut_to_pulse_delays=hArray(float,copy=timeseries_data_cut_to_pulse_offsets)
        timeseries_data_cut_to_pulse_delays *= sample_interval
        timeseries_data_cut_to_pulse[...].copy(pulse.timeseries_data_cut[..., timeseries_data_cut_to_pulse_offsets:timeseries_data_cut_to_pulse_offsets+timeseries_data_cut_pulse_width])

        print "---> Cross correlate pulses, get time lags, and determine direction of pulse."

        #Now cross correlate all pulses with each other
        crosscorr=trerun('CrossCorrelateAntennas',"crosscorr",timeseries_data_cut_to_pulse,pardict=par,oversamplefactor=10)

        #And determine the relative offsets between them
        maxima_cc=trerun('FitMaxima',"Lags",crosscorr.crosscorr_data,pardict=par,doplot=Pause.doplot,plotend=5,sampleinterval=sample_interval/crosscorr.oversamplefactor,peak_width=11,splineorder=3,refant=pulses_refant)

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
#        delays=hArray(copy=cabledelays)
        delays = hArray(dimensions=datafile["NOF_SELECTED_DATASETS"], fill=0.0) # no cable delay propagation anymore! Has been applied.
        #cabledelays *= 2
        #delays.fill(0)

        #Only take those lags of pulses with a high-enough SNR
        good_pulse_lags=hArray(time_lags)[antennas_with_strong_pulses]
        good_pulse_antenna_positions=hArray(float,[nantennas_with_strong_pulses,3])
        good_pulse_antenna_positions[...].copy(antenna_positions[antennas_with_strong_pulses,...,0:3])
        good_pulse_cabledelays=delays[antennas_with_strong_pulses] # i.e. put in zeros (!)

        # testing direct plane-wave direction fit
        directionPlaneWave = trerun("DirectionFitPlaneWave", "directionPlaneWave", pardict=par, positions=good_pulse_antenna_positions, timelags = good_pulse_lags, verbose=True, doplot=True)
        
        print "========================================================================"
        print "Plane Wave Fit Az/EL -> ", directionPlaneWave.meandirection_azel_deg,"deg"
        print " "
        print "Delays =",directionPlaneWave.total_delays * 1e9
        print "#DirectionFitPlaneWave: delta delays =",directionPlaneWave.delta_delays_mean_history[0],"+/-",directionPlaneWave.delta_delays_rms_history[0]



#        (direction.total_delays*1e9).plot(xvalues=good_antennas_IDs[antennas_with_strong_pulses],linestyle="None",marker="x")
#        (direction.delays_history*1e9)[1].plot(clf=False,xvalues=good_antennas_IDs[antennas_with_strong_pulses],linestyle="None",marker="o")
#        (cabledelays*1e9).plot(clf=False,xlabel="Antenna",ylabel="Delay [ns]",xvalues=good_antennas_IDs,title="Fitted cable delays",legend=(["fitted delay","1st step","cable delay"]))
            # NB! What does this return? Really fitted cable delays?
 #       Pause(name="fitted-cable-delays")

        scrt = hArray(copy=directionPlaneWave.residual_delays) # NB! Need to copy the hArray
        # otherwise, the original array will get modified by scrt.abs().
        scrt.abs()
        delay_quality_error=scrt.median()[0]/maximum_allowed_delay
        print "#Delay Quality Error:",delay_quality_error

        # also count # outliers, impose maximum
        number = hArray(int, dimensions=scrt)
        delay_outliers = number.findgreaterthanabs(scrt, maximum_allowed_delay)[0] # returns count, indices in 'number' array
        
        print "# delay outliers: ", delay_outliers

        time_lags = hArray(time_lags)
#        time_lags -= cabledelays

        #plotdirection=trerun("PlotDirectionTriangles","plotdirection",pardict=par,centers=direction.centers,positions=direction.positions,directions=direction.directions,title="Sub-Beam directions")
        #Pause(name="sub-beam-directions")
        elevation = directionPlaneWave.meandirection_azel_deg[1]

        if delay_quality_error >= 1:
            final_status = "BAD"
        elif delay_outliers > max_outliers:
            final_status = "BAD / TOO MANY OUTLIERS (" + str(delay_outliers) + ")"
        elif elevation < 30.0:
            final_status = "Elevation too low: " + str(elevation)
        else:
            final_status = "OK"
        
        finish_file(full_filename, final_status)

plt.ion()
