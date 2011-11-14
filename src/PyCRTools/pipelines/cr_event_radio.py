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
parser.add_option("-t","--nsigma", type="float", default=4.0,help="Threshold for identifying peaks")
parser.add_option("-b","--block", type="int", default=-1,help="in which block do you expect the peak  (use LORA guess if -1 or central block if no guess is present)")
parser.add_option("-w","--search_window_width", type="int", default=-1,help="Width of search window for peak around sample_number (in samples) - full block if not given")
parser.add_option("-n","--sample_number", type="int", default=-1,help="Sample number within block where peak is expected (use LORA guess if -1 or center if no guess is present)")
parser.add_option("-s","--blocksize", type="int", default=2**16,help="Blocksize with which to read blocks of data.")
parser.add_option("--max_data_length", type="int", default=-1,help="Maximum length the file is allowed to have (avoid hangups for corrupted files)")
parser.add_option("--min_data_length", type="int", default=-1,help="Minimum length the file should have (sanity check)")
parser.add_option("-T","--timestamp", type="str", default="",help="Timestamp of observation for creating output directory (if not to be deduced from filename)")
parser.add_option("-S","--station", type="str", default="",help="Station name for data set for creating output directory (if not to be deduced from filename)")
parser.add_option("-q","--nopause", action="store_true",help="Do not pause after each plot and display each figure interactively")
parser.add_option("-k","--skip_existing_files", action="store_true",help="Skip file if results directory already exists")
parser.add_option("-R","--norefresh", action="store_true",help="Do not refresh plotting window after each plot, don't stop, no plotting window in command line mode (use for batch operation).")
parser.add_option("-D","--maximum_allowed_delay", type="float", default=1e-8,help="maximum differential mean cable delay that the expected positions can differ rom the measured ones, before we consider something to be wrong")
parser.add_option("-O", "--max_outliers", type="int", default=5, help="Maximum allowed number of outliers in calculated cable delays")

if parser.get_prog_name()=="cr_event_radio.py":
    (options, args) = parser.parse_args()
    if options.norefresh:
        nogui=True
        import matplotlib
        matplotlib.use('Agg')


from pycrtools import *
from pycrtools import lora
from pycrtools import xmldict
import pickle


#plt.figure(num=1, figsize=(8*2, 6*2), dpi=300, facecolor='w', edgecolor='k')

#------------------------------------------------------------------------
# Main input parameters
#------------------------------------------------------------------------
flag_delays = False
minimum_number_good_antennas=8
timeseries_data_cut_pulse_width=12
min_number_of_antennas_with_pulses=8
start_time=time.strftime("%A, %Y-%m-%d at %H:%M:%S")

if not parser.get_prog_name()=="cr_event_radio.py":
#   Program was run from within python
    filefilter="~/LOFAR/work/CR/lora-event-1-station-2.h5"; lofarmode="LBA_OUTER"
    filefilter="~/LOFAR/work/CR/LORAweekdump--2-15.h5"; lofarmode="LBA_INNER"
    filefilter="~/LOFAR/work/data/VHECR_LORA-20110716T094509.665Z-002.h5"; lofarmode="LBA_OUTER"


    outputdir="/Users/falcke/LOFAR/work/results"
    lora_logfile="/Users/falcke/LOFAR/work/data/LORAtime4"
    polarization=1 # either 1 or 0 for odd or even antennapolarization=1 # either 1 or 0 for odd or even antenna
    blocknumber=-1
    blocksize=2**16
    nsigma=4
    timestamp=""
    station=""
    plotpause=False
    refresh=True
    search_window_width=1024
    sample_number=65536 + 512 # LOFAR radio-triggered specific! NB gets overridden by options.sample_number etc...

    maximum_allowed_delay=1e-8 # maximum differential mean cable delay
                               # that the expected positions can differ
                               # from the measured ones, before we
                               # consisder something to be wrong
else:
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

#The Pause instance will pause (or not) after each plot and write the plotfiles
Pause=plotfinish(plotpause=plotpause,refresh=refresh)

plt.ioff()
#------------------------------------------------------------------------

good_old_time_stamp = "None"

