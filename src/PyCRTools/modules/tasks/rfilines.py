"""
Task: Get locations of RF transmitters present in spectra of data files; get calibration delays from their phases.
======================

Created by Arthur Corstanje, June 2012.
"""

import pycrtools as cr
from pycrtools import hArray
from pycrtools import srcfind as sf
import numpy as np
import matplotlib.pyplot as plt
import pycrtools.tasks as tasks
import pytmf
import datetime

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


def getLines(dirtychannels, blocksize_wanted, blocksize_used):
    # list of dirty channels in, convert blocksizes if needed.
    ch = np.array(dirtychannels) * 1.0
    ch /= float(blocksize_used / blocksize_wanted)
    ch = round(ch)


def dirtyChannelsFromPhaseSpreads(spreads, verbose=False):
    length = len(spreads)
    sorted = np.sort(spreads)
    # Sorted plot will have many entries below the uniform-random value of sqrt(2).
    # They correspond to the RFI lines. Circumvent taking standard-deviation and peak-detection
    # by estimating the (noise) spread using the higher half of the data
    # then have a criterion of > 3x noise to make an RF-line detection.
    medianSpread = np.median(spreads)
    noise = sorted[length * 0.95] - sorted[length / 2]  # value at 95 percentile minus median

    dirtyChannels = np.where(spreads < medianSpread - 3 * noise)[0]

    # extend dirty channels by one entry to each side
    extDirtyChannels = []
    for i in range(len(spreads)):
        if i in dirtyChannels:
            if i - 1 not in dirtyChannels:
                extDirtyChannels.append(i - 1)
            extDirtyChannels.append(i)
            if i + 1 not in dirtyChannels:
                extDirtyChannels.append(i + 1)

    print 'Median spread = %f' % medianSpread
    print 'Noise = %f' % noise
    print 'There are %d dirty channels' % len(dirtyChannels)
    dirtyChannels = np.array(extDirtyChannels)
    print 'There are %d dirty channels when extended by 1 channel to either side' % len(dirtyChannels)

    if verbose:
        plt.figure()
        plt.plot(sorted)
        plt.axvline(x=len(dirtyChannels), ymin=0, ymax=1.83)
        plt.title("Sorted phase spread values. Vertical line = cutoff for 'dirty channels'")
        plt.ylabel("Spread value [rad]")
        plt.xlabel("index")

    return dirtyChannels


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


