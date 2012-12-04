"""
--- Modifying this test script to work as a Task (with sensible parameters and all). ---

Test script to gather processed timeseries data from results directory of a given event.
Reprocess cross correlation using MultiTBBData datareader, which reads and bundles data from all
datafiles for a given event.

Reprocess cross correlation over all stations (polarisations separately).
Obtain arrival times; apply calibration delays and inter-station delays.

Get wavefront shape and fit curvature using point-source approximation.

later?: Reprocess cross correlation over 0/1 polarisation inside each station.
======================

Created by Arthur Corstanje, Apr. 2012
Wavefront Task created by Arthur Corstanje, Nov. 2012
"""

"""
Module documentation
====================

"""

from pycrtools.tasks import Task
import numpy as np
import pycrtools as cr
import matplotlib.pyplot as plt

from pycrtools import srcfind as sf
import os
# from pycrtools.tasks.shortcuts import *
from pycrtools import metadata as md

import numpy as np
from scipy.optimize import fmin
import matplotlib.pyplot as plt
from datetime import datetime
from pycrtools import xmldict
from pycrtools import lora

deg2rad = np.pi / 180


def mseMinimizer(direction, pos, times, outlierThreshold=0, allowOutlierCount=0):
    # Quality function for simplex search
    az = direction[0]
    el = direction[1]
    R = 1000.0 / direction[2]  # using 1/R as parameter - curvature dependence is on 1/R to lowest order; will work better in simplex search.
    mse = sf.mseWithDistance(az, el, R, pos, times, outlierThreshold, allowOutlierCount)
    print 'Evaluated direction: az = %f, el = %f, R = %f: MSE = %f' % (az / deg2rad, el / deg2rad, R, mse)

    return mse

# REMOVE:
eventdir = '/Users/acorstanje/triggering/CR/results_withcaltables/VHECR_LORA-20110714T174749.986Z'
# eventdir = '/Users/acorstanje/triggering/CR/testRFIevent'
# eventdir = '/data/VHECR/Radiotriggered/results_mailed/L42358_D2012'

def pulseTimeFromLORA(lora_dir, datafile):
    print "---> Reading pulse timing information from LORA"
    tbb_starttime = datafile["TIME"][0]
    tbb_samplenumber = max(datafile["SAMPLE_NUMBER"])
    blocksize = datafile["BLOCKSIZE"]
    
    lora_event_info = 0 # to check with 'if lora_event_info:'
    lora_logfile = os.path.join(lora_dir,"LORAtime4")
    if os.path.exists(lora_logfile):
        (tbb_starttime_sec,tbb_starttime_nsec)=lora.nsecFromSec(tbb_starttime,logfile=lora_logfile)

        if tbb_starttime_sec:
            try:
                (block_number_lora,sample_number_lora)=lora.loraTimestampToBlocknumber(tbb_starttime_sec,tbb_starttime_nsec,tbb_starttime,tbb_samplenumber,blocksize=blocksize)
            except ValueError:
                raise ValueError("#ERROR - LORA trigger information not found")
                
            print "---> Estimated block number from LORA: block =",block_number_lora,"sample =",sample_number_lora
            block_number = block_number_lora
            sample_number = sample_number_lora
            print "---> From LORA: block =",block_number,"sample =",sample_number
        else:
            raise ValueError("LORA trigger not found in database")
            # Write parameters to database
            # print "WARNING: LORA logfile found but no info for this event time in LORAtime4 file!"
    else:
        raise ValueError( "WARNING: No LORA logfile found - " + str(lora_logfile) )

    return (block_number, sample_number)
    
    
def gatherresults(eventdir):
    """
    Gather results from 1st pipeline analysis if status is 'good', group into a list of dicts containing
    - station name
    - results pol 0
    - results pol 1
    - data filename
    """