def finish_file(laststatus=""):
    ########################################################################
    #Writing summary output to html file
    ########################################################################

    if laststatus: statuslist.append(laststatus)
    status="/".join(statuslist)

    results["status"]=status

    if not os.path.exists(summaryfilename):
        summaryfile=open(summaryfilename,"w")
        summaryfile.write("<html><head><title>{0:s}</title></head><body>\n".format(topdir_name))
        summaryfile.write("<h1>CR Pipeline Summary for {0:s}</h1>\n".format(topdir_name))
        summaryfile.write("<i>File created on {0:s} by user {1:s}.</i><br>\n".format(file_time,os.getlogin()))
        summaryfile.write("Directories: <a href=pol0>pol0</a>, <a href=pol1>pol1</a><p>\n")
        summaryfile.close()

    if not os.path.exists(goodsummaryfilename):
        topsummaryfile=open(goodsummaryfilename,"w")
        topsummaryfile.write("<html><head><title>{0:s}</title></head><body>\n".format("CR Pipeline Summary FILE of good events"))
        topsummaryfile.write("<h1>{0:s}</h1>\n".format("CR Pipeline Summary File of Good Events"))
        topsummaryfile.write("<i>File created on {0:s} by user {1:s}.</i><p>\n".format(file_time,os.getlogin()))
        topsummaryfile.close()

    if not os.path.exists(allsummaryfilename):
        topsummaryfile=open(allsummaryfilename,"w")
        topsummaryfile.write("<html><head><title>{0:s}</title></head><body>\n".format("CR Pipeline Summary FILE of all events"))
        topsummaryfile.write("<h1>{0:s}</h1>\n".format("CR Pipeline Summary File of All Events"))
        topsummaryfile.write("<i>File created on {0:s} by user {1:s}.</i><p>\n".format(file_time,os.getlogin()))
        topsummaryfile.close()

    summaryfile=open(summaryfilename,"a")
    if not old_time_stamp == time_stamp: summaryfile.write("<h3>"+pretty_time_stamp+"</h3>\n")
    summaryfile.write('<a name={0:s} href="{1:s}">{0:s}</a> ({2:s} - {3:s}): <b>Error={4:6.2}</b>, npeaks={8:d}, azel=[{az:.1f},{el:.1f}], height={7:6.2f}, Energy={5:10.2g} eV, norm. pulse={6:6.2f}, <b>{status:s}</b><br>\n'.format(outfilename,os.path.join(reldir_from_event,"index.html"),file_time_short,os.getlogin(),delay_quality_error,lora_energy,pulse_normalized_height,pulse_height,pulse_npeaks,az=pulse_direction[0],el=pulse_direction[1],status=status))
    summaryfile.close()

    global good_old_time_stamp
    if delay_quality_error<1 and delay_outliers < max_outliers: 
        topsummaryfile=open(goodsummaryfilename,"a")
        if not good_old_time_stamp == time_stamp:
            topsummaryfile.write("<h3>"+pretty_time_stamp+"</h3>\n")
            good_old_time_stamp = time_stamp
        topsummaryfile.write('<a name={0:s} href="{1:s}">{0:s}</a> ({2:s} - {3:s}): <b>Error={4:6.2}</b>, npeaks={8:d}, azel=[{az:5.1f}, {el:4.1f}], height={7:6.2f}, Energy={5:10.2g} eV, norm. pulse={6:6.2f}, <b>{status:s}</b><br>\n'.format(outfilename,os.path.join(reldir_from_top,"index.html"),file_time_short,os.getlogin(),delay_quality_error,lora_energy,pulse_normalized_height,pulse_height,pulse_npeaks,az=pulse_direction[0],el=pulse_direction[1],status=status))
        topsummaryfile.close()

    topsummaryfile=open(allsummaryfilename,"a")
    if not old_time_stamp == time_stamp: topsummaryfile.write("<h3>"+pretty_time_stamp+"</h3>\n")
    topsummaryfile.write('<a name={0:s} href="{1:s}">{0:s}</a> ({2:s} - {3:s}): <b>Error={4:6.2}</b>, npeaks={8:d}, azel=[{az:5.1f}, {el:4.1f}], height={7:6.2f}, Energy={5:10.2g} eV, norm. pulse={6:6.2f}, <b>{status:s}</b><br>\n'.format(outfilename,os.path.join(reldir_from_top,"index.html"),file_time_short,os.getlogin(),delay_quality_error,lora_energy,pulse_normalized_height,pulse_height,pulse_npeaks,az=pulse_direction[0],el=pulse_direction[1],status=status))
    topsummaryfile.close()

    htmlfile=open(htmlfilename,"w")
    htmlfile.write("<html><head><title>{0:s}</title></head><body>\n".format(outfilename))
    htmlfile.write("<h1>{0:s}</h1>\n".format(outfilename))
    htmlfile.write("<b><i>"+pretty_time_stamp+": </i></b>")
    htmlfile.write("<i>processed on {0:s} by user {1:s}: processing time={2:5.2f}s.<br>Directories: <a href=../../..>Project</a>, <a href=../..>{3:s}</a>, <a href=..>Stations</a></i>.<p>\n".format(file_time,os.getlogin(),time.clock()-t0,topdir_name))
    htmlfile.write('<b>Error={error:6.2}</b>, npeaks={npeaks:d}, azel=[{az:5.1f}, {el:4.1f}], height={height:6.2f}, Energy={energy:10.2g} eV, norm. pulse={norm:6.2f}, <b>{status:s}</b><p>'.format(error=delay_quality_error,energy=lora_energy,norm=pulse_normalized_height,height=pulse_height,npeaks=pulse_npeaks,az=pulse_direction[0],el=pulse_direction[1],status=status))

    htmlfile.write('<h2><a name="{1:s}">{0:s}</a></h2>\n'.format("Table of Contents","top"))
    htmlfile.write("<a href=#{0:s}>{0:s}</a><br>\n".format("Parameters"))
    htmlfile.write("<a href=#{0:s}>{0:s}</a><br>\n".format("Parfiles"))
    htmlfile.write("<a href=#{0:s}>{0:s}</a><br>\n".format("Logger"))
    htmlfile.write("<a href=#{0:s}>{0:s}</a><p>\n".format("Plotfiles"))
    htmlfile.write(", ".join(['<a href=#{0:s}>{1:s}</a>'.format(os.path.split(f)[-1],os.path.split(f)[-1].split("pol"+str(current_polarization)+"-")[-1][:-4]) for f in results["plotfiles"]]))


    htmlfile.write("<h2><a name={0:s}>{0:s}</a></h2>\n".format("Parameters"))

    l=results.items(); l.sort()
    htmlfile.write(", ".join(["<a href=#{0:s}>{0:s}</a>".format(k) for k,v in l])+"<p>\n")
    htmlfile.write('<table border="1">\n'.format())
    for k,v in l:
        htmlfile.write("<tr><td><b><a name={0:s}>{0:s}</a></b></td><td>{1:s}</td></tr>\n".format(k,str(v)))
    htmlfile.write("</table>\n".format())

    htmlfile.write("<h2><a name={0:s}>{0:s}</a></h2>\n".format("Parfiles"))
    for i in range(nparfiles,len(tasks.task_parfiles)):
        if os.path.exists(tasks.task_parfiles[i]):
            os.rename(tasks.task_parfiles[i],tasks.task_parfiles[i]+".txt")
            htmlfile.write('<a type="text/http" href="{0:s}.txt">{1:s}</a><br>\n'.format(tasks.task_parfiles[i],os.path.split(tasks.task_parfiles[i])[-1]))

    htmlfile.write("<h2><a name={0:s}>{0:s}</a></h2>\n".format("Logger"))
    logstring=tlog(doprint=False)
    if logstring: htmlfile.write("<PRE>\n"+logstring+"</PRE>\n")

    htmlfile.write("<h2><a name={0:s}>{0:s}</a></h2>\n".format("Plotfiles"))
    for f in results["plotfiles"]:
        htmlfile.write('<a name="{0:s}" href="{0:s}">{0:s}</a>:<br><a href="{0:s}"><img src="{0:s}" width=500></a><a href=#top>(back to top)</a><p>\n'.format(os.path.split(f)[-1]))

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
    print "Finished cr_event after",time.clock()-t0,"seconds at",time.strftime("%A, %Y-%m-%d at %H:%M:%S")
    print "-----------------------------------------------------------------------------------------------------------\n"