class rfilines(tasks.Task):
    """
    **Description:**

    Get cable delays.

    **Usage:**

    **See also:**
    :class:`plotfootprint`

    **Example:**

    ::
        filefilter="/Volumes/Data/sandertv/VHECR/LORAtriggered/results/VHECR_LORA-20110716T094509.665Z/"
        crfootprint=cr.trun("plotfootprint",filefilter=filefilter,pol=polarization)
   """
    parameters = dict(
        # event={default:None, doc:"Event data filename (.h5), or list of files"},
        filefilter={default: None, doc: "File filter for multiple data files in one event, e.g. '/my/data/dir/L45472_D20120206T030115.786Z*.h5' "},
        filelist={default: None, doc: "List of filenames in one event. "},
        outputDataFile={default: None, doc: "Optional: output text file where results can be written."},
        # batch = {default: False, doc: "Suppress plot display when set to True."},
        doplot={default: True, doc: "Produce output plots"},
        testplots={default: False, doc: "Produce testing plots for calibration on RF lines"},
        pol={default: 0, doc: "0 or 1 for even or odd polarization"},
#        maxlines = {default: 1, doc: "Max number of RF lines to consider"},
        lines={default: None, doc: "(List of) RF line(s) to use, by frequency channel index"},
        antennaselection={default: None, doc: "Optional: list of antenna numbers (RCU/2) to include"},
#        minSNR = {default: 50, doc: "Minimum required SNR of the line"},
        blocksize={default: 8192, doc: "Blocksize of timeseries data, for FFTs"},
        nofblocks={default: 100, doc: "Max. number of blocks to process"},
#        smooth_width = {default: 16, doc: "Smoothing window for FFT, for determining base level (cheap way of doing what AverageSpectrum class does)"},
        referenceTransmitterGPS={default: None, doc: "GPS [long, lat] in degrees (N, E is positive) for a known transmitter. Typically used when tuning to a known frequency. The Smilde tower is at (6.403565, 52.902671)."},
        correctOneSampleShifts={default: False, doc: "Automatically correct for +/- 5 ns shifts in the data. Only works correctly if the direction fit / the reference transmitter GPS is good. Output is in oneSampleShifts list."},
        direction_resolution={default: [1, 5], doc: "Resolution in degrees [az, el] for direction search"},
        directionFromAllStations={default: False, doc: "Set True if you want to use all stations together to calculate the incoming direction. Otherwise, it is done per station and the results are averaged. "},
        freq_range={default: [30, 70], doc: "Frequency range in which to search for calibration sources"},
        # OUTPUT CHANNELS:
        nofchannels={default: -1, doc: "nof channels", output: True},
        medians={default: None, doc: "Median (over all antennas) standard-deviation, per frequency channel. 1-D array with length blocksize/2 + 1.", output: True},
        dirtychannels={default: None, doc: "Output array of dirty channels, based on stability of relative phases from consecutive data blocks. Deviations from uniform-randomness are quite small, unless there is an RF transmitter present at a given frequency.", output: True},
        oneSampleShifts={default: None, doc: "Output array containing the 5 ns-shifts per antenna, if they occur. Output contains integer number of samples the data is shifted. To correct for it, shift data _forward_ by this number of samples.", output: True},
        interStationDelays={default: None, doc: "Output dict containing station name - delay value pairs.", output: True},
        maxPhaseError={default: 1.0, doc: "Maximum allowed phase error in ns^2 (added from all antennas) to call it a good fit and set calibrationStatus to 'OK'."},
        phaseError={default: None, doc: "Phase error for the first station in the list. Used to check if the fit is good, e.g. when using fixed reference transmitter", output: True},
        calibrationStatus={default: None, doc: "Output status string. ", output: True},
        phase_average={default: None, doc: "Output array of average phases for each antenna, per frequency channel, dim = e.g. 48 x 4097", output: True},
        strongestDirection={default: None, doc: "Output list [az, el] with the direction of the strongest transmitter", output: True},
        strongestFrequency={default: None, doc: "The frequency with the smallest phaseRMS, from the range specified in the freq_range parameter", output: True},
        bestPhaseRMS={default: None, doc: "PhaseRMS for the best frequency", output: True},
        timestamp={default: None, doc: "Unix timestamp of input file(s)", output: True},
        average_spectrum={default: None, doc: "Output median over antennas of averaged spectrum (over blocks)", output: True},
#        results={default:lambda self:gatherresults(self.topdir, self.maxspread, self.antennaSet),doc:"Results dict containing cabledelays_database, antenna positions and names"},
#        positions={default:lambda self:obtainvalue(self.results,"positions"),doc:"hArray of dimension [NAnt,3] with Cartesian coordinates of the antenna positions (x0,y0,z0,...)"},
#        antid = {default:lambda self:obtainvalue(self.results,"antid"), doc:"hArray containing strings of antenna ids"},
#        names={default:lambda self:obtainvalue(self.results,"names"),doc:"hArray of dimension [NAnt] with the names or IDs of the antennas"},
        plot_finish={default: lambda self: cr.plotfinish(doplot=True, filename="rfilines", plotpause=False), doc: "Function to be called after each plot to determine whether to pause or not (see ::func::plotfinish)"},
        plot_name={default: "rfilines", doc: "Extra name to be added to plot filename."},
        nofantennas=p_(lambda self: self.positions.shape()[-2], "Number of antennas.", output=True),
        filetype={default: "pdf", doc: "extension/type of plot output files"},
        save_images={default: False, doc: "Enable if images should be saved to disk in default folder"},
#        generate_html = {default:False,doc:"Default output to altair webserver"}
        )

    def call(self):
        pass

    def run(self):

        twopi = 2 * np.pi
        deg2rad = twopi / 360.0
        if self.filelist and self.filefilter:
            raise RuntimeError("Both filelist and filefilter given; provide only one of them")

        if not self.doplot and self.testplots:
            print 'Warning: setting self.testplots to False because self.doplot = False'
            self.testplots = False
        # Fool the OS's disk cache by reading the entire file as binary
        # Hopefully reducing the long I/O processing time (?)
