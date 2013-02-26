"""
Task:

Calibrate delays between antennas, and especially inter-station delays, using narrow-band radio transmitters.
Especially transmitters with a known location are useful, e.g. the FM tower in Smilde (GPS long = 6.403565 E, lat = 52.902671 N).

Uses the phases (average and rms) obtained in the FindRFI Task; or will re-run that task if no phases are given
as input.

.. moduleauthor:: Arthur Corstanje <a.corstanje@astro.ru.nl>

"""

from pycrtools.tasks import Task, pulsecal
import numpy as np
import pycrtools as cr
import matplotlib.pyplot as plt
import pycrtools as cr
from pycrtools import srcfind as sf
import pytmf
import datetime
import os

twopi = 2 * np.pi
deg2rad = twopi / 360.0
halfpi = twopi / 4


def vectorAverage(directions):
    """
    Takes a list of (az, el) tuples
    Converts to cartesian vectors, sums them up to average
    Converts average back to (az, el)
    returns average (az, el)
    """
    sumvector = cr.hArray(float, dimensions=[3])
    azel = cr.hArray(copy=sumvector)
    cartesian = cr.hArray(copy=sumvector)

    for direction in directions:
        azel[0] = direction[0]
        azel[1] = direction[1]
        azel[2] = 1.0  # radius
        cr.hCoordinateConvert(azel, cr.CoordinateTypes.AzElRadius, cartesian, cr.CoordinateTypes.Cartesian, False)
        sumvector += cartesian
#    cr.hCoordinateConvert(sumvector, cr.CoordinateTypes.Cartesian, azel, cr.CoordinateTypes.AzElRadius, False)
    # not implemented in hCoordinateConvert... todo
    meandirection_spherical = pytmf.cartesian2spherical(sumvector[0], sumvector[1], sumvector[2])

    meandirection_azel = (np.pi - (meandirection_spherical[2] + halfpi), halfpi - (meandirection_spherical[1]))

#    print meandirection_azel[0] / deg2rad
#    print meandirection_azel[1] / deg2rad

    del sumvector
    del cartesian

    return (meandirection_azel[0], meandirection_azel[1])


def getOneSampleShifts(timeDiff, stationStartIndex, interStationDelays):
    """
    Scan the array of time differences (between measured phases and the modeled incoming wave)
    for presence of offsets that are a multiple of 5 ns.
    Note: due to the modulo-2pi from the phases, only small multiples (effectively +/- 1) can be
    reliably corrected.

    Input stationStartIndex contains the starting points in the array for each station.
    interStationDelays are subtracted here.

    Returns array oneSampleShifts, containing integer # samples by which the data is shifted.
    So when it returns [0, 0, 1, 0, -1, ...] a positive number means that the data has to be advanced 1 sample
    i.e. sample 0 <-- sample 1
    Negative number the other way round.

    (NB. check!)

    """

    n = len(timeDiff)
    delays = np.zeros(n)
    oneSampleShifts = np.zeros(n)
    nofStations = len(stationStartIndex) - 1
    for i in range(nofStations):
        start = stationStartIndex[i]
        end = stationStartIndex[i + 1]
        for j in range(start, end):
            # delays[j] = timeDiff[j] - interStationDelays[i]
            thisDelay = timeDiff[j] - interStationDelays[i]
            if abs(thisDelay) > 3:  # 3 ns threshold to call it a glitch
                oneSampleShifts[j] = int(round(thisDelay / 5))

    return oneSampleShifts

