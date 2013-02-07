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
results.xml. Just use ``results=xmldict.load(os.path.join(outputdir,"results.xml"))``.

There is also a summary of the results ``summary.html`` and
``summary-good.html``in the top level results directory.

**Example:**

*Command line use:*
::
    chmod a+x $LOFARSOFT/src/PyCRTools/pipelines/cr_event.py

    $LOFARSOFT/src/PyCRTools/pipelines/cr_event.py ~/LOFAR/work/data/VHECR_LORA-20110716T094509.665Z-002.h5 --outputdir=/Users/falcke/LOFAR/work/results --loradir /Users/falcke/LOFAR/work/data/ --lora_logfile LORAtime4 --search_window_width=4000 --nsigma=3 --max_data_length=12289024 -R -p0

run  $LOFARSOFT/src/PyCRTools/pipelines/cr_event.py ~/LOFAR/work/data/VHECR_LORA-*.h5 --outputdir=/Users/falcke/LOFAR/work/results --loradir /Users/falcke/LOFAR/work/data/ --lora_logfile LORAtime4 --search_window_width=4000 --nsigma=3 --max_data_length=12289024 -R

------------------------------------------------------------------------

Test event: Event-1, LBA_OUTER
filename="lora-event-1-station-2.h5"
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
usage = """
usage: %prog [--database <databasename>] [options] <datafile.h5>
       %prog --database <databasename> [-i <eventID>] [options]
"""

parser = OptionParser(usage=usage)

parser.add_option("--database", default=None, help="Location of the database file")
parser.add_option("-i","--id", type="int", dest="event_id", default=0, help="event ID as stored in the database")

parser.add_option("-o","--outputdir", type="str", default="/data/VHECR/LORAtriggered/results",help="directory where to store the final results (will be stored in a subdirectory filename.dir/ within that directory)")
parser.add_option("-L","--lora_logfile", type="str", default="LORAtime4",help="LORA logfile (LORAtime4)")
parser.add_option("--loradir", type="str", default="/data/VHECR/LORAtriggered/LORA/",help="Directory to find LORA information")
parser.add_option("-d","--datadir", type="str", default="/data/VHECR/LORAtriggered/data",help="Will be added to each of the filename arguments (hence, use quotations for the main arguments if you use this option and have asterisks in the argument!)")
parser.add_option("-p","--polarization", type="int", default=-1,help="either 0 or 1 for selecting even or odd antennas, or -1 for both")
parser.add_option("-t","--nsigma", type="float", default=3.0,help="Threshold for identifying peaks")
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
parser.add_option("-C","--checksum", action="store_true", help="Calculate checksums used for debugging")
parser.add_option("-I","--imager", action="store_true", help="Run imager")
parser.add_option("-O", "--max_outliers", type="int", default=24, help="Maximum allowed number of outliers in calculated cable delays")
parser.add_option("-r", "--randomize_rfi", action="store_true", help="Replace RFI lines with mean value at random phase.")
parser.add_option("-F", "--full_output", action="store_true", help="Write data arrays to pcr files and task parameters to par files.")
parser.add_option("--host", default=None, help="PostgreSQL host.")
parser.add_option("--user", default=None, help="PostgreSQL user.")
parser.add_option("--password", default=None, help="PostgreSQL password.")
parser.add_option("--dbname", default=None, help="PostgreSQL dbname.")

if parser.get_prog_name()=="cr_event.py":
    (options, args) = parser.parse_args()
    if options.norefresh:
        nogui=True
        import matplotlib
        matplotlib.use('Agg')



import pickle

from pycrtools import *
from pycrtools import lora
from pycrtools import xmldict
from pycrtools import crdatabase as crdb
from pycrtools.tasks import averagespectrum, beamformer2, fitbaseline, pulsecal, crosscorrelateantennas, directionfitplanewave, crimager

#plt.figure(num=1, figsize=(8*2, 6*2), dpi=300, facecolor='w', edgecolor='k')

#------------------------------------------------------------------------
# Main input parameters
#------------------------------------------------------------------------
flag_delays = False
minimum_number_good_antennas=8
timeseries_data_cut_pulse_width=12
min_number_of_antennas_with_pulses=8
start_time=time.strftime("%A, %Y-%m-%d at %H:%M:%S")

if not parser.get_prog_name()=="cr_event.py":
    #   Program was run from within python
    filefilter="~/LOFAR/work/CR/lora-event-1-station-2.h5"
    filefilter="~/LOFAR/work/CR/LORAweekdump--2-15.h5"
    filefilter="~/LOFAR/work/data/VHECR_LORA-20110716T094509.665Z-002.h5"


    outputdir="/Users/falcke/LOFAR/work/results"
    lora_logfile="/Users/falcke/LOFAR/work/data/LORAtime4"
    polarization=1 # either 1 or 0 for odd or even antennapolarization=1 # either 1 or 0 for odd or even antenna
    blocknumber=93
    blocksize=2**16
    nsigma=4
    timestamp=""
    station=""
    plotpause=False
    refresh=True
    search_window_width=-1
    sample_number=-1
    event_id = 0
    db = None

    maximum_allowed_delay=1e-8 # maximum differential mean cable delay
                               # that the expected positions can differ
                               # from the measured ones, before we
                               # consisder something to be wrong