########################################################################
#Loop over all files and polarizations
########################################################################

old_time_stamp = None
time_stamp=""
polarizations=[0,1] if polarization<0 else [polarization]
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
        lora_direction=False; lora_energy=-1.0; lora_core=(0.0,0.0)
        pulse_normalized_height=-1.0
        pulse_height=-1.0
        pulse_direction=(-99.,-99.)
        pulse_npeaks=-1
        tasks.task_logger=[]
        statuslist=[]
        block_number=blocknumber
        sample_number=samplenumber


        ########################################################################
        #Open the data file
        ########################################################################
        print "---> Open data file",full_filename
        try:
            datafile=open(full_filename); 
            if datafile["ANTENNA_SET"]=="UNDEFINED": 
                datafile["ANTENNA_SET"]=lofarmode
        except RuntimeError:
            print "ERROR opening file - skipping this file!"
            statuslist.append("OPEN FAILED")
            finish_file()
            continue



        ########################################################################
        #Setting filenames and directories
        ########################################################################
        (filedir,filename)=os.path.split(full_filename)

        if not lora_logfile or not os.path.isfile(lora_logfile):
            lora_logfile=os.path.join(loradir,"LORAtime4")

        (rootfilename,fileextensions)=os.path.splitext(filename)

        #filename_split=rootfilename.split("-")
        #projectname="-".join(filename_split[:-2]) if len(filename_split)>2 else filename_split[0]
        projectname="VHECR_RADIO"

        timesec=datafile["TIME"][0]
        timestr=time.strftime("%Y%m%dT%H%M%S",time.gmtime(timesec))

        timems = int(datafile["SAMPLE_INTERVAL"][0]*datafile["SAMPLE_NUMBER"][0]*1000)
        timems = str(timems).zfill(3) # make 3 digits with leading zeros if needed.
        # NOTE: chosen to take int part of time in ms; i.e. not rounding to nearest ms.
        
        #time_stamp=filename_split[-2] if not timestamp else timestamp
        time_stamp=timestr+"."+timems+"Z" if not timestamp else timestamp

        pretty_time_stamp=time.strftime("%Y-%m-%d %H:%M:%S."+timems,time.gmtime(timesec))

        station_name=metadata.idToStationName(datafile["CHANNEL_ID"][0]/1000000) if not station else station


        #station_name=filename_split[-1] 
        old_time_stamp=time_stamp
        #time_stamp=filename_split[-2] if not timestamp else timestamp
        #pretty_time_stamp=time_stamp[0:4]+"-"+time_stamp[4:6]+"-"+time_stamp[6:8]+" "+time_stamp[9:11]+":"+time_stamp[11:13]+":"+time_stamp[13:-1] if len(time_stamp)>13 else time_stamp

        outputdir_expanded=os.path.expandvars(os.path.expanduser(outputdir))
#        topdir_name=projectname+"-"+time_stamp
        topdir_name = rootfilename
        reldir_from_event = os.path.join("pol"+str(current_polarization),station_name)
        reldir_from_top = os.path.join(topdir_name,"pol"+str(current_polarization),station_name)
        outputdir_event=os.path.join(outputdir_expanded,topdir_name)
        outputdir_with_subdirectories=os.path.join(outputdir_event,reldir_from_event)
        outfilename=rootfilename+"-pol"+str(current_polarization)

        calibrated_timeseries_file=os.path.join(outputdir_with_subdirectories,outfilename+"-calibrated-timeseries")
        result_file=os.path.join(outputdir_with_subdirectories,outfilename+".results")
        calibrated_timeseries_cut_file=os.path.join(outputdir_with_subdirectories,outfilename+"-calibrated-timeseries-cut")
        calibrated_fft_cut_file=os.path.join(outputdir_with_subdirectories,outfilename+"-calibrated-fft-cut")

        htmlfilename=os.path.join(outputdir_with_subdirectories,"index.html")
        summaryfilename=os.path.join(outputdir_event,"index.html")
        goodsummaryfilename=os.path.join(outputdir,"summary-good.html")
        allsummaryfilename=os.path.join(outputdir,"summary.html")

        tasks.task_write_parfiles=True
        tasks.task_outputdir=outputdir_with_subdirectories
        nparfiles = len(tasks.task_parfiles)

        #Removing old parfiles, if they exist
        [os.remove(f) for f in listFiles([os.path.join(outputdir_with_subdirectories,"*.par"),os.path.join(outputdir_with_subdirectories,"*.par.txt"),os.path.join(outputdir_with_subdirectories,"*.gif"),os.path.join(outputdir_with_subdirectories,"*.png")])]

        print "Start Time:",file_time
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
            if skip_existing_files and os.path.exists(htmlfilename):
                print "# Resultfile ("+htmlfilename+") already exists - skipping file:",outputdir_with_subdirectories
                continue
            else:
                print "# Using existing output directory",outputdir_with_subdirectories

        #Reset the Pause instance, which will pause (or not) after each plot and write the plotfiles
        Pause.plotfiles=[]
        Pause.counter=0
        Pause.filename=os.path.join(outputdir_with_subdirectories,outfilename)


        ########################################################################
        #Continued opening the data file
        ########################################################################


        tbb_starttime=datafile["TIME"][0]
        tbb_samplenumber=datafile["SAMPLE_NUMBER"][0]
        sample_interval=datafile["SAMPLE_INTERVAL"][0]
        data_length=datafile["DATA_LENGTH"][0]

        results=dict(
            TELESCOPE=datafile["TELESCOPE"],
            ANTENNA_SET=datafile["ANTENNA_SET"],
            NYQUIST_ZONE=datafile["NYQUIST_ZONE"][0],
            SAMPLE_FREQUENCY=datafile["SAMPLE_FREQUENCY"][0],
            FREQUENCY_RANGE=datafile["FREQUENCY_RANGE"][0],
            NOF_DIPOLE_DATASETS=datafile["NOF_DIPOLE_DATASETS"],
            DATA_LENGTH=data_length,
            DATA_LENGTH_ms=data_length*sample_interval*1000,
            SAMPLE_INTERVAL=sample_interval,
            FILENAME=filename,
            TIME=tbb_starttime,
            SAMPLE_NUMBER=tbb_samplenumber,
            polarization=current_polarization,
            filedir=filedir,
            plotfiles=Pause.plotfiles
            )

        if max_data_length>0 and max(datafile["DATA_LENGTH"])>max_data_length:
            print "ERROR: Data file size is too large (",max(datafile["DATA_LENGTH"]),") - skipping this file!"
            statuslist.append("DATA_LENGTH BAD")