#    results = {}

    outputstring = ""  # make output string summarizing outlier counts etc.

    stations = []

    print "Processing event in directory:", eventdir
    if os.path.isdir(os.path.join(eventdir, "pol0")) or os.path.isdir(os.path.join(eventdir, "pol1")):
        datadirs = cr.listFiles(os.path.join(os.path.join(eventdir, "pol?"), "*"))

    for datadir in datadirs:
        if not os.path.isfile(os.path.join(datadir, "results.xml")):
            continue  # doesn't matter if 'datadir' isn't a directory...
        resfile = os.path.join(datadir, "results.xml")
        try:
            res = xmldict.load(os.path.join(datadir, "results.xml"))
        except Exception as e:
            print "File skipped, NaN found or whatever!"
            print "Error = ", e
            continue

        print 'Antenna set: %s ' % res["ANTENNA_SET"]
        # check result status
        noutliers = res["delay_outliers"] if "delay_outliers" in res.keys() else 0
        print '# outliers = %d' % noutliers
        if res["delay_quality_error"] >= 1 or noutliers > 5:  # have status criterion in pipeline...
            print 'File status not OK, dir = %s, delay quality = %s, # outliers = %s' % (datadir, res["delay_quality_error"], noutliers)
            continue
        # get station name
        firstantenna = int(res["antennas"][0])
        thisStationName = md.idToStationName(firstantenna / 1000000)
        fromlist = [x for x in stations if x["stationName"] == thisStationName]
        if len(fromlist) > 0:
            thisDict = fromlist[0]
        else:
            thisDict = dict(stationName=thisStationName)
            stations.append(thisDict)
        pol = res["polarization"]
        thisDict[pol] = res  # adds the key 0 or 1 as number
        thisDict.update(filename=os.path.join(res["filedir"], res["FILENAME"]))

        timeseriesFilename = os.path.splitext(res["FILENAME"])[0] + '-pol' + str(pol) + '-calibrated-timeseries-cut.pcr'
        thisTimeseries = cr.hArrayRead(os.path.join(datadir, timeseriesFilename))
        thisDict[pol].update(timeseries=thisTimeseries)

    return stations

#        positions.extend(res["antenna_positions_array_XYZ_m"]) # in flat list NB! All antennas? Same order as for timeseries data?

        # timeseries inlezen, orig of calibrated? Zie evt cr_physics.py.
        # ....