def getMultiFreqDelay(lines, freqs, phase_average, median_phase_spreads, modelTimes):
    strongestChannels = []
    f0 = freqs[1]
    for line in lines: # determine strongest frequency in [line-0.1, line+0.1]
        startindex = int((line-0.1) * 1.0e6 / f0)
        stopindex = 1 + int((line+0.1) * 1.0e6 / f0)
        bestchannel = startindex + median_phase_spreads[startindex:stopindex].argmin()
        print 'Best channel for freq %3.1f is: %3.3f at phase stability %1.4f' % (line, bestchannel * f0 / 1.0e6, median_phase_spreads[bestchannel])
        strongestChannels.append(bestchannel)

    nofchannels = len(modelTimes)
    bestFitDelay = np.zeros(nofchannels)
    t = 0.01e-9 * (-800 + np.arange(1600)) # in 0.1 ns
    #window = np.exp(-np.abs(t) / 100e-9)
    for i in range(nofchannels): # for all antennas...
        ycos = np.zeros(1600)
        ysin = np.zeros(1600)
        for channel in strongestChannels:
            freq = f0 * channel
            phase = phase_average[i, channel] # take antenna 10 to test
            # subtract model phases from incoming wave
    #        modelphases = sf.timeDelaysFromGPSPoint(self.referenceTransmitterGPS, positions, np.array(self.f["SELECTED_DIPOLES"]), self.f["ANTENNA_SET"][0])
            modelphases = - twopi * freq * modelTimes # Changes with frequency
            modelphases = sf.phaseWrap(modelphases)
            phase -= modelphases[i]
            #HACK
            #phase = np.random.rand() * twopi - twopi/2
            ycos += np.cos( - twopi * freq * t + phase)
            ysin += np.sin( - twopi * freq * t + phase)

        bestFitIndex = np.argmax(ycos) # ycos ** 2 + ysin ** 2 for Hilbert envelope...
        bestFitDelay[i] = t[bestFitIndex]
        if i == 15:
            print 'Plotting crosscorrelation for antenna %d' % i
            plt.figure()
            plt.plot(t*1e9, ycos)
            plt.plot(t*1e9, ysin)
            plt.figure()
            plt.plot(t*1e9, ycos**2 + ysin**2)

            print bestFitDelay[i]


    return bestFitDelay



