"""
Get delays between polarizations from results.xml files output by the pipeline.
======================

Created by Arthur Corstanje, Mar 2012
"""

import pycrtools as cr
import pycrtools.tasks as tasks
import os
from pycrtools.tasks.shortcuts import *
from pycrtools import metadata as md
from pycrtools import xmldict
import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime

#def timeStringNow():
#    now = datetime.now()
#    return now.strftime("%Y-%m-%d_%H%M%S")

eventdir = '/Users/acorstanje/triggering/CR/results_withcaltables/VHECR_LORA-20111228T200122.223Z'
#eventdir = '/data/VHECR/Radiotriggered/results/L42358_D20120110T204435.095Z_CS005_R000_tbb'
antennaset = 'LBA_OUTER'

def gatherresults(eventdir, antennaSet):
    """
    Gather fitted cable delay results from all 'results.xml' files in the subdirectories of 'topdir'.
    """
#    if not topdir:
#        return None
    
    results = {}
    
#    filelist=cr.listFiles(os.path.join(topdir,'*'))
     
#    eventlist=[] # get all events for this top-dir
#    for file in filelist: # can this be done shorter / in listFiles?
#        if os.path.isdir(file):
#            eventlist.extend([file])
            
    antid = []
    outputstring = "" # make output string summarizing outlier counts etc.
#    for eventdir in eventlist:        
    print "Processing event in directory: ",eventdir
    if os.path.isdir(os.path.join(eventdir,"pol0")) and os.path.isdir(os.path.join(eventdir, "pol1")):
        datadirs=cr.listFiles(os.path.join(os.path.join(eventdir,"pol?"),"*"))
    else:
        print 'both pol0 and pol1 are needed, not there...'
        return 
    thisEventsArrivalTimes = dict()
    for datadir in datadirs:
        if not os.path.isfile(os.path.join(datadir,"results.xml")):
            continue # doesn't matter if 'datadir' isn't a directory...
        resfile = os.path.join(datadir,"results.xml")
        res = {}
        try:
            res = xmldict.load(os.path.join(datadir,"results.xml"))
        except NameError:
            print 'Warning: NAN found in results.py (Name error)! Skipping dir %s ' % datadir
            continue
        #res = res["results"]
        if res["ANTENNA_SET"] != antennaSet:
            print 'Skipping file in dir %s, wrong antenna set %s' % (datadir, res["ANTENNA_SET"])
            continue
        # check result status
#        if res["status"] != 'OK':
        # A 'status' keyword would be nice... Number of delay outliers above (say) 10 ns is in 'delay_outliers' key.
        noutliers = res["delay_outliers"] if "delay_outliers" in res.keys() else 0 
        print '# outliers = %d' % noutliers
        if res["delay_quality_error"] >= 1 or noutliers > 5: # have status criterion in pipeline...
            print 'File status not OK, dir = %s, delay quality = %s, # outliers = %s' % (datadir, res["delay_quality_error"], noutliers)
            continue
        # read in antenna positions from file
#            positions.extend(res["antenna_positions_array_XYZ_m"]) # in flat list
        nof_new_antids = len([str(int(v)) for v in res["antennas"]])
#            nof_new_cabledelays = len(res["antennas_residual_cable_delays_planewave"])
#            nof_new_triangledelays = len(res["antennas_residual_cable_delays"])
        
       # print '*** New ant-ids: %d' % nof_new_antids
       # print '*** New cable delays: %d' % nof_new_cabledelays
       # print '*** New triangle delays: %d' % nof_new_triangledelays
#            if nof_new_cabledelays != nof_new_antids:
#                print '*** ERROR: WRONG NUMBER OF ANTENNAS!'
        theseAntIDs = [str(int(v)) for v in res["antennas"].values()]
        if "pulses_absolute_time_of_arrival" in res.keys():
            theseArrivalTimes = res["pulses_absolute_time_of_arrival"]
            print 'absolute toa for this file: %s' % datadir 
            print 'block: %d' % res["BLOCK"]
            print 'snr  : %d' % res["SAMPLE_NUMBER"]
#            if 'CS003' in datadir:
#                import pdb; pdb.set_trace()
        else:
            theseMaximaX = res["pulses_maxima_x"]
            refant = res["pulses_refant"]
            fractionalSampleOffset = np.modf(theseMaximaX[refant])[0]
            theseArrivalTimes = res["SAMPLE_INTERVAL"] * (res["SAMPLE_NUMBER"] + res["BLOCK"] * res["BLOCKSIZE"] + res["pulse_start_sample"] + fractionalSampleOffset) + np.array(res["pulses_timelags_ns"])
            
        if len(theseAntIDs) != len(theseArrivalTimes):
            print '*** ERROR: lengths dont match between arrival times and ant-ids!'
        antid.extend(theseAntIDs) # res["antennas"] is a dict!
        for i in range(len(theseAntIDs)):
            id = theseAntIDs[i]
            thisEventsArrivalTimes[str(id)] = theseArrivalTimes[i] # put into dict for quick reference
        
        
#            timelist = [res["TIME"]] * len(res["antennas"])
#            timestamps.extend(timelist)
    
    # finish data for this event
    antennasPerStationID = dict()
    for id in antid: 
        thisStationID = int(id) / 1000000
        if not thisStationID in antennasPerStationID.keys():
            antennasPerStationID[thisStationID] = []
        antennasPerStationID[thisStationID].append(id)
    
    for station in antennasPerStationID:
        thisStationName = md.idToStationName(station)
        # get time delays between polarizations and plot.
        y = [float('nan')] * 48
        x = range(48)
        theseEvenIDs = [id for id in antennasPerStationID[station] if int(id) % 2 == 0]
        theseRCUnumbers = [int(id) % 100 for id in antennasPerStationID[station]]
        for id in theseEvenIDs:
            thisRCU = int(id) % 100
            if thisRCU + 1 in theseRCUnumbers: # cannot assume id -> id+1 for corresponding odd pol.
                oddID = [loopid for loopid in antennasPerStationID[station] if int(loopid) % 100 == thisRCU + 1]
                # NB! [id for id in ...] destroys variable 'id'!
                oddID = oddID[0]
                y[thisRCU / 2] = thisEventsArrivalTimes[oddID] - thisEventsArrivalTimes[id]
        
        y = np.array(y) * 1.0e9
        plt.figure()
        plt.scatter(x, y, label = 'time differences')
        plt.ylabel('Time differences [ns]')
        plt.xlabel('Antenna number (RCU/2)')
        eventName = os.path.split(eventdir)[1].strip('VHECR_LORA-')
        plt.title('Time difference between odd and even polarization per antenna\nEvent '+ eventName + '   Station ' +thisStationName)
        plt.legend(loc='best')
                        

gatherresults(eventdir, antennaset)