else:
    #   Program was started from command line


    datadir = options.datadir
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
    statuslist = []

    polarizations=[0,1] if polarization<0 else [polarization]

    # Database
    db = None
    if (options.database or options.host):
        db_manager = crdb.CRDatabase(filename = options.database, host = options.host, user = options.user, password = options.password, dbname = options.dbname)
        db = db_manager.db

    if db:
        datadir = db_manager.settings.datapath
        outputdir = db_manager.settings.resultspath
        loradir = db_manager.settings.lorapath

    event_id = options.event_id

    # Filenames
    if args:
        filefilter = [os.path.join(datadir,f) for f in args]
    else:
        filefilter = []

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
    if delay_quality_error<1:
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
    print "which can be found in `results.xml`(use, e.g., xmldict)."
    print "Shifted time series data of all antennas is in event.par.time_series."
    print "Open",htmlfilename,"in your browser to get a summary."
    print "-----------------------------------------------------------------------------------------------------------"
    print "Finished cr_event after",time.clock()-t0,"seconds at",time.strftime("%A, %Y-%m-%d at %H:%M:%S")
    print "-----------------------------------------------------------------------------------------------------------\n"


def process_datafile(full_filename, datafile_info=None, dump_html=False):

    old_time_stamp = None
    time_stamp=""
    polarizations=[0,1] if polarization<0 else [polarization]

    print "  Processing file: %s..." %(full_filename)

    station_info = None
    if datafile_info:
        station_info = datafile_info.stations[0] # Only process 1st station

    for current_polarization in polarizations:
        polarization_info = None
        if station_info:
            if station_info.polarization:
                polarization_info = station_info.polarization["%d" %(current_polarization)]
                polarization_info.alt_status = "PROCESSING"
                polarization_info.write(recursive=False, parameters=False)

        results={}
        results["pulse_height_incoherent"]=0.0
        t0=time.clock()
        file_time=time.strftime("%A, %Y-%m-%d at %H:%M:%S")
        file_time_short=time.strftime("%Y-%m-%d  %H:%M:%S")
        #------------------------------------------------------------------------
        # Initialization of some parameters
        #------------------------------------------------------------------------
        delay_quality_error=99.
        lora_direction=False; lora_energy=-1.0; lora_core=(0.0,0.0); lora_coreuncertainties=(0.0,0.0); lora_moliere=-1.0
        pulse_normalized_height=-1.0
        pulse_height=-1.0
        pulse_direction=(-99.,-99.)
        pulse_npeaks=-1
        tasks.task_logger=[]
        statuslist=[]
        block_number=blocknumber
        sample_number=samplenumber

        checksums = []
        ########################################################################
        #Open the data file
        ########################################################################
        print "---> Open data file",full_filename
        try:
            datafile=open(full_filename);
            if datafile["ANTENNA_SET"]=="UNDEFINED":
                datafile["ANTENNA_SET"]="LBA_OUTER"
        except RuntimeError as e:
            print "ERROR opening file - skipping this file!"
            statuslist.append("OPEN FAILED "+str(e))
            # xmldict.dump(os.path.join(outputdir_with_subdirectories,"results.xml"), results)
            write_results_to_database(results, polarization_info, xml_file=os.path.join(outputdir_with_subdirectories,"results.xml"))
            if datafile_info:
                datafile_info.alt_status = "OPEN FAILED"
                datafile_info.write(recursive=False, parameters=False)
            if polarization_info:
                polarization_info.alt_status = "BAD"
                polarization_info.write(recursive=False, parameters=False)
            if dump_html:
                finish_file()
            continue



        ########################################################################
        #Setting filenames and directories
        ########################################################################
        (filedir,filename)=os.path.split(full_filename)

        if not options.lora_logfile or not os.path.isfile(options.lora_logfile):
            lora_logfile=os.path.join(loradir,"LORAtime4")

        (rootfilename,fileextensions)=os.path.splitext(filename)

        #filename_split=rootfilename.split("-")
        #projectname="-".join(filename_split[:-2]) if len(filename_split)>2 else filename_split[0]
        projectname="VHECR_LORA"

        timesec=datafile["TIME"][0]
        timestr=time.strftime("%Y%m%dT%H%M%S",time.gmtime(timesec))

        timems = int(datafile["SAMPLE_INTERVAL"][0]*datafile["SAMPLE_NUMBER"][0]*1000)
        timems = str(timems).zfill(3) # make 3 digits with leading zeros if needed.
        # NOTE: chosen to take int part of time in ms; i.e. not rounding to nearest ms.

        #time_stamp=filename_split[-2] if not timestamp else timestamp
        time_stamp=timestr+"."+timems+"Z" if not timestamp else timestamp

        pretty_time_stamp=time.strftime("%Y-%m-%d %H:%M:%S."+timems,time.gmtime(timesec))

        #station_name = metadata.idToStationName(datafile["CHANNEL_ID"][0]/1000000) if not station else station
        station_name = "%03d" %(datafile["CHANNEL_ID"][0]/1000000) if not station else station
        station_name = "CS{0}".format(station_name)

        #station_name=filename_split[-1]
        old_time_stamp=time_stamp
        #time_stamp=filename_split[-2] if not timestamp else timestamp
        #pretty_time_stamp=time_stamp[0:4]+"-"+time_stamp[4:6]+"-"+time_stamp[6:8]+" "+time_stamp[9:11]+":"+time_stamp[11:13]+":"+time_stamp[13:-1] if len(time_stamp)>13 else time_stamp

        outputdir_expanded=os.path.expandvars(os.path.expanduser(outputdir))
        topdir_name=projectname+"-"+time_stamp
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

        if options.full_output:
            tasks.task_write_parfiles=True
        else:
            tasks.task_write_parfiles=False
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
                # Write parameters to database
                write_results_to_database(results, polarization_info, xml_file=os.path.join(outputdir_with_subdirectories,"results.xml"))
                if polarization_info:
                    polarization_info.alt_status = "BAD"
                    polarization_info.write(recursive=False, parameters=True)
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
        tbb_samplenumber=max(datafile["SAMPLE_NUMBER"])
        sample_interval=datafile["SAMPLE_INTERVAL"][0]
        data_length=datafile["DATA_LENGTH"][0]

        results.update(dict(
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
            ))

        if "HBA" in datafile["ANTENNA_SET"]:
            statuslist.append("HBA SKIPPED")
            # Write parameters to database
            write_results_to_database(results, polarization_info, xml_file=os.path.join(outputdir_with_subdirectories,"results.xml"))
            if polarization_info:
                polarization_info.alt_status = "BAD"
                polarization_info.write(recursive=False, parameters=False)
            if dump_html:
                finish_file()
            continue

        if max_data_length>0 and max(datafile["DATA_LENGTH"])>max_data_length:
            print "ERROR: Data file size is too large (",max(datafile["DATA_LENGTH"]),") - skipping this file!"
            statuslist.append("DATA_LENGTH BAD")

        if min(datafile["DATA_LENGTH"])<min_data_length or min(datafile["DATA_LENGTH"]) < blocksize:
            print "ERROR: Data file size is too small (",max(datafile["DATA_LENGTH"]),") - skipping this file!"
            statuslist.append("FILE TOO SMALL")
            # Write parameters to database
            write_results_to_database(results, polarization_info, xml_file=os.path.join(outputdir_with_subdirectories,"results.xml"))
            if polarization_info:
                polarization_info.alt_status = "BAD"
                polarization_info.write(recursive=False, parameters=False)
            if dump_html:
                finish_file()
            continue

        if blocknumber<0:
            block_number=(data_length/blocksize-1)/2
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
                    statuslist.append("TRIGGER TOO LATE")
                    # Write parameters to database
                    write_results_to_database(results, polarization_info, xml_file=os.path.join(outputdir_with_subdirectories,"results.xml"))
                    if polarization_info:
                        polarization_info.alt_status = "BAD"
                        polarization_info.write(recursive=False, parameters=False)
                    if dump_html:
                        finish_file()
                    continue
                print "---> Estimated block number from LORA: block =",block_number_lora,"sample =",sample_number_lora
                if blocknumber<0:
                    block_number=block_number_lora
                if samplenumber<0:
                    sample_number=sample_number_lora
                print "---> Taking as initial guess: block =",block_number,"sample =",sample_number
                lora_event_info=lora.loraInfo(tbb_starttime_sec,datadir=loradir,checkSurroundingSecond=True,silent=False)
            else:
                statuslist.append("TIMESTAMP NOT IN LORA LOGFILE")
                # Write parameters to database
                write_results_to_database(results, polarization_info, xml_file=os.path.join(outputdir_with_subdirectories,"results.xml"))
                if polarization_info:
                    polarization_info.alt_status = "BAD"
                    polarization_info.write(recursive=False, parameters=False)
                if dump_html:
                    finish_file()
                continue
                # print "WARNING: LORA logfile found but no info for this event time in LORAtime4 file!"
        else:
            print "WARNING: No LORA logfile found - ",lora_logfile

        #lora_event_info=lora.loraInfo(tbb_starttime_sec,datadir=loradir,checkSurroundingSecond=True,silent=False)

        if lora_event_info:
                lora_direction=(lora_event_info["Azimuth"],lora_event_info["Elevation"])
                lora_core=(lora_event_info["Core_X"],lora_event_info["Core_Y"])
                lora_energy=lora_event_info["Energy_eV"]
                lora_coreuncertainties=lora_event_info["coreuncertainties"]
                lora_moliere=lora_event_info["moliere"]

        results.update(dict(
            pulse_direction_lora=lora_direction,
            pulse_energy_lora=lora_energy,
            pulse_core_lora=lora_core,
            pulse_coreuncertainties_lora=lora_coreuncertainties,
            pulse_moliere_lora=lora_moliere
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
                store_spectra = options.full_output,
                maxnchunks=maxnchunks, #To avoid too many empty blocks being read in, if DATA_LENGTH is wrong
                calc_incoherent_sum=True,
                addantennas=False,
                output_subdir=outputdir_with_subdirectories,
                filefilter=os.path.join(filedir,filename),
                root_filename=outfilename,
                antennas_start=current_polarization,
                antennas_stride=2,maxpeak=7,meanfactor=3,peak_rmsfactor=5,rmsfactor=2,spikeexcess=7,
                blocklen=4096, # only used for quality checking within one block
        #        delta_nu=3000, # -> blocksize=2**16
                quality_db_filename="",
                chunksize_estimated=blocksize, # -> blocksize=2**16
                rmsrange=(0.5,50) #Range of RMS that is allowed in timeseries
                                  #before flagging. RMS is related to power,
                                  #hence we check for power excesses or
                                  #deficits in an antenna.
                ),
            FitBaseline = dict(ncoeffs=80,numin=30,numax=85,fittype="POLY",splineorder=3),
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
        flaglist=[i for i in quality if i["chunk"]==block_number and i["nblocksflagged"]>1]
        bad_antennas=[i["antenna"] for i in flaglist]
        bad_antennas_index=[antenna_index[i] for i in bad_antennas]

        #Select good antennas
        good_antennas=antennalist[:]
        [good_antennas.remove(i) for i in bad_antennas] # remove the bad antennas
        good_antennas_index=[antenna_index[i] for i in good_antennas] # get the index numbers of good antennas pointing to position in array
        ndipoles=len(good_antennas)

        results["ndipoles"]=ndipoles
        if ndipoles<minimum_number_good_antennas:
            print "#ERROR: To few good antennas ("+str(ndipoles)+")"
            statuslist.append("TOO FEW ANTENNAS")
            # Write parameters to database
            write_results_to_database(results, polarization_info, xml_file=os.path.join(outputdir_with_subdirectories,"results.xml"))
            if polarization_info:
                polarization_info.alt_status = "BAD"
                polarization_info.write(recursive=False, parameters=False)
            if dump_html:
                finish_file()
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

        if do_checksums:
            checksum = averagespectrum_good_antennas.checksum()
            checksums.append('Checksum after average spectrum: ' + checksum)
            print checksums[-1]

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
                statuslist.append("TOO FEW ANTENNAS")
                # Write parameters to database
                write_results_to_database(results, polarization_info, xml_file=os.path.join(outputdir_with_subdirectories,"results.xml"))
                if polarization_info:
                    polarization_info.alt_status = "BAD"
                    polarization_info.write(recursive=False, parameters=False)
                if dump_html:
                    finish_file()
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

        antennas_power=hArray(amplitudes[...].mean())
        results.update(dict(
            antennas_spectral_power=list(antennas_power),
            station_spectral_power=antennas_power.mean().val()
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

        print "---> Load the block with the peak"
        datafile["BLOCKSIZE"]=blocksize #2**16
        datafile["BLOCK"]=block_number #93

        datafile["SELECTED_DIPOLES"]=good_antennas
        antenna_positions=datafile["ANTENNA_POSITIONS"]

        results.update(dict(
            BLOCK=block_number,
            BLOCKSIZE=blocksize,
            antennas=dict(zip(range(ndipoles),good_antennas)),
            bad_antennas=bad_antennas,
            antenna_positions_station_XYZ_m=list(antenna_positions.vec()),
            antenna_positions_ITRF_m=list(datafile["ITRFANTENNA_POSITIONS"].vec()),
            antenna_positions_array_XYZ_m=list(metadata.convertITRFToLocal(datafile["ITRFANTENNA_POSITIONS"]).vec())
            ))

        #Getting original cabledelays
#        import pdb; pdb.set_trace()
#        nofChannels = len(datafile["CHANNEL_ID"]) # already noted somewhere else?
#        cabledelayList = [0.0] * nofChannels
#        cabledelays = hArray(float, dimensions = [nofChannels])
        # check if 'Cabledelays.pic' exists in results dir
#        delayfile = os.path.join(outputdir, 'Cabledelays.pic')
#        print delayfile
#        if os.path.isfile(delayfile):
#            print 'Reading cable delays'
#            infile = open(delayfile, 'rb')
#            cabledelay_database = pickle.load(infile)
#            n=0
#            for id in datafile["CHANNEL_ID"]:
#                key = str(id)
#                if key in cabledelay_database:
#                    cabledelayList[n] = cabledelay_database[str(id)]["cabledelay"]
#                n+=1
#            print cabledelayList
#        else:
#            print 'Cable delays file does not exist yet.'
#        cabledelays = hArray(cabledelayList) # zeros if file isn't there. Other choice: take from metadata

#        cabledelays_full=metadata.get("CableDelays",datafile["CHANNEL_ID"],datafile["ANTENNA_SET"])  # Obtain cabledelays
#        cabledelays_full-=cabledelays_full[0] # Correct w.r.t. referecence antenna
#        cabledelays=cabledelays_full % sample_interval #Only sub-sample correction has not been appliedcabledelays=cabledelays_full % 5e-9  # Only sub-sample correction has not been applied
#        cabledelays=hArray(cabledelays)

        ########################################################################
        #Read block with peak and FFT
        ########################################################################
        timeseries_data=datafile["EMPTY_TIMESERIES_DATA"]
        try:
            timeseries_data.read(datafile,"TIMESERIES_DATA")
        except RuntimeError:
            print "Error reading file - skipping this file"
            statuslist.append("READ ERROR")
            # Write parameters to database
            write_results_to_database(results, polarization_info, xml_file=os.path.join(outputdir_with_subdirectories,"results.xml"))
            if polarization_info:
                polarization_info.alt_status = "BAD"
                polarization_info.write(recursive=False, parameters=False)
            if dump_html:
                finish_file()
            continue

        if do_checksums:
            checksum = timeseries_data.checksum()
            checksums.append('Checksum after reading in timeseries data: ' + checksum)
            print checksums[-1]

        for i in bad_antennas_index:
            print "FLAGGING ANTENNA", i

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

            results.update(dict(
                DIPOLE_CALIBRATION_DELAY_APPLIED=True
                ))

        except IOError:
            results.update(dict(
                DIPOLE_CALIBRATION_DELAY_APPLIED=False
                ))

        if do_checksums:
            checksum = timeseries_data.checksum()
            checksums.append('Checksum after applying cable delays and invfft: ' + checksum)
            print checksums[-1]

        ########################################################################
        #RFI excision
        ########################################################################
        if not options.randomize_rfi:
            amplitudes.fill(0.)
        fft_data[...].randomizephase(applybaseline.dirty_channels[...,[0]:applybaseline.ndirty_channels.vec()],amplitudes[...])

        results.update(dict(
            dirty_channels=list(set(applybaseline.dirty_channels.vec()))
            ))

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
        if (search_window_width > 0) and (sample_number > 0):
            # only narrow the search window if we have a 'good' guess for sample_number...
            search_window=(max(0, sample_number-search_window_width/2), min(timeseries_calibrated_data.shape()[1] - 1, sample_number+search_window_width/2))
        else:
            search_window=False

        if lora_direction:
            print "---> Now make an incoherent beam in the LORA direction, locate pulse, and cut time series around it."
            beamformer_full=trerun("BeamFormer2","bf_full",data=timeseries_calibrated_data,fftdata=fft_data,dofft=False,pardict=par,maxnantennas=ndipoles,antpos=antenna_positions,FarField=True,sample_interval=sample_interval,pointings=rf.makeAZELRDictGrid(lora_direction[0]*deg,lora_direction[1]*deg,1,nx=1,ny=1), calc_timeseries=True,doplot=False,doabs=True,smooth_width=5,plotspec=False,verbose=False,calc_tbeams=False,store_spectrum=options.full_output)
            # removed cabledelays
            tbeam_incoherent=hArray(beamformer_full.tbeam_incoherent.vec(),[blocksize])#make this a one-dimensional array to not confuse LocatePulseTrain ...
        else:
            print "---> Now add all antennas in the time domain, locate pulse, and cut time series around it."
            tbeam_incoherent=None

        #If we had a pre-determined direction use this to find the dominant peak and cut out the time series around it
        if lora_direction:
            print 'Have lora direction...'
            pulse=trerun("LocatePulseTrain","",timeseries_calibrated_data,timeseries_data_sum=tbeam_incoherent,pardict=par,doplot=Pause.doplot,search_window=search_window,search_per_antenna=False)
            antennas_with_peaks=good_antennas
            pulse_npeaks=pulse.npeaks

        # Otherwise take the previously found window with the most pulses
        if not lora_direction or (lora_direction and pulse.npeaks==0):
            print "searching per antenna"
            pulse=trerun("LocatePulseTrain","separate",timeseries_calibrated_data,pardict=par,doplot=Pause.doplot,search_window=False,search_per_antenna=True)
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
            statuslist.append("NO PULSE")
            # Write parameters to database
            write_results_to_database(results, polarization_info, xml_file=os.path.join(outputdir_with_subdirectories,"results.xml"))
            if polarization_info:
                polarization_info.alt_status = "BAD"
                polarization_info.write(recursive=False, parameters=False)
            if dump_html:
                finish_file()
            continue

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
        if options.full_output:
            print "---> Saving calibrated time series to",calibrated_timeseries_file
            timeseries_calibrated_data.write(calibrated_timeseries_file)

        if do_checksums:
            checksum = timeseries_calibrated_data.checksum()
            checksums.append('Checksum after getting FINAL timeseries_calibrated_data: ' + checksum)
            print checksums[-1]

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

        #Store the small version of the calibrated timeseries, e.g. for the imager
        if options.full_output:
            print "---> Saving ",calibrated_timeseries_cut_file
            pulse.timeseries_data_cut.write(calibrated_timeseries_cut_file)

        print "---> Get peaks in power of each antenna (Results in maxima_power.maxy/maxx)."
        timeseries_power=hArray(copy=pulse.timeseries_data_cut)
#        upsampled_timeseries_data = hArray(float, dimensions=[timeseries_power.shape()[0], 16*timeseries_power.shape()[1]])
#        hFFTWResample(upsampled_timeseries_data[...], timeseries_power[...])
        timeseries_power.square()
#        upsampled_timeseries_data.square()
#        hFFTWResample(self.timeseries_data_resampled[...], self.timeseries_data[..., self.pulse_start:self.pulse_end])

#        import pdb; pdb.set_trace()
#        upsampled_timeseries_data[...].runningaverage(16*7, hWEIGHTS.GAUSSIAN)
        timeseries_power[...].runningaverage(7,hWEIGHTS.GAUSSIAN) # NB. [...] put in, want to ensure average per antenna... (AC)
#        timeseries_power *= 10 # parameter...
        maxima_power=trerun('FitMaxima',"Power",timeseries_power,pardict=par,doplot=Pause.doplot,refant=0,plotend=ndipoles,sampleinterval=sample_interval,peak_width=11,splineorder=3)
#        maxima_power=trerun('FitMaxima',"Power", upsampled_timeseries_data,pardict=par,doplot=Pause.doplot,refant=0,plotend=ndipoles,sampleinterval=sample_interval,peak_width=11*16,splineorder=3)
#        maxima_power.maxx /= 16.0
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
            statuslist.append("TOO FEW PULSES")
            # Write parameters to database
            write_results_to_database(results, polarization_info, xml_file=os.path.join(outputdir_with_subdirectories,"results.xml"))
            if polarization_info:
                polarization_info.alt_status = "BAD"
                polarization_info.write(recursive=False, parameters=False)
            if dump_html:
                finish_file()
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
        crosscorr=trerun('CrossCorrelateAntennas',"crosscorr",timeseries_data=timeseries_data_cut_to_pulse,pardict=par,oversamplefactor=10)

        #And determine the relative offsets between them
        maxima_cc=trerun('FitMaxima',"Lags",crosscorr.crosscorr_data,pardict=par,doplot=Pause.doplot,plotend=5,sampleinterval=sample_interval/crosscorr.oversamplefactor,peak_width=11,splineorder=3,refant=pulses_refant)

        Pause(name="pulse-maxima-crosscorr")

        #The actual delays are those from the (rounded) peak locations plus the delta from the cross correlation
        time_lags = sample_interval * timeseries_data_cut_pulse_width/2 + (timeseries_data_cut_to_pulse_delays.vec()+maxima_cc.lags) + sample_interval * np.modf(maxima_power.maxx[pulses_refant])[0]
        # reference antenna has maxima_cc.lags = 0 by def. Offsets to this, in integer samples, are in timeseries_data_cut_to_pulse_delays. The subsample part w.r.t. the ref-ant is in maxima_cc.lags.
        # The sub-sample part of the reference antenna time has to be included as well, using np.modf (takes fractional part).
        # Absolute times of arrival, using reference antenna, from the latest integer Unix / UTC second.
        # i.e. in seconds between 0.0 and 1.0 excl.
        absolute_arrivaltime = sample_interval * (tbb_samplenumber + block_number * blocksize + pulse.start) + time_lags

        print "Time lag [ns]: ", time_lags
        print " "
        ########################################################################
        #Direction fitting based on time lags
        ########################################################################
        #Now fit the direction and iterate over cable delays to get a stable
        #solution

        #cabledelays.negate()
#        delays=hArray(copy=cabledelays)
        delays = hArray(dimensions=cabledelays, fill=0.0) # no cable delay propagation anymore! Has been applied.
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

        # perform Triangle fit
        direction=trerun("DirectionFitTriangles","direction",pardict=par,positions=good_pulse_antenna_positions,timelags=good_pulse_lags, cabledelays=good_pulse_cabledelays, verbose=True,doplot=True)
          # put in zeros for 'cable delays' as they are fitted and put into the same array...
        if direction.ngood == 0:
            statuslist.append("NO GOOD TRIANGLES FOUND")
            # Write parameters to database
            write_results_to_database(results, polarization_info, xml_file=os.path.join(outputdir_with_subdirectories,"results.xml"))
            if polarization_info:
                polarization_info.alt_status = "BAD"
                polarization_info.write(recursive=False, parameters=False)
            if dump_html:
                finish_file()
            continue

        print "========================================================================"
        print "Triangle Fit Az/EL -> ", direction.meandirection_azel_deg,"deg"
        print " "
        print "Delays =",direction.total_delays * 1e9

        print "#DirectionFitTriangles: delta delays =",direction.delta_delays_mean_history[0],"+/-",direction.delta_delays_rms_history[0]

        (direction.total_delays*1e9).plot(xvalues=good_antennas_IDs[antennas_with_strong_pulses],linestyle="None",marker="x")
        (direction.delays_history*1e9)[1].plot(clf=False,xvalues=good_antennas_IDs[antennas_with_strong_pulses],linestyle="None",marker="o")
        (cabledelays*1e9).plot(clf=False,xlabel="Antenna",ylabel="Delay [ns]",xvalues=good_antennas_IDs,title="Fitted cable delays",legend=(["fitted delay","1st step","cable delay"]))
            # NB! What does this return? Really fitted cable delays?
        Pause(name="fitted-cable-delays")

        scrt = hArray(copy=direction.residual_delays) # NB! Need to copy the hArray
        # otherwise, the original array will get modified by scrt.abs().

        scrt_numpy = scrt.toNumpy()

        # remove > k-sigma outliers, then process again
        # one outlier can push the mean upwards so all delays get > maximum_allowed_delay... Therefore the k-sigma is needed.
        k = 3.0
        good_delays = scrt_numpy
        spread = np.std(good_delays)

        nremoved = 1
        delay_outliers = 0
        while (nremoved > 0 and delay_outliers < max_outliers):
            nbefore = len(good_delays)
            good_delays = good_delays[np.abs(good_delays - np.mean(good_delays)) < k * spread]
            nremoved = nbefore - len(good_delays)
            spread = np.std(good_delays)
            delay_outliers = abs(len(good_delays) - len(scrt_numpy))
            print "delay fitting ... removed",nremoved,"outliers"

        delay_quality_error = spread / maximum_allowed_delay

        # also count # outliers, impose maximum
        print "#Delay Quality Error:",delay_quality_error
        print "#Delay Outlier Count:",delay_outliers

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
        final_cable_delays=final_residual_delays # +cabledelays

        # include average cable delays (as in cable delay database) in time lags per pulse
        time_lags = hArray(time_lags)
#        time_lags -= cabledelays

        #plotdirection=trerun("PlotDirectionTriangles","plotdirection",pardict=par,centers=direction.centers,positions=direction.positions,directions=direction.directions,title="Sub-Beam directions")
        #Pause(name="sub-beam-directions")

        trerun("PlotAntennaLayout","Delays",pardict=par,positions=good_pulse_antenna_positions,colors=final_residual_delays[antennas_with_strong_pulses],sizes=100,names=good_antennas_IDs[antennas_with_strong_pulses],title="Delay errors in station",plotlegend=True)

        if options.imager:
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
        bf=trerun("BeamFormer2","bf",data=pulse.timeseries_data_cut,pardict=par,maxnantennas=ndipoles,antpos=antenna_positions,FarField=True,sample_interval=sample_interval,pointings=rf.makeAZELRDictGrid(*(direction.meandirection_azel+(10000,)),nx=3,ny=3), calc_timeseries=True,doplot=2 if Pause.doplot else False,doabs=True,smooth_width=5,plotspec=False,verbose=False,store_spectrum=options.full_output)
            # removed cabledelays = final_cable_delays in beamformer (AC)
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

        beamformed=trerun("BeamFormer2","beamformed",data=pulse.timeseries_data,pardict=par,maxnantennas=ndipoles,antpos=antenna_positions,FarField=True,sample_interval=sample_interval,pointings=rf.makeAZELRDictGrid(*(direction.meandirection_azel+(10000,)),nx=1,ny=1), calc_timeseries=False,doabs=False,smooth_width=0,doplot=False,plotspec=False,verbose=False,store_spectrum=options.full_output)
        print "Beamformer2...done"
            # removed cabledelays = final_cable_delays in beamformer (AC)
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

        # Check if found maximum is outside valid range
        maximum = int(floor(beam_maxima.maxx.val()))
        if maximum < 0 or maximum >= blocksize:
            print "FITTED MAXIMUM OUTSIDE RANGE"
            statuslist.append("FITTED MAXIMUM OUTSIDE RANGE")
            # Write parameters to database
            write_results_to_database(results, polarization_info, xml_file=os.path.join(outputdir_with_subdirectories,"results.xml"))
            if polarization_info:
                polarization_info.alt_status = "BAD"
                polarization_info.write(recursive=False, parameters=False)
            if dump_html:
                finish_file()
            continue

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
        pulse_direction_planewave = directionPlaneWave.meandirection_azel_deg

        # gather checksums if present
        if len(checksums) > 0:
            checksumArray = hArray(checksums)
            masterChecksum = checksumArray.checksum()
            s = ''
            for item in checksums:
                s += item + '\n'
            results.update(dict(checksums = s, masterChecksum = masterChecksum))
            print '*** Checksums: '
            for item in checksums:
                print item
            print '*** Master checksum = %s' % masterChecksum


        if delay_quality_error >= 1:
            final_status = "BAD"
        elif delay_outliers > max_outliers:
            final_status = "GOOD BUT TOO MANY OUTLIERS (" + str(delay_outliers) + ")"
        else:
            final_status = "GOOD"
        statuslist.append(final_status)

        results.update(dict(
            status="/".join(statuslist),
            antennas_flagged_delays=list(flagged_delays.vec()),
            antennas_final_cable_delays=list(final_cable_delays.vec()), # include metadata-cabledelays again here?? (AC)
            antennas_residual_cable_delays = list(final_residual_delays.vec()),
            antennas_with_peaks=antennas_with_peaks,
            delay_quality_error=delay_quality_error,
            pulse_location=beam_maxima.maxx.val(),
            pulses_strength=list(pulses_strength.vec()),
            pulses_maxima_x=list(maxima_power.maxx),
            pulses_maxima_y=list(maxima_power.maxy),
            pulses_timelags_ns=list(time_lags),
            pulses_absolute_arrivaltime = list(absolute_arrivaltime),
            pulse_start_sample=pulse.start,
            pulse_end_sample=pulse.end,
            pulse_time_ms=pulse_time_ms,
            pulse_height=pulse_height,
            pulse_height_incoherent=pulse_height_incoherent,
            pulse_normalized_height=pulse_normalized_height,
            pulse_direction=pulse_direction,
            pulse_direction_planewave = pulse_direction_planewave,
            pulse_direction_delta_delays_start=direction.delta_delays_mean_history[0],
            pulse_direction_delta_delays_final=direction.delta_delays_mean_history[-1]
            ))

        event.par.quality=quality
        event.par.timeseries_data=pulse.timeseries_data_cut
        event.par.results=results

        if options.full_output:
            event.write(result_file)

        # Writing results
        write_results_to_database(results, polarization_info, xml_file=os.path.join(outputdir_with_subdirectories,"results.xml"))
        if polarization_info:
            polarization_info.alt_status = final_status
            polarization_info.write(recursive=False, parameters=False)

        if dump_html:
            finish_file()


def process_event(event_id=-1):
    """Process an event given its identifier (*event_id*).

    **Properties**

    ==========  ============================================
    Parameter   Description
    ==========  ============================================
    *event_id*  Event ID of the event to process.
    ==========  ============================================
    """
    if event_id > 0:
        event_info = db_manager.getEvent(id=event_id)
        event_info.alt_status = "PROCESSING"
        event_info.write(recursive=False, parameters=False)
        for datafile_info in event_info.datafiles:
            filename_full = os.path.join(datadir, datafile_info.filename)
            process_datafile(filename_full, datafile_info)
        if event_info.is_cr_found(alt_status=True):
            event_info.alt_status = "CR_FOUND"
        else:
            event_info.alt_status = "CR_NOT_FOUND"
        event_info.write(recursive=True, parameters=True)

        event_info.summary()
    else:
        raise ValueError("Invalid event_id provided")


def write_results_to_database(results=None, polarization_info=None, xml_file=None):
    """Write results to the database.

    **Properties**

    ===================  ================================================
    parameter            Description
    ===================  ================================================
    *results*            Dictionary with analysis results.
    *polarization_info*  Object containing polarization information.
    ===================  ================================================
    """
    if results:
        if xml_file:
            xmldict.dump(xml_file, results)
        if polarization_info:
            for key in results.keys():
                polarization_info[key.lower()] = results[key]
            polarization_info.write(recursive=False, parameters=False)


########################################################################
# Loop over all files and polarizations
########################################################################


if filefilter:
    print "Running over all files (database will not be used)..."
    print filefilter
    filenames = args
    for filename in filenames:
        full_filename = os.path.join(datadir, filename)
        process_datafile(full_filename)
else:
    print "Running over event_id = %d..." %(event_id)

    if event_id > 0:
        process_event(event_id)
    else:
        raise ValueError("Invalid event_id provided")


plt.ion()