class CalibrateFM(Task):
    """
    **Description:**

    Calibrate antenna delays, especially inter-station delays, by using FM transmitter RFI in the data.

    **Usage:**

    **See also:**
    :class:`plotfootprint`

    **Example:**

    ::
        filefilter="/Volumes/Data/sandertv/VHECR/LORAtriggered/results/VHECR_LORA-20110716T094509.665Z/"
        crfootprint=cr.trun("plotfootprint",filefilter=filefilter,pol=polarization)
   """
    parameters = dict(
        filefilter = dict(default=None,
                          doc="File filter for multiple data files in one event, e.g. '/my/data/dir/L45472_D20120206T030115.786Z*.h5' "),

        filelist = dict(default=None,
                        doc="List of filenames in one event. "),

        f = dict(default=None,
                 doc="File object."),

        phase_average = dict(default=None,
                             doc="Average phase spectrum per antenna, as output from FindRFI Task. Optional, if not given 'filefilter' or 'filelist' will be used to (re)run FindRFI."),

        phase_RMS = dict(default=None,
                         doc="Phase RMS from FindRFI Task."),

        median_phase_spreads = dict(default=None,
                                    doc="Output from FindRFI: Median (over all antennas) standard-deviation, per frequency channel. 1-D array with length blocksize/2 + 1."),
        refant = dict(default=0, doc="Reference antenna from FindRFI."),

        referenceTransmitterGPS = dict(default=None,
                                       doc="GPS [long, lat] in degrees (N, E is positive) for a known transmitter. Typically used when tuning to a known frequency. The Smilde tower is at (6.403565, 52.902671)."),

        doplot = dict(default=True,
                      doc="Produce output plots"),

        testplots = dict(default=False,
                         doc="Produce testing plots for calibration on RF lines"),

        pol = dict(default=0,
                   doc="0 or 1 for even or odd polarization. Only used when re-running FindRFI."),

        # maxlines = dict(default=1,
        #                 doc="Max number of RF lines to consider"),

        lines = dict(default=None,
                     doc="(List of) RF line(s) to use, by frequency channel index"),

        # antennaselection = dic(default=None,
        #                        doc="Optional: list of antenna numbers (RCU/2) to include"),

        # minSNR = dict(default=50,
        #               doc="Minimum required SNR of the line"),

        blocksize = dict(default=8000,
                         doc="Blocksize of timeseries data, for FFTs. Only used when no phase_average are given. Take e.g. 8000 to match radio station frequencies."),

        nofblocks = dict(default=100,
                         doc="Max. number of blocks to process"),

        correctOneSampleShifts = dict(default=False,
                                      doc="Automatically correct for +/- 5 ns shifts in the data. Only works correctly if the direction fit / the reference transmitter GPS is good. Output is in oneSampleShifts list."),

        direction_resolution = dict(default=[1, 5],
                                    doc="Resolution in degrees [az, el] for direction search"),

        directionFromAllStations = dict(default=False,
                                        doc="Set True if you want to use all stations together to calculate the incoming direction. Otherwise, it is done per station and the results are averaged. "),

        freq_range = dict(default=[30, 70],
                          doc="Frequency range in which to search for calibration sources"),

        # OUTPUT PARAMETERS:

        nofchannels = dict(default=-1,
                           doc="nof channels",
                           output=True),

        dirtychannels = dict(default=None,
                             doc="Output array of dirty channels, based on stability of relative phases from consecutive data blocks. Deviations from uniform-randomness are quite small, unless there is an RF transmitter present at a given frequency.",
                             output=True),


        oneSampleShifts = dict(default=None,
                               doc="Output array containing the 5 ns-shifts per antenna, if they occur. Output contains integer number of samples the data is shifted. To correct for it, shift data _forward_ by this number of samples.",
                               output=True),

        interStationDelays = dict(default=None,
                                  doc="Output dict containing station name - delay value pairs.",
                                  output=True),

        maxPhaseError = dict(default=1.0,
                             doc="Maximum allowed phase error in ns^2 (added from all antennas) to call it a good fit and set calibrationStatus to 'OK'."),

        phaseError = dict(default=None,
                          doc="Phase error for the first station in the list. Used to check if the fit is good, e.g. when using fixed reference transmitter",
                          output=True),

        calibrationStatus = dict(default=None,
                                 doc="Output status string. ",
                                 output=True),

        fittedDirections = dict(default=None,
                                doc="Output dict with for each station [az, el] = the direction of the strongest transmitter. Output in degrees",
                                output=True),

        strongestFrequency = dict(default=None,
                                  doc="The frequency with the smallest phase_RMS, from the range specified in the freq_range parameter",
                                  output=True),

        bestPhaseRMS = dict(default=None,
                            doc="PhaseRMS for the best frequency",
                            output=True),

        timestamp = dict(default=None,
                         doc="Unix timestamp of input file(s)",
                         output=True),

        plot_finish = dict(default=lambda self: cr.plotfinish(doplot=True, filename="calibratefm", plotpause=False),
                           doc="Function to be called after each plot to determine whether to pause or not (see ::func::plotfinish)"),

        plot_name = dict(default="calibratefm",
                         doc="Extra name to be added to plot filename."),

        nofantennas=dict(default=lambda self: self.positions.shape()[-2],
                         doc="Number of antennas.",
                         output=True),

        filetype = dict(default="pdf",
                        doc="extension/type of plot output files"),

        save_images = dict(default=False,
                           doc="Enable if images should be saved to disk in default folder"),
        )

    def call(self):
        pass

    def run(self):

        if self.filelist and self.filefilter:
            raise RuntimeError("Both filelist and filefilter given; provide only one of them")

        if not self.doplot and self.testplots:
            print 'Warning: setting self.testplots to False because self.doplot = False'
            self.testplots = False

        if not self.f or not (self.phase_average and self.median_phase_spreads and self.phase_RMS):  # re-run FindRFI to get those.
            if self.filelist:
                filelist = self.filelist
            else:
                filelist = cr.listFiles(self.filefilter)
            superterpStations = ["CS002", "CS003", "CS004", "CS005", "CS006", "CS007", "CS021"]
            if len(filelist) == 1:
                print 'Note: There is only 1 file'
                #filelist = filelist[0]
            else:  # sort files on station ID
                sortedlist = []
                for station in superterpStations:
                    thisStationsFile = [filename for filename in filelist if station in filename]
                    if len(thisStationsFile) > 0:
                        sortedlist.append(thisStationsFile[0])
                filelist = sortedlist
            print '[CalibrateFM] Processing files: '
            print filelist
            self.f = cr.open(filelist, blocksize=self.blocksize)
            selected_dipoles = [x for x in self.f["DIPOLE_NAMES"] if int(x) % 2 == self.pol]
            print 'SELECTING POL %d' % self.pol
            self.f["SELECTED_DIPOLES"] = selected_dipoles
            print 'DONE SELECTION'
            findrfi = cr.trerun("FindRFI", "0", f=self.f, nofblocks=self.nofblocks, refant=0)  # do all blocks by default. Do only given polarisation.
            # NB! Assumed that FindRFI has been run with one (the given) polarisation only!
            # Get output of FindRFI into our parameter space. Apply channel selection for polarisation
            findrfi_channels = findrfi.nantennas  # total nr. of channels
            # thisPolsChannels = range(self.pol, findrfi_channels, 2)

            # a = findrfi.phase_average[thisPolsChannels, ...]
            a = findrfi.phase_average.toNumpy()  # [thisPolsChannels]
            self.phase_average = cr.hArray(a)
            a = findrfi.median_phase_spreads
            self.median_phase_spreads = cr.hArray(a)  # they are a median over all antennas, which is OK
            a = findrfi.phase_RMS.toNumpy()  # [thisPolsChannels]
            self.phase_RMS = cr.hArray(a)
            self.refant = findrfi.refant
            # Numpy for using argmin, argsort etc.

        self.blocksize = self.f["BLOCKSIZE"]
        self.nofchannels = len(self.f["SELECTED_DIPOLES"])
        print '# channels = %d' % self.nofchannels
        if not self.filefilter:  # set for title plotting purpose
            firstname = self.f._MultiTBBData__files[0]["FILENAME"]
            self.filefilter = os.path.split(firstname)[-1].split("_CS")[0]
        # get calibration delays from file
        caldelays = cr.hArray(self.f["DIPOLE_CALIBRATION_DELAY"])  # f[...] outputs list!
        # add subsample clock offsets to caldelays
        if isinstance(self.f, cr.io.tbb.MultiTBBData):
            subsample_clockoffsets = self.f["SUBSAMPLE_CLOCK_OFFSET"]
            station_startindex = self.f["STATION_STARTINDEX"]
            for i in range(len(subsample_clockoffsets)):
                start = station_startindex[i]
                end = station_startindex[i + 1]
                caldelays[start:end] += subsample_clockoffsets[i] # debug float unit?

        # make calibration phases, i.e. convert delays to phases for every frequency
        freqs = self.f["FREQUENCY_DATA"]
        calphases = cr.hArray(float, dimensions=[self.nofchannels, len(freqs)])
        cr.hDelayToPhase(calphases, freqs, caldelays)

        self.phase_average += calphases # Apply dipole calibration delays + subsample clock offsets as phases
        cr.hPhaseWrap(self.phase_average, self.phase_average) # And wrap phases again into (-pi, pi)
        self.phase_average = self.phase_average.toNumpy()
        # Find the best line in given frequency range, i.e. with best phase stability [ to function? ]
        self.median_phase_spreads = self.median_phase_spreads.toNumpy()
        if not self.freq_range:  # take overall best channel
            bestchannel = self.median_phase_spreads.argmin()
        else:  # take the best channel in the given range
            f0 = 200.0e6 / self.blocksize  # hardcoded sampling rate
            startindex = int(self.freq_range[0] * 1.0e6 / f0)
            stopindex = 1 + int(self.freq_range[1] * 1.0e6 / f0)
            bestchannel = startindex + self.median_phase_spreads[startindex:stopindex].argmin()

        bestchannel = int(bestchannel)  # ! Needed for use in hArray slicing etc. Type numpy.int64 not recognized otherwise