class Wavefront(Task):
    """
    Obtain the shape of the wavefront of a CR pulse.
    Re-calculate all relative arrival times using cross correlation on the full event dataset.
    Uses MultiTBBData to open all files, applying clock offset shifts etc.

    Subtract best-fitting plane wave solution in order to obtain second-order wavefront shape (i.e. curvature).

    Compare with point-source approximation, get best-fitting point source and look at residuals.
    """

    parameters = dict(
        filefilter = dict(default=None, doc="File filter for multiple data files in one event, e.g. '/my/data/dir/L45472_D20120206T030115.786Z*.h5' "),
        filelist = dict(default=None, doc="List of filenames in one event. "),
        f = dict( default = None,
            doc = "File object. Blocksize, polarisation + antenna selection etc. are taken from the given file object, and should have been set before calling this Task." ),
        loradir = dict( default = None, doc = "Directory with LORA trigger data. Required for obtaining pulse location in the data. "),
        interStationDelays = dict( default=None, doc="Inter-station delays as a correction on current LOFAR clock offsets. To be obtained e.g. from the CalibrateFM Task. Assumed to be in alphabetic order in the station name e.g. CS002, CS003, ... If not given, zero correction will be assumed." ),
        blocksize = dict ( default = 65536, doc = "Blocksize." ),
        nantennas = dict( default = lambda self : self.f["NOF_SELECTED_DATASETS"],
            doc = "Number of selected antennas." ),
#        nofblocks = dict( default = -1, doc = "Number of data blocks to process. Set to -1 for entire file." ),
        pulse_block = dict( default = 0, doc = "Block nr. where the pulse is" ),
        refant = dict(default = None, doc = "Optional parameter to set reference antenna number."),
        pol = dict(default = 0, doc = "Polarization. Only used if no file object is given."),
#        timeseries_data = dict( default = lambda self : cr.hArray(float, dimensions=(self.nantennas, self.blocksize)),
#            doc = "Timeseries data." ),
        #fftwplan = dict( default = lambda self : cr.FFTWPlanManyDftR2c(self.blocksize, self.nantennas, 1, 1, 1, 1, cr.fftw_flags.MEASURE),
        #    doc = "Forward plan for FFTW, by default use a plan that works over all antennas at once and is measured for speed because this is applied many times." ),
        arrivalTimes = dict( default = None, doc = "Pulse arrival times per antenna, for antennas in self.f['SELECTED_DIPOLES']", output = True),
        pointsourceArrivalTimes = dict( default = None, doc = "Arrival times from best-fit point source approximation", output = True),
        planewaveArrivalTimes = dict( default = None, doc = "Arrival times from best-fit plane-wave approximation.", output = True),  
        fitPlaneWave = dict( default = None, doc = "Fit results (az, el, mse) for planar fit.", output = True ),
        fitPointSource = dict( default = None, doc = "Fit results (az, el, R, mse) for point source simplex search.", output = True ),
        bad_antennas = dict( default = [],
            doc = "Antennas found to be bad.", output = True ),
        good_antennas = dict( default = lambda self : [name for name in self.f["SELECTED_DIPOLES"] if name not in self.bad_antennas],
            doc = "Antennas found to be good.", output = True ),
        save_plots = dict( default = False,
            doc = "Store plots" ),
        plot_prefix = dict( default = "",
            doc = "Prefix for plots" ),
        plotlist = dict( default = [],
            doc = "List of plots" ),
        plot_antennas = dict( default = lambda self : range(self.nantennas),
            doc = "Antennas to create plots for." ),
        verbose = dict( default = True, doc = "Verbose output."),
    )

    def run(self):
        """Run the task.
        """
        stations = gatherresults(eventdir)
        # now accumulate arrays with:
        # - all timeseries data for all stations
        # - all corresponding antenna ids and positions
        # - all calibration delays, and (station) clock offsets for those ids
        # - array of all reference offsets per antenna id. (starting point of timeseries array)

        #pol = 1  # later do both together
        if not self.f and not self.filefilter and not self.filelist:
            raise RuntimeError("Give a file object or a filefilter or a filelist")
        if not self.f:
            #filefilter = '/Users/acorstanje/triggering/CR/*.986Z*.h5'
            if not self.filelist:
                self.filelist = cr.listFiles(self.filefilter)
            superterpStations = ["CS002", "CS003", "CS004", "CS005", "CS006", "CS007", "CS021"]
            if len(self.filelist) == 1:
                print 'There is only 1 file'
                self.filelist = self.filelist[0]
            else:  # sort files on station ID
                sortedlist = []
                for station in superterpStations:
                    thisStationsFile = [filename for filename in self.filelist if station in filename]
                    if len(thisStationsFile) > 0:
                        sortedlist.append(thisStationsFile[0])
                self.filelist = sortedlist
            print '[Wavefront] Processing files: '
            print self.filelist
            self.f = cr.open(self.filelist, blocksize=self.blocksize)
            selected_dipoles = [x for x in self.f["DIPOLE_NAMES"] if int(x) % 2 == self.pol]
            self.f["SELECTED_DIPOLES"] = selected_dipoles

        self.blocksize = self.f["BLOCKSIZE"]
        
        # Get the pulse location in the data from LORA timing: block and sample number
        (block, pulse_samplenr) = pulseTimeFromLORA(self.loradir, self.f)
                    
        firstDataset = stations[0][self.pol]  # obtained from cr_event results (1st stage pipeline), used to locate pulse.
        # assume file shifts are of the order ~ 200 samples << blocksize for cut-out timeseries
#        block = firstDataset["BLOCK"]
#        blocksize = firstDataset["BLOCKSIZE"] # REMOVE
        # tbb_samplenr = thisDataset["SAMPLE_NUMBER"]
