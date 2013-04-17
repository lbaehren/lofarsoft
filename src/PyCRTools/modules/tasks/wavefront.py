"""
Module documentation
====================

Wavefront Task: produces the shape of the incoming wavefront, using pulse arrival times at each antenna.

Reprocess cross correlation using MultiTBBData datareader, which reads and bundles data from all
datafiles for a given event.

Reprocess cross correlation over all stations (polarisations separately).
Obtain arrival times; apply calibration delays and inter-station delays.

Get wavefront shape and fit curvature using point-source approximation.

later?: Reprocess cross correlation over 0/1 polarisation inside each station.


.. moduleauthor:: Arthur Corstanje <a.corstanje@astro.ru.nl>

"""


from pycrtools.tasks import Task
import pycrtools as cr
from pycrtools import srcfind as sf
from pycrtools import lora
from pycrtools.tasks import directionfitplanewave
import os
import numpy as np
from numpy import sin, cos, tan, sqrt
from scipy.optimize import fmin
import matplotlib.pyplot as plt

c = 299792458.0
deg2rad = np.pi / 180

def mseMinimizer(direction, pos, times, outlierThreshold=0, allowOutlierCount=0):
    # Quality function for simplex search
    az = direction[0]
    el = direction[1]
    R = 1000.0 / direction[2]  # using 1/R as parameter - curvature dependence is on 1/R to lowest order; will work better in simplex search.
    mse = sf.mseWithDistance(az, el, R, pos, times, outlierThreshold, allowOutlierCount)
    print 'Evaluated direction: az = %f, el = %f, R = %f: MSE = %f' % (az / deg2rad, el / deg2rad, R, mse)

    return mse

def flaggedIndicesForOutliers(inarray, k_sigma = 5):
    # Remove outliers beyond k-sigma (above and below)
    # Use robust estimators, i.e. median and percentile-based sigma.
    n = len(inarray)
    sortedArray = np.sort(inarray)
    median = np.median(sortedArray)
    # ~ sigma estimator assuming normal distribution, robust against 30% bad data
    noise = (sortedArray[0.7*n] - sortedArray[0.3*n]) / 1.05

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
#        filefilter = dict(default=None, doc="File filter for multiple data files in one event, e.g. '/my/data/dir/L45472_D20120206T030115.786Z*.h5' "),
#        filelist = dict(default=None, doc="List of filenames in one event. "),
#        f = dict( default = None,
#            doc = "File object. Blocksize, polarisation + antenna selection etc. are taken from the given file object, and should have been set before calling this Task." ),
#        loradir = dict( default = None, doc = "Directory with LORA trigger data. Required for obtaining pulse location in the data. "),

        arrivaltimes = dict( default = None, doc = "Pulse arrival times per antenna; give as input parameter together with 'positions' if not using 'filefilter' or 'filelist'."),
        positions = dict( default = None, doc = "Antenna positions array in [Nx3] format (2D)."),
        stationnames = dict( default=None, doc="Array of station names for each antenna in 'positions'"),
        loracore = dict( default=None, doc="LORA shower core position. If not given, (0, 0) will be assumed."),
        lora_direction = dict( default=None, doc="LORA shower direction."),
        interStationDelays = dict( default=None, doc="Inter-station delays as a correction on current LOFAR clock offsets. To be obtained e.g. from the CalibrateFM Task. Assumed to be in alphabetic order in the station name e.g. CS002, CS003, ... If not given, zero correction will be assumed." ),
        stationList = dict( default=None, doc="List of station names present in the positions and arrivaltimes arrays. Only needed if interStationDelays also supplied."),
        stationStartIndex = dict( default=None, doc="List of start indices of a given station. Array should end with an entry n where n = nof antennas. Only needed if interStationDelays given."),