#        elif isinstance(self.lines, type([])):
#            bestchannel = self.lines[0]  # known strong transmitter channel for this event (may vary +/- 5 channels?)
#        else:  # either list or number assumed
#            bestchannel = self.lines

        channelsSortedByStability = self.median_phase_spreads.argsort()
        freqsByStability = freqs.toNumpy()[channelsSortedByStability]
        print 'The best 10 channels: '
        for i in range(10):
            print 'Channel %d, freq %2.3f: phase RMS = %1.4f' % (channelsSortedByStability[i], freqsByStability[i] / 1.0e6, self.median_phase_spreads[channelsSortedByStability[i]])

        print ' Median phase-sigma is lowest (i.e. best phase stability) at channel %d, value = %f' % (bestchannel, self.median_phase_spreads[bestchannel])

        freq = freqs[bestchannel] # the frequency we will use for calibration

        # FIX: condition only if no reference transmitter given...
        # find direction of this source; plot image to check quality of fit.
        print 'Find direction of source based on averaged phases per antenna...'

        allpositions = self.f["ANTENNA_POSITIONS"].toNumpy()
        azSteps = int(360 / self.direction_resolution[0])
        elSteps = int(90 / self.direction_resolution[1])

        # get list of stations in this dataset
        stationlist = self.f["STATION_LIST"]
        stationStartIndex = self.f["STATION_STARTINDEX"]
        nofStations = len(stationlist)
        directions = []
        nofStationsToTest = 1 if self.referenceTransmitterGPS else nofStations
        self.fittedDirections = {} # accumulate dict
        # Get direction fits for each station if no fixed reference transmitter is given.
        for i in range(nofStationsToTest):
            print 'Finding direction for station: %s' % stationlist[i]
            start = stationStartIndex[i]
            end = stationStartIndex[i + 1]
            thesePositions = allpositions[start:end].ravel()

            averagePhaseThisStation = self.phase_average[start:end, bestchannel]
            plt.figure()
            (fitaz, fitel, minPhaseError) = sf.directionBruteForcePhases(thesePositions, averagePhaseThisStation, freq, azSteps=azSteps, elSteps=elSteps, allowOutlierCount=4, showImage=(self.doplot and self.testplots), verbose = True)
            if self.doplot and self.testplots:
                self.plot_finish(filename=self.plot_name + "-phaseimage", filetype=self.filetype)

            self.fittedDirections[stationlist[i]] = (fitaz / deg2rad, fitel / deg2rad)
            print 'Best fit az = %f, el = %f' % self.fittedDirections[stationlist[i]]
            print 'Phase error = %f' % minPhaseError
            directions.append((fitaz, fitel))

            averageIncomingDirection = vectorAverage(directions)

        # get modeled phases for a plane wave of given overall direction, for all stations together
        allpositions = allpositions.ravel()

        if self.referenceTransmitterGPS:
        # Get model phases from this GPS point
            modelTimes = sf.timeDelaysFromGPSPoint(self.referenceTransmitterGPS, allpositions, np.array(self.f["SELECTED_DIPOLES"]), self.f["ANTENNA_SET"][0])
            modelTimes -= modelTimes[self.refant] # Should compare with refant instead of ant 0 ??!
            modelphases = - twopi * freq * modelTimes # !! Minus sign to be compatible with FFT's phase sign
            modelphases = sf.phaseWrap(modelphases)
        else: # get them from average incoming (fitted) direction
            modelphases = sf.phasesFromDirection(allpositions, averageIncomingDirection, freq)
            modelphases -= modelphases[0]
            modelphases = sf.phaseWrap(modelphases)  # have to wrap again as we subtracted the ref. phase

        averagePhasePerAntenna = self.phase_average[:, bestchannel]  # now for all antennas

        # Get phase differences w.r.t. model phases, evaluate goodness of fit.
        phaseDiff = averagePhasePerAntenna - modelphases
        phaseError = sf.phaseErrorFromDifference(phaseDiff[0:stationStartIndex[1]], freq, allowOutlierCount=4)

        print 'Phase error for first station: %3.3f' % phaseError
        self.phaseError = phaseError
        self.calibrationStatus = "OK" if phaseError < self.maxPhaseError else "BAD"
        print 'Fit is %s as the max allowed phase error is set to %3.3f' % (self.calibrationStatus, self.maxPhaseError)
        nanosecondPhase = twopi * freq * 1.0e-9

        if self.lines is not None and self.referenceTransmitterGPS is not None:
            timeDiff = 1.0e9* getMultiFreqDelay(self.lines, freqs, self.phase_average, self.median_phase_spreads, modelTimes)
        else:
            timeDiff = sf.phaseWrap(phaseDiff) / nanosecondPhase

        # Do inter-station delays with plot and output param (dict)
        if nofStations > 1:
            interStationDelays = np.zeros(nofStations)
            refdelay = 0.0
            if self.doplot:
                plt.figure()
            for i in range(nofStations):
                start = stationStartIndex[i]
                end = stationStartIndex[i + 1]
                interStationDelays[i] = np.median(timeDiff[start:end])
                if i == 0:
                    refdelay = interStationDelays[i]

                interStationDelays[i] -= refdelay
                if self.doplot:
                    plt.plot(np.array([start, end]), np.array([interStationDelays[i], interStationDelays[i]]), c='g', lw=3, label='Median station delay' if i == 0 else '')

    #                        plt.annotate(stationlist[i])
            # Subtract reference station-delay
            timeDiff -= refdelay
            print '--- Inter-station delays: ---'
            self.interStationDelays = {}
            for i in range(nofStations):
                self.interStationDelays[stationlist[i]] = interStationDelays[i]  # write to Task output param
                print '%s: %2.3f ns' % (self.f["STATION_LIST"][i], interStationDelays[i])

        # Correct one-sample shifts that may be present in the data.
        # Looks for diff w.r.t. LOFAR calibration delays > +/- 5 ns
        if self.correctOneSampleShifts:
            self.oneSampleShifts = getOneSampleShifts(timeDiff, stationStartIndex, interStationDelays)
        else:
            self.oneSampleShifts = np.zeros(len(timeDiff))

        timeDiff_fixed = timeDiff - 5.0 * self.oneSampleShifts
        timeDiff_glitches = np.array([np.float('nan') if dt == 0.0 else 0.0 for dt in self.oneSampleShifts])
        # make zeros for glitch indices, 'nan' otherwise. Then add raw timeDiff to show glitches
        timeDiff_glitches += timeDiff

        # Make plots
        if self.doplot:
            plt.plot(timeDiff_fixed, 'o-', c='b', label='Measured - expected phase')
            if self.correctOneSampleShifts:
                plt.plot(timeDiff_glitches, 'o-', c='r', label='5 ns shifts found')
            # plt.figure()
            rms_phase = self.phase_RMS.toNumpy()[:, bestchannel]
            plt.plot(rms_phase, 'r', label='RMS phase noise')
            plt.plot(- rms_phase, 'r')
            plt.title(self.filefilter + '\nPhase difference between measured and best-fit modeled phase\nChannel %d,   f = %2.4f MHz,   pi rad = %1.3f ns' % (bestchannel, freq / 1.0e6, 1.0e9 / (2 * freq)))
            plt.ylabel('Time difference from phase [ns]')
            plt.xlabel('Antenna number (RCU/2)')
            plt.legend()

            self.plot_finish(filename=self.plot_name + "-calibration-phases", filetype=self.filetype)

            cr.trerun("PlotAntennaLayout", "0", positions=self.f["ANTENNA_POSITIONS"], colors=cr.hArray(list(timeDiff)), sizes=100, title="Measured - expected time", plotlegend=True)

            self.plot_finish(filename=self.plot_name + "-calibration-layout", filetype=self.filetype)

        # set output params
        # TEST
        if self.lines is not None and self.referenceTransmitterGPS is not None:
            getMultiFreqDelay(self.lines, freqs, self.phase_average, self.median_phase_spreads, modelTimes)

        self.strongestFrequency = freqs[bestchannel]
        self.bestPhaseRMS = self.median_phase_spreads[bestchannel]
        self.timestamp = self.f["TIME"][0]