#        pulse_samplenr = firstDataset["pulse_start_sample"]
        refant = firstDataset["pulses_refant"]
        print 'From cr-event results: block = %d, sample nr = %d' % (block, pulse_samplenr)
        print 'refant = %d' % refant
          # need to cut out timeseries around pulse
        # select only even/odd antennas according to 'pol'

        antennaPositions = self.f["ANTENNA_POSITIONS"]

        timeseries = self.f.empty("TIMESERIES_DATA")
        # get timeseries data with pulse, then cut out a region around the pulse.
        cutoutSize = 1024
        self.f.getTimeseriesData(timeseries, block = block)
        nofChannels = timeseries.shape()[0]
        cutoutTimeseries = cr.hArray(float, dimensions=[nofChannels, cutoutSize])
        start = pulse_samplenr - cutoutSize / 2
        end = pulse_samplenr + cutoutSize / 2
        cutoutTimeseries[...].copy(timeseries[..., start:end])
        
        # Get reference antenna, take the one with the highest maximum.
        y = cutoutTimeseries.toNumpy()
        refant = int(np.argmax(np.max(y, axis=1)))
        print 'Taking channel %d as reference antenna' % refant
        
        #import pdb; pdb.set_trace()
        plt.plot(y[0])
        plt.plot(y[140])

        sample_interval = 5.0e-9 # hardcoded...
        # is the index of the ref antenna also in the full list of antids / antpos

        # now cross correlate all channels in full_timeseries, get relative times
        crosscorr = cr.trerun('CrossCorrelateAntennas', "crosscorr", cutoutTimeseries, oversamplefactor=64)

        # And determine the relative offsets between them
        maxima_cc = cr.trerun('FitMaxima', "Lags", crosscorr.crosscorr_data, doplot=True, plotend=5, sampleinterval=sample_interval / crosscorr.oversamplefactor, peak_width=11, splineorder=3, refant=refant)

        # print startTimes
        # print maxima_cc.lags

        # plot lags, plot flagged lags from a k-sigma criterion on the crosscorr maximum

        plt.figure()
        cr.hArray(maxima_cc.lags).plot()

        # Plot arrival times, do plane-wave fit, plot residuals wrt plane wave
        arrivaltime = cr.hArray(maxima_cc.lags)
        times = arrivaltime.toNumpy()
        positions = antennaPositions.toNumpy().ravel()

        # HACK out outlying values
        arrivaltime[206] = 0.0

        # Apply calibration delays (per antenna)
        arrivaltime -= cr.hArray(self.f["DIPOLE_CALIBRATION_DELAY"])
        # Apply sub-sample inter-station clock offsets (LOFAR)
        subsampleOffsets = self.f["SUBSAMPLE_CLOCK_OFFSET"] # numpy array!
        # Apply inter-station delays from RFIlines Task
        # Assuming ordering CS002, 3, 4, 5, 7 for this event (sorted alphabetically)
        # import pdb; pdb.set_trace()
        # Hack values in here
        if not type(self.interStationDelays) == type(None): # cannot do if self.interStationDelays, apparently...
            subsampleOffsets -= self.interStationDelays
            