#        bytes_read = open(self.event, "rb").read()

        if self.filelist:
            filelist = self.filelist
        else:
            filelist = cr.listFiles(self.filefilter)
        superterpStations = ["CS002", "CS003", "CS004", "CS005", "CS006", "CS007", "CS021"]
        if len(filelist) == 1:
            print 'There is only 1 file'
            filelist = filelist[0]
        else:  # sort files on station ID
            sortedlist = []
            for station in superterpStations:
                thisStationsFile = [filename for filename in filelist if station in filename]
                if len(thisStationsFile) > 0:
                    sortedlist.append(thisStationsFile[0])
            filelist = sortedlist
        print '[rfilines] Processing files: '
        print filelist
        f = cr.open(filelist, blocksize=self.blocksize)

        # select channels with polarisation 'self.pol'
        if self.antennaselection:
            selected_dipoles = [x for x in f["DIPOLE_NAMES"] if (int(x) % 2 == self.pol) and ((int(x) % 100) / 2 in self.antennaselection)]
        else:
            selected_dipoles = [x for x in f["DIPOLE_NAMES"] if int(x) % 2 == self.pol]

        f["SELECTED_DIPOLES"] = selected_dipoles
#        if isinstance(self.event, list):
#            f.applyClockOffsets()
 #       import pdb; pdb.set_trace()
# test shift:        f.shiftTimeseriesData([-1, 1, 0, -1, 0, -1])
        self.nofchannels = len(selected_dipoles)
        print '# channels = %d' % self.nofchannels
        # get calibration delays from file
        caldelays = hArray(f["DIPOLE_CALIBRATION_DELAY"])  # f[...] outputs list!
        # add subsample clock offsets to caldelays
        if isinstance(filelist, list):
            subsample_clockoffsets = f["SUBSAMPLE_CLOCK_OFFSET"]
            station_startindex = f["STATION_STARTINDEX"]
            for i in range(len(subsample_clockoffsets)):
                start = station_startindex[i]
                end = station_startindex[i + 1]
                caldelays[start:end] += subsample_clockoffsets[i]

        # make calibration phases, i.e. convert delays to phases for every frequency
        freqs = f["FREQUENCY_DATA"]
        calphases = hArray(float, dimensions=[self.nofchannels, len(freqs)])  # check dim.
        cr.hDelayToPhase(calphases, freqs, caldelays)  # check!

#        import pdb; pdb.set_trace()

        if self.doplot and self.testplots:
            plt.figure()
            plt.title('Measured phases per antenna')
            plt.xlabel('Antenna number (RCU/2)')
            plt.ylabel('Phase [rad]')
        nblocks = min(f["DATA_LENGTH"]) / self.blocksize  # Why is this not working?
        nblocks -= 5  # HACK -- why needed?

        if self.nofblocks:
            nblocks = min(self.nofblocks, nblocks)  # limit to # blocks given in params

        print 'Processing %d blocks of length %d' % (nblocks, self.blocksize)
        averagePhasePerAntenna = np.zeros(self.nofchannels / 2)  # do one polarisation; officially: look them up in channel ids!
        n = 0
        phaseblocks = []  # list of arrays of phases (over N freq channels) per antenna

        fftdata = f.empty("FFT_DATA")
        incphasemean = hArray(complex, dimensions=fftdata)
        incphase = hArray(complex, dimensions=incphasemean)

        avgspectrum = hArray(float, dimensions=fftdata)
        spectrum = hArray(complex, dimensions=fftdata)

        skippedblocks = 0
        for i in range(nblocks):
        # accumulate list of arrays of phases, from spectrum of all antennas of every block
        # have to cut out the block with the pulse... autodetect is best?
            print 'Doing block %d of %d' % (i, nblocks)
            f["BLOCK"] = i

            # want to discard blocks with spiky signals... see AverageSpectrum? Or a simple max/sigma test?