#        blocksize = dict ( default = 65536, doc = "Blocksize." ),
#        nantennas = dict( default = lambda self : self.f["NOF_SELECTED_DATASETS"],
#            doc = "Number of selected antennas." ),
#        nofblocks = dict( default = -1, doc = "Number of data blocks to process. Set to -1 for entire file." ),
#        pulse_block = dict( default = 0, doc = "Block nr. where the pulse is" ),
#        refant = dict(default = None, doc = "Optional parameter to set reference antenna number."),
        pol = dict(default = 0, doc = "Polarization. Only (to be) used for reference / plotting purposes."),
        ## OUTPUT PARAMS ##
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
        plot_type = dict(default="png",
            doc="Plot type (e.g. png, jpeg, pdf)"),
        verbose = dict( default = True, doc = "Verbose output."),
    )

    def run(self):
        """Run the task.
        """

        if self.positions is None or self.arrivaltimes is None:
            raise ValueError("Need to supply 'positions' and 'arrivaltimes' parameters!")

        if not hasattr(self.arrivaltimes, 'toNumpy'):
            print 'Warning: arrivaltimes expected as hArray. Converting it now.'
            self.arrivaltimes = cr.hArray(self.arrivaltimes)

        if not hasattr(self.positions, 'toNumpy'):
            print 'Warning: arrivaltimes expected as hArray. Converting it now.'
            self.positions = cr.hArray(self.positions)
        # Applying of inter-station delays goes here... if we decide to do that in here.

            # Apply calibration delays (per antenna)
#            arrivaltime -= cr.hArray(self.f["DIPOLE_CALIBRATION_DELAY"])
            # Apply sub-sample inter-station clock offsets (LOFAR)
#            subsampleOffsets = self.f["SUBSAMPLE_CLOCK_OFFSET"] # numpy array!
            # Apply inter-station delays from RFIlines Task
            # Assuming ordering CS002, 3, 4, 5, 7 for this event (sorted alphabetically)
#            if not type(self.interStationDelays) == type(None): # cannot do if self.interStationDelays, apparently...
#                subsampleOffsets -= self.interStationDelays

    #        subsampleOffsets[1] -= 0.11e-9
    #        subsampleOffsets[2] -= -1.30e-9
    #        subsampleOffsets[3] -= -1.32e-9
    #        subsampleOffsets[4] -= 0.71e-9

#            stationList = self.f["STATION_LIST"]
#            print stationList
#            stationStartIndex = self.f["STATION_STARTINDEX"]
#            for i in range(len(subsampleOffsets)):
#                arrivaltime[stationStartIndex[i]:stationStartIndex[i + 1]] -= subsampleOffsets[i]
                # Sign + or - ???
#            self.positions = antennaPositions
#            self.arrivaltimes = arrivaltime

        # plt.figure()
        # arrivaltime.plot()
        # plt.title('Arrival times, matched with offsets per station (check!)')
        if isinstance(self.arrivaltimes,np.ndarray):
            times = self.arrivaltimes
        else:
            times = self.arrivaltimes.toNumpy()
        if isinstance(self.positions,np.ndarray):
            positions2D = self.positions
        else:
            positions2D = self.positions.toNumpy()
        positions = positions2D.ravel()
        # now make footprint plot of all arrival times

        signals = np.copy(times)
        signals.fill(2.71) # signal power not used here; do not give all 1.0 as the log is taken.
        fptask = cr.trerun("Shower", "1", positions=positions2D, signals=signals, timelags=times, footprint_colormap='jet', footprint_enable=True, footprint_shower_enable=False)

        plt.title('Footprint using crosscorrelated arrival times')
        # Do plane-wave direction fit on full arrivaltimes
        # Fit pulse direction
        print 'Do plane wave fit on full arrival times (cross correlations here)...'
        direction_fit_plane_wave = cr.trun("DirectionFitPlaneWave", positions=self.positions, timelags=self.arrivaltimes, verbose=True)

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
        goodStationNames = self.stationnames[goodIndices]

        stationStartIndex = []
        stationList = []
        for i in range(len(goodStationNames)):
            if goodStationNames[i] != goodStationNames[i-1]:
                stationList.append(goodStationNames[i])
                stationStartIndex.append(i)
        stationStartIndex.append(len(goodStationNames)) # last index + 1 for indexing with start:end

        # now the good one: difference between measured arrival times and plane wave fit!
        fptask_delta = cr.trerun("Shower", "3", positions=goodPositions2D, signals=goodSignals, timelags=goodResidues, footprint_colormap='jet', footprint_enable=True, footprint_shower_enable=False)
        plt.title('Footprint of residual delays w.r.t. planewave fit')

        if self.save_plots:
            p = self.plot_prefix + "wavefront_vs_planewave.{0}".format(self.plot_type)
            plt.savefig(p)
            self.plotlist.append(p)

        # Make 1-D projected plot of curvature:
        # - take shower direction and core position from LORA
        # - project antenna positions to shower coordinates (in the plane with shower axis as normal vector)
        # - subtract the (plane-wave) time difference due to the distance shower plane - real antenna position
        #az = np.radians(self.lora_direction[0])
        #el = np.radians(self.lora_direction[1])
        (az, el) = direction_fit_plane_wave.meandirection_azel