#            finish_file(status="FILE TOO LARGE")
#            continue

        if min(datafile["DATA_LENGTH"])<min_data_length:
            print "ERROR: Data file size is too small (",max(datafile["DATA_LENGTH"]),") - skipping this file!"
            finish_file(laststatus="FILE TOO SMALL")
            continue

        if blocknumber<0:
            block_number=0
#       Only use sample_number to indicate a 'good' pulse position
#        if samplenumber<0:
#            sample_number=blocksize/2

        ########################################################################
        #Getting information from LORA if present
        ########################################################################
        print "---> Reading information from LORA, if present"
        lora_event_info = 0 # to check with 'if lora_event_info:'
        if os.path.exists(lora_logfile):
            (tbb_starttime_sec,tbb_starttime_nsec)=lora.nsecFromSec(tbb_starttime,logfile=lora_logfile)

            if tbb_starttime_sec:
                try:
                    (block_number_lora,sample_number_lora)=lora.loraTimestampToBlocknumber(tbb_starttime_sec,tbb_starttime_nsec,tbb_starttime,tbb_samplenumber,blocksize=blocksize)
                except ValueError:
                    print "#ERROR - LORA trigger information not found"
                    finish_file(laststatus="NO TRIGGER")
                    continue
                print "---> Estimated block number from LORA: block =",block_number_lora,"sample =",sample_number_lora
                if blocknumber<0:
                    block_number=block_number_lora
                if samplenumber<0:
                    sample_number=sample_number_lora
                print "---> Taking as initial guess: block =",block_number,"sample =",sample_number
                lora_event_info=lora.loraInfo(tbb_starttime_sec,datadir=loradir,checkSurroundingSecond=True,silent=False)
        else:
            print "WARNING: No LORA logfile found - ",lora_logfile

        #lora_event_info=lora.loraInfo(tbb_starttime_sec,datadir=loradir,checkSurroundingSecond=True,silent=False)

        if lora_event_info:
                lora_direction=(lora_event_info["Azimuth"],lora_event_info["Elevation"])
                lora_core=(lora_event_info["Core(X)"],lora_event_info["Core(Y)"])
                lora_energy=lora_event_info["Energy(eV)"]

        results.update(dict(
            pulse_direction_lora=lora_direction,
            pulse_energy_lora=lora_energy,
            pulse_core_lora=lora_core
            ))

        ########################################################################
        #Setting the parameter block with parameters for tasks
        ########################################################################

        par=dict(
            plot_antenna=0,
            newfigure=False, #Don't create a new plotting window for some of the tasks, but use the old one.
            plot_finish=Pause,
            output_dir=outputdir_with_subdirectories,
            AverageSpectrum = dict(
                maxnchunks=maxnchunks, #To avoid too many empty blocks being read in, if DATA_LENGTH is wrong
                calc_incoherent_sum=True,
                addantennas=False,
                output_subdir=outputdir_with_subdirectories,
                filefilter=os.path.join(filedir,filename),
                root_filename=outfilename,
                lofarmode=lofarmode,
                antennas_start=current_polarization,
                antennas_stride=2,maxpeak=7000,meanfactor=3,peak_rmsfactor=5,rmsfactor=2,spikeexcess=500,
                blocklen=4096, # only used for quality checking within one block
        #        delta_nu=3000, # -> blocksize=2**16
                quality_db_filename="",
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
 #           DirectionFitTriangles = dict(maxiter=6,rmsfactor=2,minrmsfactor=0,direction_guess=lora_direction,direction_guess_label="LORA"), # determine delays iteratively
            DirectionFitTriangles = dict(maxiter=6,rmsfactor=2,minrmsfactor=0,direction_guess=lora_direction,direction_guess_label="LORA"), # determine delays iteratively
#            DirectionFitTriangles = dict(maxiter=2,rmsfactor=0,minrmsfactor=0,direction_guess=lora_direction,direction_guess_label="LORA"), # determine delays iteratively
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

        print "Note: You can get all antenna properties from ``antennacharacteristics=avspectrum.power.getHeader('antennacharacteristics')``"

        results.update(dict(
            antennas_timeseries_rms=list(avspectrum.power.getHeader("rms_antenna")) if avspectrum.power.getHeader("rms_antenna") else None,
            antennas_timeseries_npeaks=list(avspectrum.power.getHeader("npeaks_antenna")) if avspectrum.power.getHeader("npeaks_antenna") else None,
            station_timeseries_rms=avspectrum.power.getHeader("rms"),
            station_timeseries_npeaks=avspectrum.power.getHeader("npeaks"),
            station_antennas_homogeneity_factor=avspectrum.power.getHeader("homogeneity_factor") #how similar are the antennas
            ))
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
        flaglist=[i for i in quality if i["chunk"]==block_number and i["nblocksflagged"]>0] # was > 1 for LORA data....
        bad_antennas=[i["antenna"] for i in flaglist]
        bad_antennas_index=[antenna_index[i] for i in bad_antennas]
          # also flag as 'bad' those antennas which yield no average spectrum data 
          # i.e. all data was flagged in the avg spectrum process
        
        
        #Select good antennas
        good_antennas=antennalist[:]
        [good_antennas.remove(i) for i in bad_antennas] # remove the bad antennas
        good_antennas_index=[antenna_index[i] for i in good_antennas] # get the index numbers of good antennas pointing to position in array
        ndipoles=len(good_antennas)

        results["ndipoles"]=ndipoles
        if ndipoles<minimum_number_good_antennas:
            print "#ERROR: To few good antennas ("+str(ndipoles)+")"
            finish_file(laststatus="TOO FEW ANTENNAS")
            continue

        #Create new average spectrum with only good antennas
        if len(bad_antennas)>0:
            print "# Antenna Flagging:",len(bad_antennas),"bad antennas!"
            print "# Bad Antennas:",bad_antennas
            averagespectrum_good_antennas=hArray(dimensions=[ndipoles,speclen],properties=avspectrum.power)
            averagespectrum_good_antennas[...].copy(avspectrum.power[good_antennas_index,...])
        else:
            print "# Antenna Flagging: All antennas OK!"
            averagespectrum_good_antennas=avspectrum.power

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

            results["ndipoles"]=ndipoles
            if ndipoles<minimum_number_good_antennas:
                print "#ERROR: To few good antennas ("+str(ndipoles)+")"
                finish_file(laststatus="TOO FEW ANTENNAS")
                continue

            #Create new average spectrum with only good antennas
            print "# Antenna Flagging:",len(bad_antennas),"bad antennas!"
            print "# Bad Antennas:",bad_antennas
            averagespectrum_good_antennas=hArray(dimensions=[ndipoles,speclen],properties=avspectrum.power)
            averagespectrum_good_antennas[...].copy(avspectrum.power[good_antennas_index,...])

        good_antennas_IDs=hArray(good_antennas_index)*2+current_polarization

        print "---> Calculate a smooth version of the spectrum which is later used to set amplitudes."
        calcbaseline1=trerun("CalcBaseline",1,averagespectrum_good_antennas,pardict=par,invert=False,HanningUp=False,normalize=False,doplot=0)
        amplitudes=hArray(copy=calcbaseline1.baseline)

        #Get a measure of the total power (actually sqrt thereof)
        #received in each antenna (without RFI) and in the entire
        #station.

        antennas_power=amplitudes[...].mean()
        results.update(dict(
            antennas_spectral_power=list(antennas_power),
            station_spectral_power=antennas_power.mean()
            ))

        print "---> Calculate baseline again, but now for multiplication with data to flatten the spectrum."
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

        print "---> Load the block with the peak", block_number, blocksize
        datafile["BLOCKSIZE"]=blocksize #2**16
        datafile["BLOCK"]=block_number #93

        datafile["SELECTED_DIPOLES"]=good_antennas
        antenna_positions=datafile["ANTENNA_POSITIONS"]

        results.update(dict(
            svn_revision=PYCRREVNR,
            BLOCK=block_number,
            BLOCKSIZE=blocksize,
            pipeline_version=PYCRREVNR,
            antennas=dict(zip(range(ndipoles),good_antennas)),
            bad_antennas=bad_antennas,
            antenna_positions_station_XYZ_m=list(antenna_positions.vec()),
            antenna_positions_ITRF_m=list(datafile["ITRFANTENNA_POSITIONS"].vec()),
            antenna_positions_array_XYZ_m=list(metadata.convertITRFToLocal(datafile["ITRFANTENNA_POSITIONS"]).vec())
            ))

        #Getting original cabledelays
#        import pdb; pdb.set_trace()
        nofChannels = len(datafile["CHANNEL_ID"]) # already noted somewhere else?
        cabledelayList = [0.0] * nofChannels
#        cabledelays = hArray(float, dimensions = [nofChannels]) 
        # check if 'Cabledelays.pic' exists in results dir
        delayfile = os.path.join(outputdir, 'Cabledelays.pic')
        print delayfile
        if os.path.isfile(delayfile):
            print 'Reading cable delays'
            infile = open(delayfile, 'rb')
            cabledelay_database = pickle.load(infile)
            n=0
            for id in datafile["CHANNEL_ID"]:
                key = str(id)
                if key in cabledelay_database:
                    cabledelayList[n] = cabledelay_database[str(id)]["cabledelay"]
                n+=1 
            print cabledelayList
        else:
            print 'Cable delays file does not exist yet.'
        cabledelays = hArray(cabledelayList) # zeros if file isn't there. Other choice: take from metadata 
        
#        cabledelays_full=metadata.get("CableDelays",datafile["CHANNEL_ID"],datafile["ANTENNA_SET"])  # Obtain cabledelays
#        cabledelays_full-=cabledelays_full[0] # Correct w.r.t. referecence antenna
#        cabledelays=cabledelays_full % sample_interval #Only sub-sample correction has not been appliedcabledelays=cabledelays_full % 5e-9  # Only sub-sample correction has not been applied
        cabledelays=hArray(cabledelays)
  # ??????????????      cabledelays *= -1.0
        ########################################################################
        #Read block with peak and FFT
        ########################################################################
        timeseries_data=datafile["EMPTY_TIMESERIES_DATA"]
        try:
            timeseries_data.read(datafile,"TIMESERIES_DATA")
        except RuntimeError:
            print "Error reading file - skipping this file"
            finish_file(laststatus="READ ERROR")
            continue

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
        fft_data[...,0]=0 # take out zero (DC) offset (-> offset/mean==0)

        ########################################################################
        #RFI excision
        ########################################################################
        fft_data[...].randomizephase(applybaseline.dirty_channels[...,[0]:applybaseline.ndirty_channels.vec()],amplitudes[...])

        ########################################################################
        #Gain calibration of data
        ########################################################################
        print "---> Calculate a baseline with Galactic powerlaw"
        calcbaseline_galactic=trerun("CalcBaseline","galactic",averagespectrum_good_antennas,pardict=par,invert=True,normalize=False,powerlaw=0.5,doplot=Pause.doplot)
        #import pdb; pdb.set_trace()
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
        timeseries_calibrated_data=hArray(properties=timeseries_data)
        #fft_data[...,0]=0 # take out zero offset (-> offset/mean==0)
        hFFTWExecutePlan(timeseries_calibrated_data[...], fft_data[...], invfftplan)

        timeseries_calibrated_data /= blocksize # normalize back to original value

        timeseries_calibrated_data_rms=timeseries_calibrated_data.stddev(0.0).val()
        results.update(dict(
            pulse_height_rms=timeseries_calibrated_data_rms
            ))

        # Note: to finish calibration, we have to know the pulse location first
        # Then we divide out by the rms per antenna in this block, while excluding the pulse region.
        # so we use [0:pulse.start] ( and [pulse.end:] to be implemented).

        ########################################################################
        #Locate pulse and cut data around it
        ########################################################################
        #First determine where the pulse is in a simple incoherent sum of all time series data
        if (search_window_width > 0) and (sample_number > 0):
            # only narrow the search window if we have a 'good' guess for sample_number...
            search_window=(sample_number-search_window_width/2,sample_number+search_window_width/2)
            print search_window
        else:
            search_window=False

        if lora_direction:
            print "---> Now make an incoherent beam in the LORA direction, locate pulse, and cut time series around it."
            beamformer_full=trerun("BeamFormer2","bf_full",data=timeseries_calibrated_data,fftdata=fft_data,dofft=False,pardict=par,maxnantennas=ndipoles,antpos=antenna_positions,FarField=True,sample_interval=sample_interval,pointings=rf.makeAZELRDictGrid(lora_direction[0]*deg,lora_direction[1]*deg,1,nx=1,ny=1),cable_delays=cabledelays,calc_timeseries=True,doplot=False,doabs=True,smooth_width=5,plotspec=False,verbose=False,calc_tbeams=False)
            tbeam_incoherent=hArray(beamformer_full.tbeam_incoherent.vec(),[blocksize])#make this a one-dimensional array to not confuse LocatePulseTrain ...
        else:
            print "---> Now add all antennas in the time domain, locate pulse, and cut time series around it."
            tbeam_incoherent=None

        #Get a list of pulses in the search window
        pulses=trerun("LocatePulseTrain","separate",timeseries_calibrated_data,pardict=par,doplot=Pause.doplot,search_window=search_window,search_per_antenna=True)
        #How many antennas have pulses? Also: determine a finer window where the pulses are
        if pulses.npeaks>0:
            antennas_with_peaks=list(asvec(hArray(good_antennas)[pulses.peaks_found_list]))
            print "#LocatePulses - ",pulses.npeaks,"antennas with a pulse found. Pulse window start=",pulses.start,"+/-",pulses.start_rms,"pulse range:",pulses.start_min,"-",pulses.start_max
            print "#LocatePulses - antennas with pulses:",antennas_with_peaks

        #If we had a pre-determined direction use this to find the dominant peak and cut out the time series around it
        if lora_direction:
            pulse=trerun("LocatePulseTrain","",timeseries_calibrated_data,timeseries_data_sum=tbeam_incoherent,pardict=par,doplot=Pause.doplot,search_window=search_window,search_per_antenna=False)

        # Otherwise take the previously found window with the most pulses
        else:
            pulse=pulses
        pulse_npeaks=pulses.npeaks
        print "#LocatePulse: ",pulse_npeaks,"pulses found."

# Finish gain calibration - also apply gain calibration on time series data in 'pulse' result workspace.
        timeseries_calibrated_data_gainnormalisation = timeseries_calibrated_data[...,0:pulse.start].stddev(0.0)
        # improve to also use [pulse.end:]
        timeseries_calibrated_data[:,...].div(timeseries_calibrated_data_gainnormalisation)
        timeseries_calibrated_data_antennas_rms=timeseries_calibrated_data[...,0:pulse.start].stddev(0.0)
        # this now must contain all 1.0's

#        pulse.timeseries_data[:,...].div(timeseries_calibrated_data_gainnormalisation)
        # Note: pulse.timeseries_data is the same array (by ref) as timeseries_calibrated_data.

        pulse.timeseries_data_cut[:,...].div(timeseries_calibrated_data_gainnormalisation)
        # this is a copied array so needs to be normalised as well
#        timeseries_raw_rms = hArray(dimensions=timeseries_calibrated_data_antennas_rms)
        # get raw rms value to see how much calibration was done in total
        timeseries_raw_rms = timeseries_data[..., 0:pulse.start].stddev() # no DC offset correction was done
        
# calibration complete.
        print "---> Saving calibrated time series to",calibrated_timeseries_file
        timeseries_calibrated_data.write(calibrated_timeseries_file)

        if Pause.doplot: timeseries_calibrated_data[0:min(2,ndipoles),...].plot(title="Calibrated time series of first 2 antennas")
        Pause("Plotted time series data. ",name="calibrated-imeseries")

        #a = timeseries_calibrated_data.toNumpy()
        #plt.plot(a[:][pulse.start:pulse.end])

        print 'PULSE START = '
        print pulse.start
        results.update(dict(
            timeseries_rms=list(timeseries_calibrated_data_antennas_rms),
            timeseries_raw_rms=list(timeseries_raw_rms),
            npeaks_found=pulse_npeaks
            ))

        if pulse_npeaks==0:
            print "************************************************************************"
            print "********          ATTENTION: No pulses found          ******************"
            print "************************************************************************"
            print "ERROR: LocatePulseTrain: No pulses found!"
            finish_file(laststatus="NO PULSE")
            continue

        #Store the small version of the calibrated timeseries, e.g. for the imager
        print "---> Saving ",calibrated_timeseries_cut_file
        pulse.timeseries_data_cut.write(calibrated_timeseries_cut_file)

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


        ########################################################################
        #Determine antennas that have pulses with a high enough SNR for beamforming
        ########################################################################
        pulses_snr=maxima_power.maxy/timeseries_power_rms
        pulses_refant=pulses_snr.maxpos()

        antennas_with_strong_pulses=list(pulses_snr.Find(">",pulses_sigma))
        nantennas_with_strong_pulses=len(antennas_with_strong_pulses)

        results.update(dict(
            pulses_sigma=pulses_sigma,
            timeseries_power_mean=list(timeseries_power_mean),
            timeseries_power_rms=list(timeseries_power_rms),
            pulses_power_snr=list(pulses_snr),
            pulses_refant=pulses_refant,
            antennas_with_strong_pulses=antennas_with_strong_pulses,
            nantennas_with_strong_pulses=nantennas_with_strong_pulses
            ))

        if nantennas_with_strong_pulses<min_number_of_antennas_with_pulses:
            print "ERROR: LocatePulseTrain: Not enough pulses found for beam forming!"
            finish_file(laststatus="TOO FEW PULSES")
            continue


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
        delays=hArray(copy=cabledelays)
        #cabledelays *= 2
        #delays.fill(0)

        #Only take those lags of pulses with a high-enough SNR
        good_pulse_lags=hArray(time_lags)[antennas_with_strong_pulses]
        good_pulse_antenna_positions=hArray(float,[nantennas_with_strong_pulses,3])
        good_pulse_antenna_positions[...].copy(antenna_positions[antennas_with_strong_pulses,...,0:3])
        good_pulse_cabledelays=cabledelays[antennas_with_strong_pulses]

        direction=trerun("DirectionFitTriangles","direction",pardict=par,positions=good_pulse_antenna_positions,timelags=good_pulse_lags,cabledelays=good_pulse_cabledelays,verbose=True,doplot=True)
        print "========================================================================"
        print "Triangle Fit Az/EL -> ", direction.meandirection_azel_deg,"deg"
        print " "
        print "Delays =",direction.total_delays * 1e9

        print "#DirectionFitTriangles: delta delays =",direction.delta_delays_mean_history[0],"+/-",direction.delta_delays_rms_history[0]

      #  import pdb; pdb.set_trace()
        (direction.total_delays*1e9).plot(xvalues=good_antennas_IDs[antennas_with_strong_pulses],linestyle="None",marker="x")
        (direction.delays_history*1e9)[1].plot(clf=False,xvalues=good_antennas_IDs[antennas_with_strong_pulses],linestyle="None",marker="o")
        (cabledelays*1e9).plot(clf=False,xlabel="Antenna",ylabel="Delay [ns]",xvalues=good_antennas_IDs,title="Fitted cable delays",legend=(["fitted delay","1st step","cable delay"]))
        Pause(name="fitted-cable-delays")

        scrt = hArray(copy=direction.residual_delays); scrt.abs(); # NB! Need to copy the hArray
        # otherwise, the original array will get modified by scrt.abs().
        delay_quality_error=scrt.median()[0]/maximum_allowed_delay
        print "#Delay Quality Error:",delay_quality_error

        # also count # outliers, impose maximum
        number = hArray(int, dimensions=scrt)
        delay_outliers = number.findgreaterthanabs(scrt, maximum_allowed_delay)[0] # returns count, indices in 'number' array
        
        print "# delay outliers: ", delay_outliers

        if delay_quality_error>1:
            print "************************************************************************"
            print "********ATTENTION: Fitted delays deviate too strongly!******************"
            print "********            check antennna positions          ******************"
            print "************************************************************************"
            print "#DirectionFitTriangles: ERROR!"
        else:
            print "#DirectionFitTriangles: OK!"

        print "---> Checking and flagging delays which are larger than +/-"+str(maximum_allowed_delay)

        final_residual_delays=hArray(float,[ndipoles],fill=0)
        final_residual_delays[antennas_with_strong_pulses,...]=direction.residual_delays[...]
        flagged_delays=final_residual_delays.Find('outside',-maximum_allowed_delay,maximum_allowed_delay)
        if len(flagged_delays)>0:
            print "#Delay flagging - these residual delays are suspicious:",flagged_delays
            if flag_delays:
                print "#Delay flagging - setting them to zero!"
                final_residual_delays.set(flagged_delays,0)
        else:
            print "#Delay flagging - all delays seem OK."
        final_cable_delays=final_residual_delays+cabledelays

        # include average cable delays (as in cable delay database) in time lags per pulse
        time_lags = hArray(time_lags)
        time_lags -= cabledelays
        
     #   import pdb; pdb.set_trace()
        
        #plotdirection=trerun("PlotDirectionTriangles","plotdirection",pardict=par,centers=direction.centers,positions=direction.positions,directions=direction.directions,title="Sub-Beam directions")
        #Pause(name="sub-beam-directions")
        trerun("PlotAntennaLayout","Delays",pardict=par,positions=good_pulse_antenna_positions,colors=final_residual_delays[antennas_with_strong_pulses],sizes=100,names=good_antennas_IDs[antennas_with_strong_pulses],title="Delay errors in station",plotlegend=True)

        print "\n--->Imaging"

        ########################################################################
        #Imaging
        ########################################################################
        im=trerun("CRImager", "im", data = pulse.timeseries_data_cut, blocksize = pulse.timeseries_data_cut.shape()[1], antpos = antenna_positions, output=os.path.join(outputdir_with_subdirectories,"crimage.fits"))

        print "\n--->Beamforming"

        ########################################################################
        #Beamforming
        ########################################################################

        #Beamform short data set first for inspection (and possibly for
        #maximizing later)
        bf=trerun("BeamFormer2","bf",data=pulse.timeseries_data_cut,pardict=par,maxnantennas=ndipoles,antpos=antenna_positions,FarField=True,sample_interval=sample_interval,pointings=rf.makeAZELRDictGrid(*(direction.meandirection_azel+(10000,)),nx=3,ny=3),cable_delays=final_cable_delays,calc_timeseries=True,doplot=2 if Pause.doplot else False,doabs=True,smooth_width=5,plotspec=False,verbose=False)

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
            timeseries_power_shifted[...].plot(title="abs(E-Field) corrected for delays in beam direction per antenna",xlabel="Samples")
            plt.legend(good_antennas,ncol=2,title="Antennas",prop=matplotlib.font_manager.FontProperties(family="Helvetica",size=7))
            Pause(name="shifted-power-per-antenna")

        #matplotlib.font_manager.FontProperties(self, family=None, style=None, variant=None, weight=None, stretch=None, size=None, fname=None, _init=None)

        print "---> Plotting full beam-formed data set"
        #Beamform full data set (not really necessary, but fun).
        beamformed=trerun("BeamFormer2","beamformed",data=pulse.timeseries_data,pardict=par,maxnantennas=ndipoles,antpos=antenna_positions,FarField=True,sample_interval=sample_interval,pointings=rf.makeAZELRDictGrid(*(direction.meandirection_azel+(10000,)),nx=1,ny=1),cable_delays=final_cable_delays,calc_timeseries=False,doabs=False,smooth_width=0,doplot=False,plotspec=False,verbose=False)

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
        pulse_height_incoherent=pulses_strength.mean()

        trerun("PlotAntennaLayout","TimeLags",pardict=par,positions=antenna_positions,colors="w",sizes=pulses_strength,sizes_min=0,names=good_antennas_IDs,title="Time Lags in Station",plotlegend=False,plot_clf=True,plot_finish=plot_draw)
        trerun("PlotAntennaLayout","TimeLags",pardict=par,positions=good_pulse_antenna_positions,colors=hArray(time_lags)[antennas_with_strong_pulses],sizes=hArray(pulses_strength)[antennas_with_strong_pulses],names=good_antennas_IDs[antennas_with_strong_pulses],sizes_min=0,title="Time Lags in Station",plotlegend=True,plot_clf=False)


        ########################################################################
        #Preparing and writing results record
        ########################################################################

        pulse_normalized_height=beam_maxima.maxy.val()/lora_energy*10**17 if lora_energy>0 else -1.0 #Need Horneffer formula here
        pulse_height=beam_maxima.maxy.val()
        pulse_direction=direction.meandirection_azel_deg

        results.update(dict(
            antennas_flagged_delays=list(flagged_delays.vec()),
            antennas_final_cable_delays=list(final_cable_delays.vec()),
            antennas_residual_cable_delays = list(final_residual_delays.vec()),
            antennas_with_peaks=antennas_with_peaks,
            delay_quality_error=delay_quality_error,
            delay_outliers = delay_outliers,
            pulse_location=beam_maxima.maxx.val(),
            pulses_strength=list(pulses_strength.vec()),
            pulses_maxima_x=list(maxima_power.maxx),
            pulses_maxima_y=list(maxima_power.maxy),
            pulses_timelags_ns=list(time_lags),
            pulse_start_sample=pulse.start,
            pulse_end_sample=pulse.end,
            pulse_time_ms=pulse_time_ms,
            pulse_height=pulse_height,
            pulse_height_incoherent=pulse_height_incoherent,
            pulse_normalized_height=pulse_normalized_height,
            pulse_direction=pulse_direction,
            pulse_direction_delta_delays_start=direction.delta_delays_mean_history[0],
            pulse_direction_delta_delays_final=direction.delta_delays_mean_history[-1]
            ))

        event.par.quality=quality
        event.par.timeseries_data=pulse.timeseries_data_cut
        event.par.results=results
        event.write(result_file)

        if delay_quality_error >= 1:
            final_status = "BAD"
        elif delay_outliers > max_outliers:
            final_status = "BAD / TOO MANY OUTLIERS (" + str(delay_outliers) + ")"
        else:
            final_status = "OK"
        results.update(dict(status = final_status))
        #Writing results.py
        f=open(os.path.join(outputdir_with_subdirectories,"results.py"),"w")
        f.write("#"+outfilename+"\nresults="+str(results))
        f.close()

        # Writing results to XML file
        xmldict.dump(os.path.join(outputdir_with_subdirectories,"results.xml"), results)
        
        finish_file(laststatus = final_status)

plt.ion()