#            x = f["TIMESERIES_DATA"]
#            maxx = x.max()[0]
#            stdx = x.stddev()[0]
            f.getFFTData(fftdata, block=i, hanning=True)
            magspectrum = fftdata / f["BLOCKSIZE"]  # normalize
#            magspectrum[..., 0] = 0.0
#            magspectrum[..., 1] = 0.0
            spectrum.copy(magspectrum)

            magspectrum.abs()
#            magspectrum += 1e-9

            incphase.copy(spectrum)
            incphase /= (magspectrum + 1.0e-9)  # divide abs-magnitude out to get exp(i phi)

            incphaseRef = hArray(incphase[0].vec())  # improve? i.e. w/o memory leak. Phases of channel 0 for all freqs
            incphase /= incphaseRef

            incphase[..., 0] = 0.0
            incphase[..., 1] = 0.0

            if np.isnan(incphase.sum()[0]):
                print 'Warning: NaN found! Skipping block'
                skippedblocks += 1
                continue

            incphasemean += incphase

            magspectrum.square()
            avgspectrum += magspectrum

            magspectrum.par.logplot = 'y'
        #    magspectrum.par.xvalues = freqs / 1.0e6
        #    magspectrum.par.xvalues.fillrange(1, 1) # want channel numbers

            if self.doplot and self.testplots and (i == 3):
                plt.figure()
                magspectrum[3].plot()
                plt.title('Power spectrum of one RCU')
        #        plt.xlabel('Channel nr.')
                plt.xlabel('Frequency [MHz]')
                plt.ylabel('Power [a.u.]')
                plt.figure(1)

        nblocks -= skippedblocks
        print 'nblocks = %d' % nblocks
        if nblocks == 0:
            print 'Error: all blocks have been skipped!'
            # may want to exit here

        incPhaseRMS = hArray(float, dimensions=incphasemean)
        incPhaseAvg = hArray(float, dimensions=incphasemean)

        cr.hComplexToPhase(incPhaseAvg, incphasemean)
        incphasemean.abs()
        incPhaseRMS.copy(incphasemean)
        incPhaseRMS *= -1 / float(nblocks)
        incPhaseRMS += 1
        incPhaseRMS.sqrt()
        incPhaseRMS *= np.sqrt(2.0)
        # HACK2
