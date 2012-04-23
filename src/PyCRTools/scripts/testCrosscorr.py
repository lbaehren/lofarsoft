"""
Test script to gather processed timeseries data from results directory of a given event.
Reprocess cross correlation over all stations (polarisations separately).
Reprocess cross correlation over 0/1 polarisation inside each station.
======================

Created by Arthur Corstanje, Apr. 2012
"""

import pycrtools as cr
import pycrtools.tasks as tasks
import os
from pycrtools.tasks.shortcuts import *
from pycrtools import metadata as md
import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime
from pycrtools import xmldict

def timeStringNow():
    now = datetime.now()
    return now.strftime("%Y-%m-%d_%H%M%S")

eventdir = '/Users/acorstanje/triggering/CR/results_withcaltables/VHECR_LORA-20111228T200122.223Z'

def gatherresults(eventdir):
    """
    Gather results from 1st pipeline analysis if status is 'good', group into a list of dicts containing
    - station name
    - results pol 0
    - results pol 1
    - data filename
    """
#    results = {}
    
    outputstring = "" # make output string summarizing outlier counts etc.

    stations = []

    print "Processing event in directory:",eventdir
    if os.path.isdir(os.path.join(eventdir,"pol0")) or os.path.isdir(os.path.join(eventdir, "pol1")):
        datadirs=cr.listFiles(os.path.join(os.path.join(eventdir,"pol?"),"*"))

    for datadir in datadirs:
        if not os.path.isfile(os.path.join(datadir,"results.xml")):
            continue # doesn't matter if 'datadir' isn't a directory...
        resfile = os.path.join(datadir,"results.xml")
        try:
            res = xmldict.load(os.path.join(datadir,"results.xml"))
        except Exception as e:
            print "File skipped, NaN found or whatever!"
            print "Error = ", e
            continue

        print 'Antenna set: %s ' % res["ANTENNA_SET"]
        # check result status
        noutliers = res["delay_outliers"] if "delay_outliers" in res.keys() else 0 
        print '# outliers = %d' % noutliers
        if res["delay_quality_error"] >= 1 or noutliers > 5: # have status criterion in pipeline...
            print 'File status not OK, dir = %s, delay quality = %s, # outliers = %s' % (datadir, res["delay_quality_error"], noutliers)
            continue
        # get station name
        firstantenna = int(res["antennas"][0])
        thisStationName = md.idToStationName(firstantenna / 1000000)
        fromlist = [x for x in stations if x["stationName"] == thisStationName]
        if len(fromlist) > 0:
            thisDict = fromlist[0]
        else:
            thisDict = dict(stationName = thisStationName)
            stations.append(thisDict)
        pol = res["polarization"]
        thisDict[pol] = res # adds the key 0 or 1 as number
        thisDict.update(filename = os.path.join(res["filedir"], res["FILENAME"]))
        
        timeseriesFilename = os.path.splitext(res["FILENAME"])[0] + '-pol'+str(pol) + '-calibrated-timeseries-cut.pcr'
        thisTimeseries = cr.hArrayRead(os.path.join(datadir, timeseriesFilename))
        thisDict[pol].update(timeseries = thisTimeseries)
    
    return stations

#        positions.extend(res["antenna_positions_array_XYZ_m"]) # in flat list NB! All antennas? Same order as for timeseries data?
        
        # timeseries inlezen, orig of calibrated? Zie evt cr_physics.py.
        # ....    

def obtainvalue(par,key):
    """This function returns the value from a parameter dict or a default value if the key does not exist"""
    defaultvalues=dict(
        title=False,
        loracore=None,
        positions=None,
        antid=None,
        loradirection=None,
        names=False
    )

    if not par:
        if key in defaultvalues.keys():
            return defaultvalues[key]
        else: 
            print "please add ",key,"to defaults in cabledelays.py / obtainvalue "
            return None
    else:
        if key in par.keys():
            return par[key]
        else:
            if key in defaultvalues.keys():
                return defaultvalues[key]
            else: 
                print "please add ",key,"to defaults in cabledelays.py / obtainvalue "
                return None


stations = gatherresults(eventdir)
# now accumulate arrays with:
# - all timeseries data for all stations
# - all corresponding antenna ids and positions
# - all calibration delays, and (station) clock offsets for those ids
# - array of all reference offsets per antenna id. (starting point of timeseries array)

pol = 0 # later do both together

startTimes = []
nofantennas = 0
sample_interval = 0.0
for dataset in stations:
    thisDataset = dataset[pol]
    
    block = thisDataset["BLOCK"]
    blocksize = thisDataset["BLOCKSIZE"]
    tbb_samplenr = thisDataset["SAMPLE_NUMBER"]
    pulse_samplenr = thisDataset["pulse_start_sample"]
    sample_interval = thisDataset["SAMPLE_INTERVAL"]
    clock_correction = md.getClockCorrection(thisDataset["stationName"])
    nofantennas += len(thisDataset["antennas"])
#    pulse_end = thisDataset["pulse_end_sample"]
    timeOffset = sample_interval * (tbb_samplenr + block * blocksize + pulse_samplenr) + clock_correction
    
    startTimes.extend(timeOffset)

# accumulate ant-ids, ant-positions and timeseries data
antids = []
antpos = []
datalen = stations[0][0]["timeseries"].shape()[1] # and assume the same length for cut-timeseries for all others...
full_timeseries = hArray(float, dimensions = [len(antids), datalen])
row = 0
for dataset in stations: 
    antids.append(dataset[pol]["antennas"].values())
    antpos.append(dataset[pol]["antenna_positions_array_XYZ_m"])
    thisTimeseries = dataset[pol]["timeseries"]
    nofchannels = thisTimeseries.shape()[0]
    full_timeseries[row:row+nofchannels].copy(thisTimeseries) # check...
    row += nofchannels

# get a reference antenna with a strong pulse...
# for now, just station[0]'s reference antenna... May want to use overall-best pulse (highest SNR)
refant = station[0][pol]["pulses_refant"]
# is the index of the ref antenna also in the full list of antids / antpos

# now cross correlate all channels in full_timeseries, get relative times
crosscorr = trerun('CrossCorrelateAntennas', "crosscorr", full_timeseries,oversamplefactor=128)

#And determine the relative offsets between them
maxima_cc = trerun('FitMaxima', "Lags", crosscorr.crosscorr_data, doplot = True, plotend=5, sampleinterval = sample_interval / crosscorr.oversamplefactor, peak_width = 11, splineorder = 3, refant = refant)


#fftplan = FFTWPlanManyDftR2c(blocksize, 1, 1, 1, 1, 1, fftw_flags.ESTIMATE)
#hFFTWExecutePlan(fft_data[...], timeseries_data[...], fftplan)