#        subsampleOffsets[1] -= 0.11e-9
#        subsampleOffsets[2] -= -1.30e-9
#        subsampleOffsets[3] -= -1.32e-9
#        subsampleOffsets[4] -= 0.71e-9

        stationList = self.f["STATION_LIST"]
        print stationList
        stationStartIndex = self.f["STATION_STARTINDEX"]
        for i in range(len(subsampleOffsets)):
            arrivaltime[stationStartIndex[i]:stationStartIndex[i + 1]] -= subsampleOffsets[i]
            # Sign + or - ???

        # plt.figure()
        # arrivaltime.plot()
        # plt.title('Arrival times, matched with offsets per station (check!)')
        times = arrivaltime.toNumpy()
        positions = antennaPositions.toNumpy().ravel()

        # now make footprint plot of all arrival times
        loradir = '/Users/acorstanje/triggering/CR/LORA'
        # first show originally derived arrival times
        fptask_orig = cr.trerun("plotfootprint", "0", colormap='jet', filefilter=eventdir, loradir=loradir, plotlora=False, plotlorashower=False, pol=self.pol)  # no parameters set...
        plt.title('Footprint using original cr_event arrival times')
        plt.figure()
        # now our arrival times and antenna positions

        fptask = cr.trerun("plotfootprint", "1", colormap='jet', filefilter=eventdir, positions=antennaPositions, arrivaltime=1.0e9 * arrivaltime, loradir=loradir, plotlora=False, plotlorashower=False, pol=self.pol)  
        
        plt.title('Footprint using crosscorrelated arrival times')
        delta = arrivaltime - 1.0e-9 * fptask_orig.arrivaltime
        delta -= delta.mean()
        plt.figure()
        fptask_delta = cr.trerun("plotfootprint", "2", colormap='jet', filefilter=eventdir, positions=antennaPositions, arrivaltime=1.0e9 * delta, loradir=loradir, plotlora=False, plotlorashower=False, pol=self.pol)  # no parameters set...
        plt.title('Footprint of difference between cr_event and full-cc method')
        plt.figure()
        delta.plot()
        plt.title('Difference between plotfootprint / cr_event arrival times\nand full crosscorrelation times')
        import pdb; pdb.set_trace()
        # Do plane-wave direction fit on full arrivaltimes
        # Fit pulse direction
        print 'Do plane wave fit on full arrival times (cross correlations here)...'
        direction_fit_plane_wave = cr.trun("DirectionFitPlaneWave", positions=antennaPositions, timelags=arrivaltime, verbose=True)

        plt.figure()
        direction_fit_plane_wave.residual_delays.plot()
        plt.title('Residual delays after plane wave fit')

        print 'Do plane wave fit on stored arrival times, from plotfootprint...'
        direction_fit_plane_wave_orig = cr.trun("DirectionFitPlaneWave", positions=fptask_orig.positions, timelags=1.0e-9 * fptask_orig.arrivaltime, verbose=True)

        plt.figure()
        direction_fit_plane_wave_orig.residual_delays.plot()
        plt.title('Residual delays after plane wave fit\n to plotfootprint timelags')

        residu = direction_fit_plane_wave.residual_delays.toNumpy()
        bestfitDelays = sf.timeDelaysFromDirection(positions, direction_fit_plane_wave.meandirection_azel)
        self.planewaveArrivalTimes = bestfitDelays
        # import pdb; pdb.set_trace()
        residu[np.where(abs(residu) > 100e-9)] = 0.0  # hack
        residu -= residu.mean()
        residu[np.where(abs(residu) > 15e-9)] = np.float('nan')  # hack

        residu[np.argmax(residu)] = 0.0
        residu[np.argmin(residu)] = 0.0
        residu -= min(residu)

        plt.figure()
        # now the good one: difference between measured arrival times and plane wave fit!
        fptask_delta = cr.trerun("plotfootprint", "3", colormap='jet', filefilter=eventdir, positions=antennaPositions, arrivaltime=cr.hArray(1.0e9 * residu), power=140, loradir=loradir, plotlora=False, plotlorashower=False, pol=self.pol)  # no parameters set...
        plt.title('Footprint of residual delays w.r.t. planewave fit')

        # Simplex fit point source...
        (az, el) = direction_fit_plane_wave.meandirection_azel  # check
        startPosition = (az, el, 1.0)  # 1.0 means R = 1000.0 ...
        print 'Doing simplex search for minimum MSE...'
        optimum = fmin(mseMinimizer, startPosition, (positions, times, 0, 4), xtol=1e-5, ftol=1e-5, full_output=1)
        # raw_input('Done simplex search.')

        simplexDirection = optimum[0]
        simplexMSE = optimum[1]
        simplexDirection[2] = 1000.0 / simplexDirection[2]  # invert again to get R
        (simplexAz, simplexEl, simplexR) = simplexDirection

        print '-----'
        print 'Simplex search: '
        print 'Best MSE: %f; best direction = (%f, %f, %f)' % (simplexMSE, simplexAz / deg2rad, simplexEl / deg2rad, simplexR)
        msePlanar = sf.mseWithDistance(simplexAz, simplexEl, 1.0e6, positions, times, allowOutlierCount=4)
        print 'Best MSE for R = 10^6 (approx. planar): %f' % msePlanar
        print '-----'
        # get the calculated delays according to this plane wave
        # simplexDirection[2] = 4000.0
        bestfitDelays = sf.timeDelaysFromDirectionAndDistance(positions, simplexDirection)
        self.pointsourceArrivalTimes = bestfitDelays
        
        residu = times - bestfitDelays
        # import pdb; pdb.set_trace()
        # residu[np.where(abs(residu) > 100e-9)] = 0.0 # hack
        residu -= residu.mean()
        # residu[np.where(abs(residu) > 15e-9)] = np.float('nan') # hack

        residu[np.argmax(residu)] = 0.0
        residu[np.argmin(residu)] = 0.0
        residu[np.argmax(residu)] = 0.0
        residu[np.argmin(residu)] = 0.0
        # residu -= min(residu)

        plt.figure()
        # now the good one: difference between measured arrival times and plane wave fit!
        fptask_delta = cr.trerun("plotfootprint", "4", colormap='jet', filefilter=eventdir, positions=antennaPositions, arrivaltime=cr.hArray(1.0e9 * residu), power=140, loradir=loradir, plotlora=False, plotlorashower=False, pol=self.pol)  # no parameters set...
        plt.title('Footprint of residual delays w.r.t. point source fit')

        """
        see test script testCrosscorr_MultiTBB.py for commented-out stuff

        """