#        import pdb; pdb.set_trace()
        phaseAvg = incPhaseAvg
        phaseRMS = incPhaseRMS

        self.phase_average = cr.hArray(copy=phaseAvg)

        x = phaseRMS.toNumpy()
        medians = np.median(x, axis=0)
        medians[0] = 1.0
        medians[1] = 1.0  # first two channels have been zero'd out; set to 'flat' value to avoid them when taking minimum.
        self.medians = cr.hArray(medians)
        print ' There are %d medians' % len(medians)

        if not self.lines:  # if no value given, take the one with best phase stability
            if not self.freq_range:  # take overall best channel
                bestchannel = medians.argmin()
            else:  # take the best channel in the given range
                f0 = 200.0e6 / self.blocksize  # hardcoded sampling rate
                startindex = int(self.freq_range[0] * 1.0e6 / f0)
                stopindex = 1 + int(self.freq_range[1] * 1.0e6 / f0)
                bestchannel = startindex + medians[startindex:stopindex].argmin()

            bestchannel = int(bestchannel)  # ! Needed for use in hArray slicing etc. Type numpy.int64 not recognized otherwise
        elif isinstance(self.lines, type([])):
            bestchannel = self.lines[0]  # known strong transmitter channel for this event (may vary +/- 5 channels?)
        else:  # either list or number assumed
            bestchannel = self.lines

        channelsSortedByStability = medians.argsort()
        freqsByStability = freqs.toNumpy()[channelsSortedByStability]
        print 'The best 10 channels: '
        for i in range(10):
            print 'Channel %d, freq %2.3f: phase RMS = %1.4f' % (channelsSortedByStability[i], freqsByStability[i] / 1.0e6, medians[channelsSortedByStability[i]])

        print ' Median phase-sigma is lowest (i.e. best phase stability) at channel %d, value = %f' % (bestchannel, medians[bestchannel])
        y = np.median(avgspectrum.toNumpy(), axis=0)
        self.average_spectrum = cr.hArray(y)
        print ' Spectrum is highest at channel %d, log-value = %f' % (y.argmax(), np.log(y.max()))

        if self.testplots:
            # test phase of one antenna in consecutive data blocks, show in plot versus block nr.
            plt.figure()
            x = []
            for block in phaseblocks:
                x.append(block[2, bestchannel])
            x = np.array(x)
            plt.plot(x)
            plt.title('Phase of one antenna for freq channel %d in consecutive data blocks' % bestchannel)

        # Get 'dirty channels' and show in plot
        self.dirtychannels = dirtyChannelsFromPhaseSpreads(medians, verbose=self.testplots)

        # Plot average spectrum for one antenna, and median-RMS phase stability.
        logspectrum = np.log(y)
        if self.doplot:
            plt.figure()
            plt.plot(logspectrum, c='b')
            plt.plot(medians, c='r')
            plt.title('Median-average spectrum of all antennas versus median-RMS phase stability')
            plt.xlabel('Frequency channel')
            plt.ylabel('Blue: log-spectral power [adc units]\nRed: RMS phase stability [rad]')

            # move to testplots?
            self.plot_finish(filename=self.plot_name + "-avgspectrum_phaseRMS", filetype=self.filetype)

            # plot dirty channels into spectrum plot, in red
            x = f["FREQUENCY_DATA"].toNumpy() * 1e-6
            plt.figure()
            plt.plot(x, logspectrum, c='b')
            dirtyspectrum = np.zeros(len(logspectrum))
            dirtyspectrum += np.float('nan')  # min(logspectrum)
            dirtyspectrum[self.dirtychannels] = logspectrum[self.dirtychannels]
            plt.plot(x, dirtyspectrum, 'x', c='r', markersize=8)
            plt.title('Median-average spectrum of all antennas, with flagging')
            plt.xlabel('Frequency [MHz]')
            plt.ylabel('log-spectral power [adc units]')
            self.plot_finish(filename=self.plot_name + "-avgspectrum_withflags", filetype=self.filetype)

            # Plot cleaned spectrum for inspection
            plt.figure()
            cleanedspectrum = np.copy(logspectrum)
            cleanedspectrum[self.dirtychannels] = np.float('nan')
            plt.plot(x, cleanedspectrum, c='b')
            plt.title('Median-average spectrum of all antennas, cleaned')
            plt.xlabel('Frequency [MHz]')
            plt.ylabel('log-spectral power [adc units]')

        if self.testplots:
            # diagnostic test, plot avg phase, also plot all / many individual measured phases
            # at the frequency that gives the best phase stability
            plt.figure()
            x = phaseAvg.toNumpy()[:, bestchannel]
            #...
            plt.plot(x)

        # find direction of this source; plot image to check quality of fit.
        print 'Find direction of source based on averaged phases per antenna...'
