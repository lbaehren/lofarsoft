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
import pycrtools as cr
from pycrtools import srcfind as sf
from pycrtools import lora
import os
import numpy as np
from scipy.optimize import fmin
import matplotlib.pyplot as plt

deg2rad = np.pi / 180

def mseMinimizer(direction, pos, times, outlierThreshold=0, allowOutlierCount=0):
    # Quality function for simplex search
    az = direction[0]
    el = direction[1]
    R = 1000.0 / direction[2]  # using 1/R as parameter - curvature dependence is on 1/R to lowest order; will work better in simplex search.
    mse = sf.mseWithDistance(az, el, R, pos, times, outlierThreshold, allowOutlierCount)
    print 'Evaluated direction: az = %f, el = %f, R = %f: MSE = %f' % (az / deg2rad, el / deg2rad, R, mse)

    return mse

def flaggedIndicesForOutliers(inarray, k_sigma = 6):
    # Remove outliers beyond k-sigma (above and below)
    # Use robust estimators, i.e. median and percentile-based sigma.
    n = len(inarray)
    sortedArray = np.sort(inarray)
    median = np.median(sortedArray)
    noise = 0.5 * (sortedArray[0.7*n] - sortedArray[0.3*n]) # ~ sigma estimator, robust against 30% bad data

    goodIndices = np.where( abs(inarray - median) < k_sigma * noise )[0] # indices in original array!
    flaggedIndices = np.array( [i for i in range(n) if i not in goodIndices] ) # inversion

    return (flaggedIndices, goodIndices)


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

        # assume LORA-LOFAR delay and file shifts are of the order ~ 200 samples << blocksize for cut-out timeseries

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
        refant = self.refant
        if not self.refant:
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

        # plot lags, plot flagged lags from a k-sigma criterion on the crosscorr maximum

        cr.hArray(maxima_cc.lags).plot()

        # Plot arrival times, do plane-wave fit, plot residuals wrt plane wave
        arrivaltime = cr.hArray(maxima_cc.lags)
        times = arrivaltime.toNumpy()
        positions = antennaPositions.toNumpy().ravel()

        # Apply calibration delays (per antenna)
        arrivaltime -= cr.hArray(self.f["DIPOLE_CALIBRATION_DELAY"])
        # Apply sub-sample inter-station clock offsets (LOFAR)
        subsampleOffsets = self.f["SUBSAMPLE_CLOCK_OFFSET"] # numpy array!
        # Apply inter-station delays from RFIlines Task
        # Assuming ordering CS002, 3, 4, 5, 7 for this event (sorted alphabetically)
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
        positions2D = antennaPositions.toNumpy()
        # now make footprint plot of all arrival times

        signals = np.copy(times)
        signals.fill(2.71) # signal power not used here; do not give all 1.0 as the log is taken.
        fptask = cr.trerun("Shower", "1", positions=positions2D, signals=signals, timelags=times, footprint_colormap='jet', footprint_enable=True, footprint_shower_enable=False)

        plt.title('Footprint using crosscorrelated arrival times')
        # Do plane-wave direction fit on full arrivaltimes
        # Fit pulse direction
        print 'Do plane wave fit on full arrival times (cross correlations here)...'
        direction_fit_plane_wave = cr.trun("DirectionFitPlaneWave", positions=antennaPositions, timelags=arrivaltime, verbose=True)

        direction_fit_plane_wave.residual_delays.plot()
        plt.title('Residual delays after plane wave fit')

        residu = direction_fit_plane_wave.residual_delays.toNumpy()
        bestfitDelays = sf.timeDelaysFromDirection(positions, direction_fit_plane_wave.meandirection_azel)
        self.planewaveArrivalTimes = bestfitDelays

        # Flag outlying residues. Determine valid range for residues, and set the zero-offset value
        (flaggedIndices, goodIndices) = flaggedIndicesForOutliers(residu)
        print 'Flagged channels: '
        print flaggedIndices

        goodResidues = residu[goodIndices]
        goodPositions2D = positions2D[goodIndices]
        goodPositions = goodPositions2D.ravel()
        goodTimes = times[goodIndices]
        goodSignals = signals[goodIndices]
        goodResidues -= min(goodResidues)
        # now the good one: difference between measured arrival times and plane wave fit!
        fptask_delta = cr.trerun("Shower", "3", positions=goodPositions2D, signals=goodSignals, timelags=goodResidues, footprint_colormap='jet', footprint_enable=True, footprint_shower_enable=False)
        plt.title('Footprint of residual delays w.r.t. planewave fit')

        # Simplex fit point source...
        (az, el) = direction_fit_plane_wave.meandirection_azel  # check
        startPosition = (az, el, 1.0)  # 1.0 means R = 1000.0 ...
        print 'Doing simplex search for minimum MSE...'
        times -= times[0]
        optimum = fmin(mseMinimizer, startPosition, (goodPositions, goodTimes, 0, 4), xtol=1e-5, ftol=1e-5, full_output=1)
        # raw_input('Done simplex search.')

        simplexDirection = optimum[0]
        simplexMSE = optimum[1]
        simplexDirection[2] = 1000.0 / simplexDirection[2]  # invert again to get R
        (simplexAz, simplexEl, simplexR) = simplexDirection

        print '-----'
        print 'Simplex search: '
        print 'Best MSE: %f; best direction = (%f, %f, %f)' % (simplexMSE, simplexAz / deg2rad, simplexEl / deg2rad, simplexR)
        msePlanar = sf.mseWithDistance(simplexAz, simplexEl, 1.0e6, goodPositions, goodTimes, allowOutlierCount=4)
        print 'Best MSE for R = 10^6 (approx. planar): %f' % msePlanar
        print '-----'
        # get the calculated delays according to this plane wave
        # simplexDirection[2] = 4000.0
        bestfitDelays = sf.timeDelaysFromDirectionAndDistance(goodPositions, simplexDirection)
        self.pointsourceArrivalTimes = bestfitDelays

        residu = goodTimes - bestfitDelays
        residu -= min(residu)

        # Difference between measured arrival times and point source fit. Check plot for fit errors!

        fptask_delta_pointsource = cr.trerun("Shower", "4", positions=goodPositions2D, signals=goodSignals, timelags=residu, footprint_colormap='jet', footprint_enable=True, footprint_shower_enable=False)

        plt.title('Footprint of residual delays w.r.t. point source fit')

        """
        see test script testCrosscorr_MultiTBB.py for commented-out stuff

        """