#        print (az, el)
        cartesianDirection = - np.array([cos(el) * sin(az), cos(el) * cos(az), sin(el)]) # minus sign for incoming vector!
        #core = np.array([self.loracore[0], self.loracore[1], 0.0])
        fakecoreX = 0.0
        fakecoreY = 0.0
        for pos in goodPositions2D:
            fakecoreX += pos[0]
            fakecoreY += pos[1]
        core = (1.0 / len(goodPositions2D)) * np.array([fakecoreX, fakecoreY, 0.0])

        axisDistance = []
        showerPlaneTimeDelay = []

        for pos in goodPositions2D:
            relpos = pos - core
            delay = (1/c) * np.dot(cartesianDirection, relpos)
            distance = np.linalg.norm(relpos - np.dot(cartesianDirection, relpos) * cartesianDirection)
            axisDistance.append(distance)
            showerPlaneTimeDelay.append(delay)

        axisDistance = np.array(axisDistance)
        showerPlaneTimeDelay = np.array(showerPlaneTimeDelay)

        reducedArrivalTimes = 1e9 * (goodTimes - showerPlaneTimeDelay)

        polyfit = np.polyfit(axisDistance, reducedArrivalTimes, 4)
        polyvalues = np.poly1d(polyfit)

        reducedArrivalTimes -= polyfit[4]

        plt.figure()
        start = 0
        colors = ['b', 'g', 'r', 'c', 'm', 'y'] * 4 # don't want to run out of colors array
        for i in range(len(stationList)):
            start = stationStartIndex[i]
            end = stationStartIndex[i+1]
            plt.scatter(axisDistance[start:end], reducedArrivalTimes[start:end], 20, label=stationList[i], c = colors[i], marker='o')

        plt.plot(np.sort(axisDistance), polyvalues(np.sort(axisDistance)) - polyfit[4], marker='-', c='r')
        plt.legend()
        plt.xlabel('Distance from axis [m]')
        plt.ylabel('Arrival time in fitted shower plane [ns]')
        #plt.plot(a, expectedDelays*1e9, c='g')
        plt.title('Arrival times vs distance from fit-centered shower axis\n Polyfit coeffs t = %1.5f r + %1.5f r^2 + %1.5f r^3 + %1.5f r^4' % (polyfit[3], polyfit[2], polyfit[1], polyfit[0]))
        if self.save_plots:
            p = self.plot_prefix + "wavefront_arrivaltime_showerplane.{0}".format(self.plot_type)
            plt.savefig(p)
            self.plotlist.append(p)




        # rotate antenna position axes to a, p coordinates. a = along time gradient, p = perpendicular to that
        # Make plots for arrival times and second-order curvature:
        # - along time gradient
        # - perp to time gradient
        (az, el) = direction_fit_plane_wave.meandirection_azel

        (x, y) = (goodPositions2D[:, 0], goodPositions2D[:, 1])
        if self.loracore is None:
            self.loracore = [0.0, 0.0]
        a = - sin(az) * (x - self.loracore[0]) - cos(az) * (y - self.loracore[1])
        pp = cos(az) * (x - self.loracore[0]) - sin(az) * (y - self.loracore[1])
        z = goodPositions2D[:, 2]
        nofGoodChannels = len(a)
        rotatedPositions2D = np.concatenate([a, pp, z]).reshape(3, nofGoodChannels).T