#        if self.doplot:
        # do calibration plots
        freqs = f["FREQUENCY_DATA"]
        freq = freqs[bestchannel]

        # apply calibration phases here (?) See if that is different from doing it to all phases before...
        calphases = (twopi * freq) * caldelays
        print calphases
        for i in range(self.nofchannels):
            phaseAvg[i, bestchannel] += calphases[i]
            # if i > 0: # Needed????
             #   phaseAvg[i, bestchannel] -= 0.3

        # phaseAvg[0, bestchannel] = 0.0
        cr.hPhaseWrap(phaseAvg, phaseAvg)

        print '---'
#        print phaseAvg

        allpositions = f["ANTENNA_POSITIONS"].toNumpy()
        azSteps = int(360 / self.direction_resolution[0])
        elSteps = int(90 / self.direction_resolution[1])

        if not isinstance(filelist, list):
        # for one file being processed
            positions = allpositions.ravel()  # only pol 'polarisation'
            # NB. Indexing to the end is done by thisArray[start::step] !

            averagePhasePerAntenna = phaseAvg.toNumpy()[:, bestchannel]
            (fitaz, fitel, minPhaseError) = sf.directionBruteForcePhases(positions, averagePhasePerAntenna, freq, azSteps=azSteps, elSteps=elSteps, allowOutlierCount=4, showImage=(self.doplot and self.testplots), verbose = True)
            if self.doplot and self.testplots:
                self.plot_finish(filename=self.plot_name + "-phaseimage", filetype=self.filetype)

            print 'Best fit az = %f, el = %f' % (fitaz / deg2rad, fitel / deg2rad)
            print 'Phase error = %f' % minPhaseError

            if not self.referenceTransmitterGPS:
                modelphases = sf.phasesFromDirection(positions, (fitaz, fitel), freq)
                modelphases -= modelphases[0]
                modelphases = sf.phaseWrap(modelphases)  # have to wrap again as we subtracted the ref. phase
            else:
                modelphases = sf.timeDelaysFromGPSPoint(self.referenceTransmitterGPS, positions, np.array(f["CHANNEL_ID"]), f["ANTENNA_SET"])
                modelphases -= modelphases[0]
                modelphases *= - twopi * freq  # !! Minus sign to be compatible with FFT's phase sign
                modelphases = sf.phaseWrap(modelphases)

            if self.testplots:
                plt.figure()
                plt.plot(modelphases, label='Modeled phases (plane wave)')
                plt.plot(averagePhasePerAntenna, label='Avg. measured phases')
                plt.legend()
                self.plot_finish(filename=self.plot_name + "-avg-measuredphases", filetype=self.filetype)

            phaseDiff = averagePhasePerAntenna - modelphases

            nanosecondPhase = twopi * freq * 1.0e-9
            timeDiff = sf.phaseWrap(phaseDiff) / nanosecondPhase

            if self.doplot:
                plt.figure()

                plt.plot(timeDiff, label='Measured - expected phase')

                # plt.figure()
                rms_phase = phaseRMS.toNumpy()[:, bestchannel]
                plt.plot(rms_phase, 'r', label='RMS phase noise')
                plt.plot(- rms_phase, 'r')
                plt.title(self.filefilter + '\nPhase difference between measured and best-fit modeled phase\nChannel %d,   f = %2.4f MHz,   pi rad = %1.3f ns' % (bestchannel, freq / 1.0e6, 1.0e9 / (2 * freq)))
                plt.ylabel('Time difference from phase [ns]')
                plt.xlabel('Antenna number (RCU/2)')
                plt.legend()

                self.plot_finish(filename=self.plot_name + "-calibration-phases", filetype=self.filetype)

                cr.trerun("PlotAntennaLayout", "0", positions=f["ANTENNA_POSITIONS"], colors=cr.hArray(list(timeDiff)), sizes=100, title="Measured - expected time", plotlegend=True)
            if self.outputDataFile:
                outf = open(self.outputDataFile, 'a')  # Append to file
                timestamp = f["TIME"][0]
                datestring = datetime.datetime.fromtimestamp(timestamp).strftime('%Y-%m-%d')
                timestring = datetime.datetime.fromtimestamp(timestamp).strftime('%H:%M:%S')
                bestFrequency = freqs[bestchannel] / 1.0e6

                outstr = filelist + ' ' + datestring + ' ' + timestring + ' ' + str(timestamp) + ' ' + format('%3.4f' % bestFrequency) + ' ' + format('%3.2f' % (fitaz / deg2rad)) + ' '
                outstr += format('%3.2f' % (fitel / deg2rad)) + ' ' + format('%3.4f' % minPhaseError) + ' '
                outstr += format('%3.6f' % medians[bestchannel])

                outf.write(outstr + '\n')
                outf.close()

        else:
            # get list of stations in this dataset
            stationlist = f["STATION_LIST"]
            stationStartIndex = f["STATION_STARTINDEX"]
            nofStations = len(stationlist)
            directions = []
            if not self.referenceTransmitterGPS:
            # Get direction fits for each station if no fixed reference transmitter is given.
                for i in range(nofStations):
                    print 'Processing station: %s' % stationlist[i]
                    start = stationStartIndex[i]
                    end = stationStartIndex[i + 1]
                    thesePositions = allpositions[start:end]
                    thesePositions = thesePositions.ravel()

                    averagePhasePerAntenna = phaseAvg.toNumpy()[start:end, bestchannel]
                    plt.figure()
                    (fitaz, fitel, minPhaseError) = sf.directionBruteForcePhases(thesePositions, averagePhasePerAntenna, freq, azSteps=azSteps, elSteps=elSteps, allowOutlierCount=4, showImage=(self.doplot and self.testplots), verbose = True)
                    if self.doplot and self.testplots:
                        self.plot_finish(filename=self.plot_name + "-phaseimage", filetype=self.filetype)

                    print 'Best fit az = %f, el = %f' % (fitaz / deg2rad, fitel / deg2rad)
                    print 'Phase error = %f' % minPhaseError
                    directions.append((fitaz, fitel))

                averageIncomingDirection = vectorAverage(directions)

    # HACK: do incoming direction using all stations together and see if that gets better results...
                if self.directionFromAllStations:
                    averagePhasePerAntenna = phaseAvg.toNumpy()[:, bestchannel]
                    plt.figure()
                    (fitaz, fitel, minPhaseError) = sf.directionBruteForcePhases(allpositions.ravel(), averagePhasePerAntenna, freq, azSteps=azSteps, elSteps=elSteps, allowOutlierCount=4 * nofStations, showImage=(self.doplot and self.testplots), verbose = False)
                    averageIncomingDirection = (fitaz, fitel)

                print 'Averaged incoming direction: az = %f, el = %f' % (averageIncomingDirection[0] / deg2rad, averageIncomingDirection[1] / deg2rad)
            # get modeled phases for a plane wave of given overall direction, for all stations together
            allpositions = allpositions.ravel()

            if not self.referenceTransmitterGPS:
                modelphases = sf.phasesFromDirection(allpositions, averageIncomingDirection, freq)
                modelphases -= modelphases[0]
                modelphases = sf.phaseWrap(modelphases)  # have to wrap again as we subtracted the ref. phase
            else:
                # import pdb; pdb.set_trace()
                modelphases = sf.timeDelaysFromGPSPoint(self.referenceTransmitterGPS, allpositions, np.array(f["SELECTED_DIPOLES"]), f["ANTENNA_SET"][0])
                modelphases -= modelphases[0]
                modelphases *= - twopi * freq  # !! Minus sign to be compatible with FFT's phase sign
                modelphases = sf.phaseWrap(modelphases)

            averagePhasePerAntenna = phaseAvg.toNumpy()[:, bestchannel]  # now for all antennas

            phaseDiff = averagePhasePerAntenna - modelphases
            phaseError = sf.phaseErrorFromDifference(phaseDiff[0:stationStartIndex[1]], freq, allowOutlierCount=4)

            print 'Phase error for first station: %3.3f' % phaseError
            self.phaseError = phaseError
            self.calibrationStatus = "OK" if phaseError < self.maxPhaseError else "BAD"

            nanosecondPhase = twopi * freq * 1.0e-9
            timeDiff = sf.phaseWrap(phaseDiff) / nanosecondPhase

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
                        plt.plot(np.array([start, end]), np.array([interStationDelays[i], interStationDelays[i]]), c='g', lw=3, label='Median station delay')
                else:
                    interStationDelays[i] -= refdelay
                    if self.doplot:
                        plt.plot(np.array([start, end]), np.array([interStationDelays[i], interStationDelays[i]]), c='g', lw=3)