#        expectedDelays = sf.timeDelaysFromDirection(goodPositions, (az, el))
#        expectedDelays += (goodTimes[0] - expectedDelays[0]) # cheap linear fit

        plt.figure()
        start = 0
        colors = ['b', 'g', 'r', 'c', 'm', 'y'] * 4 # don't want to run out of colors array
        for i in range(len(stationList)):
            start = stationStartIndex[i]
            end = stationStartIndex[i+1]
            plt.scatter(a[start:end], goodTimes[start:end] * 1e9, 20, label=stationList[i], c = colors[i], marker='o')
        plt.legend()
        plt.xlabel('Distance along propagation direction [m]')
        plt.ylabel('Arrival time [ns]')
        #plt.plot(a, expectedDelays*1e9, c='g')
        plt.title('Arrival times versus distance along propagation direction')
        if self.save_plots:
            p = self.plot_prefix + "wavefront_arrivaltime_along_direction.{0}".format(self.plot_type)
            plt.savefig(p)
            self.plotlist.append(p)

        plt.figure()

        for i in range(len(stationList)):
            start = stationStartIndex[i]
            end = stationStartIndex[i+1]
            plt.scatter(pp[start:end], goodTimes[start:end] * 1e9, 20, label=stationList[i], c = colors[i], marker = 'o')

        plt.legend()
        plt.xlabel('Distance perp. to propagation direction [m]')
        plt.ylabel('Arrival time [ns]')
        plt.title('Arrival times versus distance perpendicular to propagation direction')
        if self.save_plots:
            p = self.plot_prefix + "wavefront_arrivaltime_perp_direction.{0}".format(self.plot_type)
            plt.savefig(p)
            self.plotlist.append(p)

        plt.figure()
        for i in range(len(stationList)):
            start = stationStartIndex[i]
            end = stationStartIndex[i+1]
            plt.scatter(a[start:end], goodResidues[start:end] * 1e9, 20, label=stationList[i], c = colors[i], marker='o')
        plt.legend()
        plt.xlabel('Distance along propagation direction [m]')
        plt.ylabel('Arrival time minus plane-wave [ns]')

        plt.title('Second-order wavefront, along direction of propagation')
        if self.save_plots:
            p = self.plot_prefix + "wavefront_curvature_along_direction.{0}".format(self.plot_type)
            plt.savefig(p)
            self.plotlist.append(p)


        plt.figure()
        for i in range(len(stationList)):
            start = stationStartIndex[i]
            end = stationStartIndex[i+1]
            plt.scatter(pp[start:end], goodResidues[start:end] * 1e9, 20, label=stationList[i], c=colors[i], marker='o')
        plt.legend()
        plt.xlabel('Distance perp. to propagation direction [m]')
        plt.ylabel('Arrival time minus plane-wave [ns]')

        plt.title('Second-order wavefront, perpendicular to propagation')
        if self.save_plots:
            p = self.plot_prefix + "wavefront_curvature_perp_direction.{0}".format(self.plot_type)
            plt.savefig(p)
            self.plotlist.append(p)


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

        self.fitPointSource = (simplexAz / deg2rad, simplexEl / deg2rad, simplexR, simplexMSE)

        print '-----'
        print 'Simplex search: '
        print 'Best MSE: %f; best direction = (%f, %f, %f)' % (simplexMSE, simplexAz / deg2rad, simplexEl / deg2rad, simplexR)
        msePlanar = sf.mseWithDistance(simplexAz, simplexEl, 1.0e7, goodPositions, goodTimes, allowOutlierCount=4)
        print 'Best MSE for R = 10^7 (approx. planar): %f' % msePlanar
        print '-----'
        self.fitPlaneWave = (az / deg2rad, el / deg2rad, msePlanar)

        # get the calculated delays according to this plane wave
        # simplexDirection[2] = 4000.0
        bestfitDelays = sf.timeDelaysFromDirectionAndDistance(goodPositions, simplexDirection)
        self.pointsourceArrivalTimes = bestfitDelays

        residu = goodTimes - bestfitDelays
        residu -= min(residu)

        # Difference between measured arrival times and point source fit. Check plot for fit errors!

        fptask_delta_pointsource = cr.trerun("Shower", "4", positions=goodPositions2D, signals=goodSignals, timelags=residu, footprint_colormap='jet', footprint_enable=True, footprint_shower_enable=False)

        plt.title('Footprint of residual delays w.r.t. point source fit')

        if self.save_plots:
            p = self.plot_prefix + "wavefront_vs_pointsource.{0}".format(self.plot_type)
            plt.savefig(p)
            self.plotlist.append(p)


        """
        see test script testCrosscorr_MultiTBB.py for commented-out stuff

        """