#                    plt.annotate(stationlist[i]
            # Subtract reference station-delay
            timeDiff -= refdelay
            print '--- Inter-station delays: ---'
            self.interStationDelays = {}
            for i in range(nofStations):
                self.interStationDelays[stationlist[i]] = interStationDelays[i]  # write to Task output param
                print '%s: %2.3f ns' % (f["STATION_LIST"][i], interStationDelays[i])

            if self.correctOneSampleShifts:
                self.oneSampleShifts = getOneSampleShifts(timeDiff, stationStartIndex, interStationDelays)
            else:
                self.oneSampleShifts = np.zeros(len(timeDiff))
            timeDiff_glitches = np.zeros(len(timeDiff)) + np.float('nan')
            timeDiff_fixed = timeDiff - 5.0 * self.oneSampleShifts
            for i in range(len(timeDiff)):  # separate fixed data, real data and glitches
                if timeDiff[i] != timeDiff_fixed[i]:
                    timeDiff_glitches[i] = timeDiff[i]

#            timeDiff -= self.oneSampleShifts * 5.0
            # interStationDelays -= interStationDelays[0]
            if self.doplot:
                plt.plot(timeDiff_fixed, 'o-', c='b', label='Measured - expected phase')
                if self.correctOneSampleShifts:
                    plt.plot(timeDiff_glitches, 'o-', c='r', label='5 ns shifts found')
                # plt.figure()
                rms_phase = phaseRMS.toNumpy()[:, bestchannel]
                plt.plot(rms_phase, 'r', label='RMS phase noise')
                plt.plot(- rms_phase, 'r')
                plt.title(self.filefilter + '\nPhase difference between measured and best-fit modeled phase\nChannel %d,   f = %2.4f MHz,   pi rad = %1.3f ns' % (bestchannel, freq / 1.0e6, 1.0e9 / (2 * freq)))
                plt.ylabel('Time difference from phase [ns]')
                plt.xlabel('Antenna number (RCU/2)')
                plt.legend()

                self.plot_finish(filename=self.plot_name + "-calibration-phases", filetype=self.filetype)

                cr.trerun("PlotAntennaLayout", "0", positions=f["ANTENNA_POSITIONS"], colors=cr.hArray(list(timeDiff)), sizes=100, title="Measured - expected time", plotlegend=True)

                self.plot_finish(filename=self.plot_name + "-calibration-layout", filetype=self.filetype)

        # set output params
#        self.oneSampleShifts = getOneSampleShifts(timeDiff, stationStartIndex, interStationDelays)
#        print self.oneSampleShifts
        self.strongestFrequency = freqs[bestchannel]
        # self.strongestDirection = [fitaz / deg2rad, fitel / deg2rad]
        self.bestPhaseRMS = medians[bestchannel]
        self.timestamp = f["TIME"][0]
        print self.interStationDelays
