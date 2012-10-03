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
from pycrtools.tasks.shortcuts import *
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
    sumvector = cr.hArray(float, dimensions = [3])
    azel = cr.hArray(copy = sumvector)
    cartesian = cr.hArray(copy = sumvector)

    for direction in directions:
        azel[0] = direction[0]
        azel[1] = direction[1]
        azel[2] = 1.0 # radius
        cr.hCoordinateConvert(azel, cr.CoordinateTypes.AzElRadius, cartesian, cr.CoordinateTypes.Cartesian, False)
        sumvector += cartesian
#    cr.hCoordinateConvert(sumvector, cr.CoordinateTypes.Cartesian, azel, cr.CoordinateTypes.AzElRadius, False)
    # not implemented in hCoordinateConvert... todo
    meandirection_spherical = pytmf.cartesian2spherical(sumvector[0], sumvector[1], sumvector[2])

    meandirection_azel = (np.pi - (meandirection_spherical[2] + halfpi) , halfpi - (meandirection_spherical[1]))
    
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
    # Sorted plot will have many entries below the uniform-random value of 1.81. 
    # They correspond to the RFI lines. Circumvent taking standard-deviation and peak-detection
    # by estimating the (noise) spread using the higher half of the data
    # then have a criterion of > 3x noise to make an RF-line detection.
    medianSpread = np.median(spreads)
    noise = sorted[length * 0.95] - sorted[length / 2] # value at 95 percentile minus median
    
    dirtyChannels = np.where(spreads < medianSpread - 3*noise)[0]
    print 'Median spread = %f' % medianSpread
    print 'Noise = %f' % noise
    print 'There are %d dirty channels' % len(dirtyChannels)
    if verbose:       
        plt.figure()
        plt.plot(sorted)
        plt.axvline(x = len(dirtyChannels), ymin = 0, ymax = 1.83)
        plt.title("Sorted phase spread values. Vertical line = cutoff for 'dirty channels'")
        plt.ylabel("Spread value [rad]")
        plt.xlabel("index")
        
    return dirtyChannels

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
    parameters=dict(
        #event={default:None, doc:"Event data filename (.h5), or list of files"},
        filefilter={default:None,doc:"File filter for multiple data files in one event, e.g. '/my/data/dir/L45472_D20120206T030115.786Z*.h5' "},
        outputDataFile = {default: None, doc: "Optional: output text file where results can be written."},
        doplot = {default:True, doc:"Produce output plots"},
        testplots = {default: False, doc: "Produce testing plots for calibration on RF lines"},
        pol={default:0,doc:"0 or 1 for even or odd polarization"},
#        maxlines = {default: 1, doc: "Max number of RF lines to consider"},
        lines = {default: None, doc: "(List of) RF line(s) to use, by frequency channel index"},
        antennaselection = {default: None, doc: "Optional: list of antenna numbers (RCU/2) to include"},
#        minSNR = {default: 50, doc: "Minimum required SNR of the line"},
        blocksize = {default: 8192, doc: "Blocksize of timeseries data, for FFTs"},
        nofblocks = {default:100, doc: "Max. number of blocks to process"},
#        smooth_width = {default: 16, doc: "Smoothing window for FFT, for determining base level (cheap way of doing what AverageSpectrum class does)"},
        direction_resolution = {default: [1, 5], doc: "Resolution in degrees [az, el] for direction search"},
        directionFromAllStations = {default: False, doc: "Set True if you want to use all stations together to calculate the incoming direction. Otherwise, it is done per station and the results are averaged. "},
        freq_range = {default: [30, 70], doc: "Frequency range in which to search for calibration sources"}, 
        nofchannels = {default: -1, doc: "nof channels", output:True},
        medians = {default: None, doc: "Median (over all antennas) standard-deviation, per frequency channel. 1-D array with length blocksize/2 + 1.", output: True},
        dirtychannels = {default: None, doc: "Output array of dirty channels, based on stability of relative phases from consecutive data blocks. Deviations from uniform-randomness are quite small, unless there is an RF transmitter present at a given frequency.", output:True},
        phase_average = {default: None, doc: "Output array of average phases for each antenna, per frequency channel, dim = e.g. 48 x 4097", output: True},
        strongestDirection = {default: None, doc: "Output list [az, el] with the direction of the strongest transmitter", output: True},
        strongestFrequency = {default: None, doc: "The frequency with the smallest phaseRMS, from the range specified in the freq_range parameter", output: True},
        bestPhaseRMS = {default: None, doc: "PhaseRMS for the best frequency", output: True},
        timestamp = {default: None, doc: "Unix timestamp of input file(s)", output: True},
        average_spectrum = {default: None, doc: "Output median over antennas of averaged spectrum (over blocks)", output: True},
#        results=p_(lambda self:gatherresults(self.topdir, self.maxspread, self.antennaSet),doc="Results dict containing cabledelays_database, antenna positions and names"),
#        positions=p_(lambda self:obtainvalue(self.results,"positions"),doc="hArray of dimension [NAnt,3] with Cartesian coordinates of the antenna positions (x0,y0,z0,...)"),
#        antid = p_(lambda self:obtainvalue(self.results,"antid"), doc="hArray containing strings of antenna ids"),
#        names=p_(lambda self:obtainvalue(self.results,"names"),doc="hArray of dimension [NAnt] with the names or IDs of the antennas"),
        plot_finish={default: lambda self:cr.plotfinish(doplot=True,filename="rfilines",plotpause=False),doc:"Function to be called after each plot to determine whether to pause or not (see ::func::plotfinish)"},
        plot_name={default:"rfilines",doc:"Extra name to be added to plot filename."},
        nofantennas=p_(lambda self: self.positions.shape()[-2],"Number of antennas.",output=True),
        filetype={default:"pdf",doc:"extension/type of plot output files"},
        save_images = {default:False,doc:"Enable if images should be saved to disk in default folder"},
#        generate_html = {default:False,doc:"Default output to altair webserver"}
        
        )
                
    def call(self):
        pass

    def run(self):

        twopi = 2 * np.pi
        deg2rad = twopi / 360.0
        
        # Fool the OS's disk cache by reading the entire file as binary
        # Hopefully reducing the long I/O processing time (?)
#        bytes_read = open(self.event, "rb").read()
        
        filelist = cr.listFiles(self.filefilter)
        superterpStations = ["CS002", "CS003", "CS004", "CS005", "CS006", "CS007"]
        if len(filelist) == 1:
            filelist = filelist[0]
        else: # sort files on station ID
            sortedlist = []
            for station in superterpStations:
                thisStationsFile = [filename for filename in filelist if station in filename]
                if len(thisStationsFile) > 0:
                    sortedlist.append(thisStationsFile[0])
            filelist = sortedlist
        print '[rfilines] Processing files: '
        print filelist
        f = cr.open(filelist, blocksize = self.blocksize)
        #f = open('/Users/acorstanje/triggering/CR/L29590_D20110714T174749.986Z_CS005_R000_tbb.h5')
        #f = open('/Users/acorstanje/triggering/CR/L43181_D20120120T191949.209Z_CS003_R000_tbb.h5')
#        f = open('/Users/acorstanje/triggering/CR/L40797_D20111228T200122.223Z_CS007_R000_tbb.h5')
        #f = open('/Users/acorstanje/triggering/CR/L57524_D20120428T050021.699Z_CS030_R000_tbb.h5')
        #f = open('/Users/acorstanje/triggering/CR/L44792_D20120204T002716.906Z_CS003_R000_tbb.h5')
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
        caldelays = hArray(f["DIPOLE_CALIBRATION_DELAY"]) # f[...] outputs list!
        # add subsample clock offsets to caldelays
        if isinstance(filelist, list):
            subsample_clockoffsets = f["SUBSAMPLE_CLOCK_OFFSET"]
            station_startindex = f["STATION_STARTINDEX"]
            for i in range(len(subsample_clockoffsets)):
                start = station_startindex[i]
                end = station_startindex[i+1]
                caldelays[start:end] += subsample_clockoffsets[i]
            
        # make calibration phases, i.e. convert delays to phases for every frequency
        freqs = f["FREQUENCY_DATA"]
        calphases = hArray(float, dimensions = [self.nofchannels, len(freqs)]) # check dim.
        cr.hDelayToPhase(calphases, freqs, caldelays) # check!
        
#        import pdb; pdb.set_trace()
        
        if self.doplot and self.testplots:
            plt.figure()
            plt.title('Measured phases per antenna')
            plt.xlabel('Antenna number (RCU/2)')
            plt.ylabel('Phase [rad]')
        nblocks = min(f["DATA_LENGTH"]) / self.blocksize # Why is this not working?
        nblocks -= 5 # HACK -- why needed?

        if self.nofblocks:
            nblocks = min(self.nofblocks, nblocks) # limit to # blocks given in params
        
        print 'Processing %d blocks of length %d' % (nblocks, self.blocksize)
        averagePhasePerAntenna = np.zeros(self.nofchannels / 2) # do one polarisation; officially: look them up in channel ids!
        n = 0
        # Line selection happens automatically using phase-stability measure (RMS)
#        if type(self.lines) == type([]):
#            fchannel = self.lines[0] # known strong transmitter channel for this event (may vary +/- 5 channels?)
#        elif self.lines: # either list or number assumed
#            fchannel = self.lines
#        freq = freqs[fchannel]
#        calphases = (twopi * freq) * caldelays

        #measuredphases = np.zeros((nblocks, self.nofchannels))
        phaseblocks = [] # list of arrays of phases (over N freq channels) per antenna
        
        fftdata = f.empty("FFT_DATA")
        incphasemean = hArray(complex, dimensions = fftdata)
        incphase = hArray(complex, dimensions = incphasemean)
        
        avgspectrum = hArray(float, dimensions = fftdata)
        spectrum = hArray(complex, dimensions = fftdata)
        for i in range(nblocks):
        # accumulate list of arrays of phases, from spectrum of all antennas of every block
        # have to cut out the block with the pulse... autodetect is best? 
            print 'Doing block %d of %d' % (i, nblocks)
            f["BLOCK"] = i
            
            # want to discard blocks with spiky signals... see AverageSpectrum? Or a simple max/sigma test?
#            x = f["TIMESERIES_DATA"]
#            maxx = x.max()[0]
#            stdx = x.stddev()[0]
            f.getFFTData(fftdata, block = i, hanning = True)
            magspectrum = fftdata / f["BLOCKSIZE"] # normalize
            magspectrum[..., 0] = 0.0
            magspectrum[..., 1] = 0.0
            spectrum.copy(magspectrum)

            magspectrum.abs()
#            magspectrum += 1e-9

            incphase.copy(spectrum)
            incphase /= (magspectrum + 1.0e-9) # divide magnitude out to get exp(i phi)

            incphaseRef = hArray(incphase[0].vec()) # improve? i.e. w/o memory leak. Phases of channel 0 for all freqs
            incphase /= incphaseRef

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

            # s[...].maxpos()
            #f["FREQUENCY_DATA"][5058]
            #f["FREQUENCY_DATA"][4538]
            
            # Get phases from spectrum
#            phases = cr.hArray(float, dimensions = spectrum) # dim. 48 x 4096 typically; N_ant x N_freqs
            # has to be recreated here because they are all stored into 'phaseblocks' list
#            cr.hComplexToPhase(phases, spectrum)
            # Apply calibration phases
            #phases -= calphases
            # subtract reference phase, which is phases[0, ...]

#            phaseRef = hArray(phases[0].vec()) # improve? i.e. w/o memory leak. Phases of channel 0 for all freqs
#            phases -= phaseRef
#            for i in range(1, self.nofchannels):
#                phases[i, ...] -= phases[0, ...] # any way to do this without loop?
                
#            phases[0, ...] = 0.0
            # wrap phases again into [-pi, pi]
#            cr.hPhaseWrap(phases, phases) # make that an in-place function in mMath...
            
#            phaseblocks.append(phases)
                   
#        phaseSum = hArray(float, dimensions = phaseblocks[0]) # dim. N_ant x N_freq (e.g. 48 x 4096)
#        wrapped = hArray(float, dimensions = phaseSum)
#        phaseRMS = hArray(float, dimensions = phaseSum) # RMS per antenna per freq channel
#        phizero = phaseblocks[0] # subtract that from all phases; then wrap, then add to sum, then average, then add phi[0] again, then wrap... Done! Hope it works.
#        n = 0
#        for phases in phaseblocks:
#            print 'Mean: Doing block %d' % n
#            n += 1
            # do this without loop! Also RMS. Then, no RAM storage of all the spectra is needed!
            # Calculate mean: first calculate sum, then wrap everything into (-pi, pi), then... ???
#            wrapped.copy(phases)
#            wrapped -= phizero
#            cr.hPhaseWrap(wrapped, wrapped)
            
#            phaseSum += wrapped
        
#        phaseAvg = phaseSum / len(phaseblocks)
#        phaseAvg += phizero
#        cr.hPhaseWrap(phaseAvg, phaseAvg)
        
        #linephase_avg.copy(phaseAvg[..., 1111])
        #linephase.copy(phaseblocks[0][..., 1111]) doesnt work!
        
        #HACK 
#        phaseAvg = phaseblocks[10]

        # now do RMS. Sum (phi - mu)^2 where phi - mu is wrapped first
#        n = 0
#        for phases in phaseblocks:
#            print 'RMS: doing block %d' % n
#            n += 1
#            wrapped = phases - phaseAvg
#            cr.hPhaseWrap(wrapped, wrapped)
#            wrapped.square()
#            phaseRMS += wrapped
            
#        phaseRMS /= (len(phaseblocks) - 1)
#        phaseRMS.sqrt()
        
        # to numpy for transpose, then take median RMS over antennas
        # Median is resistant to outliers and gives a good indication of the line phase quality over all antennas
        # that's all we need from it.
        
        # HACK
        incPhaseRMS = hArray(float, dimensions = incphasemean)
        incPhaseAvg = hArray(float, dimensions = incphasemean)
        
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
        
        self.phase_average = cr.hArray(copy = phaseAvg)
        
        x = phaseRMS.toNumpy()
        medians = np.median(x, axis=0)
        medians[0] = 1.0
        medians[1] = 1.0 # first two channels have been zero'd out; set to 'flat' value to avoid them when taking minimum.
        self.medians = cr.hArray(medians)
        print ' There are %d medians' % len(medians)
   
        if not self.lines: # if no value given, take the one with best phase stability
            if not self.freq_range: # take overall best channel
                bestchannel = medians.argmin()
            else: # take the best channel in the given range
                f0 = 200.0e6 / self.blocksize # hardcoded sampling rate
                startindex = int(self.freq_range[0] * 1.0e6 / f0)
                stopindex = 1 + int(self.freq_range[1] * 1.0e6 / f0) 
                bestchannel = startindex + medians[startindex:stopindex].argmin()

            bestchannel = int(bestchannel) # ! Needed for use in hArray slicing etc. Type numpy.int64 not recognized otherwise
        elif type(self.lines) == type([]):
            bestchannel = self.lines[0] # known strong transmitter channel for this event (may vary +/- 5 channels?)
        else: # either list or number assumed
            bestchannel = self.lines

        channelsSortedByStability = medians.argsort()
        freqsByStability = freqs.toNumpy()[channelsSortedByStability]
        print 'The best 10 channels: '
        for i in range(10):
            print 'Channel %d, freq %2.3f: phase RMS = %1.4f' % (channelsSortedByStability[i], freqsByStability[i] / 1.0e6, medians[channelsSortedByStability[i]])

        print ' Median phase-sigma is lowest (i.e. best phase stability) at channel %d, value = %f' % (bestchannel, medians[bestchannel])
        y = np.median(avgspectrum.toNumpy(), axis=0)
        self.average_spectrum = cr.hArray(y)
#        import pdb; pdb.set_trace()
        print ' Spectrum is highest at channel %d, log-value = %f' % (y.argmax(), np.log(y.max()))

#        if self.testplots:
#            plt.figure()
            # test against one block at one line, make figure
#            linephase_avg = hArray(float, dimensions=[self.nofchannels])
#            linephase = hArray(float, dimensions = [self.nofchannels])
            
#            for i in range(self.nofchannels):
#                linephase_avg[i] = phaseAvg[i, bestchannel]
#            plt.plot(linephase_avg.toNumpy(), c='r')
                
#            for block in phaseblocks:
#                for j in range(self.nofchannels):
#                    linephase[j] = block[j, bestchannel]
#                plt.plot(linephase.toNumpy())

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
        plt.figure()
        plt.plot(logspectrum, c='b')
        plt.plot(medians, c='r')
        plt.title('Median-average spectrum of all antennas versus median-RMS phase stability')
        plt.xlabel('Frequency channel')
        plt.ylabel('Blue: log-spectral power [adc units]\nRed: RMS phase stability [rad]')
    
        self.plot_finish(filename=self.plot_name + "-avgspectrum_phaseRMS",filetype=self.filetype)

    
        # plot dirty channels into spectrum plot, in red
        plt.figure()
        x = f["FREQUENCY_DATA"].toNumpy() * 1e-6
        plt.plot(x, logspectrum, c='b')
        dirtyspectrum = np.zeros(len(logspectrum))
        dirtyspectrum += min(logspectrum)
        dirtyspectrum[self.dirtychannels] = logspectrum[self.dirtychannels]
        plt.plot(x, dirtyspectrum, 'x', c = 'r', markersize=8)
        plt.title('Median-average spectrum of all antennas, with flagging')
        plt.xlabel('Frequency [MHz]')
        plt.ylabel('log-spectral power [adc units]')
        self.plot_finish(filename=self.plot_name + "-avgspectrum_withflags",filetype=self.filetype)
        
        if self.testplots:
            # diagnostic test, plot avg phase, also plot all / many individual measured phases
            # at the frequency that gives the best phase stability
            plt.figure()
            x = phaseAvg.toNumpy()[:, bestchannel]
            #...
            plt.plot(x)
        
        # find direction of this source; plot image to check quality of fit.
        print 'Find direction of source based on averaged phases per antenna...'
        if self.doplot:
        # do calibration plots
            freqs = f["FREQUENCY_DATA"]
            freq = freqs[bestchannel]

            # apply calibration phases here (?) See if that is different from doing it to all phases before...
            calphases = (twopi * freq) * caldelays
            print calphases
            for i in range(self.nofchannels):
                phaseAvg[i, bestchannel] += calphases[i]
                #if i > 0: # Needed????
                 #   phaseAvg[i, bestchannel] -= 0.3
            
            #phaseAvg[0, bestchannel] = 0.0
            cr.hPhaseWrap(phaseAvg, phaseAvg)
            
            print '---'
    #        print phaseAvg
    #        import pdb; pdb.set_trace()
            
            allpositions = f["ANTENNA_POSITIONS"].toNumpy()
            azSteps = int(360 / self.direction_resolution[0])
            elSteps = int(90 / self.direction_resolution[1])
            
            if not isinstance(filelist, list):
            # for one file being processed
                positions = allpositions.ravel() # only pol 'polarisation'
                # NB. Indexing to the end is done by thisArray[start::step] !
                
                averagePhasePerAntenna = phaseAvg.toNumpy()[:, bestchannel]
                (fitaz, fitel, minPhaseError) = sf.directionBruteForcePhases(positions, averagePhasePerAntenna, freq, azSteps = azSteps, elSteps = elSteps, allowOutlierCount = 1, showImage = self.testplots, verbose = True)
                self.plot_finish(filename=self.plot_name + "-phaseimage",filetype=self.filetype)

                print 'Best fit az = %f, el = %f' % (fitaz / deg2rad, fitel / deg2rad)
                print 'Phase error = %f' % minPhaseError
                
                modelphases = sf.phasesFromDirection(positions, (fitaz, fitel), freq)
                modelphases -= modelphases[0]
                modelphases = sf.phaseWrap(modelphases) # have to wrap again as we subtracted the ref. phase

                if self.testplots:
                    plt.figure()
                    plt.plot(modelphases, label='Modeled phases (plane wave)')
                    plt.plot(averagePhasePerAntenna, label='Avg. measured phases')
                    plt.legend()
                    self.plot_finish(filename=self.plot_name + "-avg-measuredphases",filetype=self.filetype)

                phaseDiff = averagePhasePerAntenna - modelphases
                
                plt.figure()
                nanosecondPhase = twopi * freq * 1.0e-9
                timeDiff = sf.phaseWrap(phaseDiff) / nanosecondPhase
                
                plt.plot(timeDiff, label='Measured - expected phase')

                #plt.figure()
                rms_phase = phaseRMS.toNumpy()[:, bestchannel]
                plt.plot(rms_phase, 'r', label='RMS phase noise')
                plt.plot( - rms_phase, 'r')
                plt.title(self.filefilter+'\nPhase difference between measured and best-fit modeled phase\nChannel %d,   f = %2.4f MHz,   pi rad = %1.3f ns' % (bestchannel, freq/1.0e6, 1.0e9 / (2 * freq)))
                plt.ylabel('Time difference from phase [ns]')
                plt.xlabel('Antenna number (RCU/2)')
                plt.legend()
                
                self.plot_finish(filename=self.plot_name + "-calibration-phases",filetype=self.filetype)
                
                if self.doplot:
                    cr.trerun("PlotAntennaLayout","0", positions = f["ANTENNA_POSITIONS"], colors = cr.hArray(list(timeDiff)), sizes=100, title="Measured - expected time",plotlegend=True)
                if self.outputDataFile:
                    outf = open(self.outputDataFile, 'a') # Append to file
                    timestamp = f["TIME"][0]
                    datestring = datetime.datetime.fromtimestamp(timestamp).strftime('%Y-%m-%d')
                    timestring = datetime.datetime.fromtimestamp(timestamp).strftime('%H:%M:%S')
                    bestFrequency = freqs[bestchannel]

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
                for i in range(nofStations):
                    print 'Processing station: %s' % stationlist[i]
                    start = stationStartIndex[i]
                    end = stationStartIndex[i+1]
                    thesePositions = allpositions[start:end]
                    thesePositions = thesePositions.ravel()

                    averagePhasePerAntenna = phaseAvg.toNumpy()[start:end, bestchannel]
                    plt.figure()
                    (fitaz, fitel, minPhaseError) = sf.directionBruteForcePhases(thesePositions, averagePhasePerAntenna, freq, azSteps = azSteps, elSteps = elSteps, allowOutlierCount = 4, showImage = self.testplots, verbose = True)
                    self.plot_finish(filename=self.plot_name + "-phaseimage",filetype=self.filetype)                   
                    
                    print 'Best fit az = %f, el = %f' % (fitaz / deg2rad, fitel / deg2rad)
                    print 'Phase error = %f' % minPhaseError                    
                    directions.append((fitaz, fitel))
                    
                averageIncomingDirection = vectorAverage(directions)
                
# HACK: do incoming direction using all stations together and see if that gets better results...
                if self.directionFromAllStations:
                    averagePhasePerAntenna = phaseAvg.toNumpy()[:, bestchannel]
                    plt.figure()
                    (fitaz, fitel, minPhaseError) = sf.directionBruteForcePhases(allpositions.ravel(), averagePhasePerAntenna, freq, azSteps = azSteps, elSteps = elSteps, allowOutlierCount = 4*nofStations, showImage = self.testplots, verbose = False)
                    averageIncomingDirection = (fitaz, fitel)
                
                print 'Averaged incoming direction: az = %f, el = %f' % (averageIncomingDirection[0] / deg2rad, averageIncomingDirection[1] / deg2rad)
                # get modeled phases for a plane wave of given overall direction, for all stations together
                allpositions = allpositions.ravel()
                modelphases = sf.phasesFromDirection(allpositions, averageIncomingDirection, freq)
                modelphases -= modelphases[0]
                modelphases = sf.phaseWrap(modelphases) # have to wrap again as we subtracted the ref. phase
                
                averagePhasePerAntenna = phaseAvg.toNumpy()[:, bestchannel] # now for all antennas
                phaseDiff = averagePhasePerAntenna - modelphases
                
                plt.figure()
                nanosecondPhase = twopi * freq * 1.0e-9
                timeDiff = sf.phaseWrap(phaseDiff) / nanosecondPhase
                
                interStationDelays = np.zeros(nofStations)
                refdelay = 0.0
                for i in range(nofStations):
                    start = stationStartIndex[i]
                    end = stationStartIndex[i+1]
                    interStationDelays[i] = np.median(timeDiff[start:end])
                    if i == 0:
                        refdelay = interStationDelays[i]
                        interStationDelays[i] -= refdelay
                        plt.plot(np.array([start, end]), np.array([interStationDelays[i], interStationDelays[i]]), c='g', lw=3, label='Median station delay')
                    else:
                        interStationDelays[i] -= refdelay
                        plt.plot(np.array([start, end]), np.array([interStationDelays[i], interStationDelays[i]]), c='g', lw=3)
#                    plt.annotate(stationlist[i]
                # Subtract reference station-delay
                timeDiff -= refdelay   
                #interStationDelays -= interStationDelays[0]
                plt.plot(timeDiff, 'o-', c = 'b', label='Measured - expected phase')

                #plt.figure()
                rms_phase = phaseRMS.toNumpy()[:, bestchannel]
                plt.plot(rms_phase, 'r', label='RMS phase noise')
                plt.plot( - rms_phase, 'r')
                plt.title(self.filefilter+'\nPhase difference between measured and best-fit modeled phase\nChannel %d,   f = %2.4f MHz,   pi rad = %1.3f ns' % (bestchannel, freq/1.0e6, 1.0e9 / (2 * freq)))
                plt.ylabel('Time difference from phase [ns]')
                plt.xlabel('Antenna number (RCU/2)')
                plt.legend()
                
                print '--- Inter-station delays: ---'
                for i in range(nofStations):
                    print '%s: %2.3f ns' % (f["STATION_LIST"][i], interStationDelays[i])
                
                cr.trerun("PlotAntennaLayout","0", positions = f["ANTENNA_POSITIONS"], colors = cr.hArray(list(timeDiff)), sizes=100, title="Measured - expected time",plotlegend=True)

        # set output params
        self.strongestFrequency = freqs[bestchannel]
        self.strongestDirection = [fitaz / deg2rad, fitel / deg2rad]
        self.bestPhaseRMS = medians[bestchannel]
        self.timestamp = f["TIME"][0]
                
"""                       

                    

            y = phases.toNumpy()
            y = np.angle(y)
            y += calphases # subtract phases from delay calibration metadata (needs a + sign, why?)
            
            ph[i] = y[27] # just to print differences across blocks for one antenna
            y -= y[self.pol] # subtract reference phase y[0] or y[1] (polarisation)
            
            wrapped = y - twopi * (y / twopi).round() # wrap into [-pi, +pi]
          
            # NB. Cannot average sequence of phases just like that; when having e.g. phi = +/- pi + noise, 
            # a spurious term -/+ k (2pi / n) will come in, depending on the number of phase wraps k (i.e. on noise). 
            # Therefore, force consecutive phases to have abs-difference < pi
            if n > 0:
                wrapped -= twopi * ((wrapped - lastwrapped) / twopi).round()
            measuredphases[i] = np.copy(wrapped)
            
            averagePhasePerAntenna += wrapped[self.pol::2]
            n+=1

            if self.doplot:
                plt.plot(wrapped[self.pol::2])

            lastwrapped = np.copy(wrapped) # for disambiguating phases around wrapping points +/- pi (+ noise)

            delta = y[32] - y[0]
            print 'Block %d: test phase = %f, maxx = %f, stddev = %f' % (i, delta -  2*np.pi * round(delta / (2*np.pi)), maxx, stdx) # crude test for phase stability

        averagePhasePerAntenna /= n
        #averagePhasePerAntenna = wrapped[polarisation::2] # hack, averaging may be ambiguous with the 2-pi wraparound (?)

        if self.doplot:
            plt.figure()
            plt.plot(np.unwrap(ph))
            plt.title('Unwrapped phase of one antenna from consecutive blocks')
            plt.ylabel('Phase [rad]')
            plt.xlabel('block nr.')

        # find direction of this source; plot image to check quality of fit.

        print 'Find direction of source based on averaged phases per antenna...'
        allpositions = f["ANTENNA_POSITIONS"].toNumpy()
        positions = allpositions[self.pol::2].ravel() # only pol 'polarisation'
        # NB. Indexing to the end is done by thisArray[start::step] !
        
        azSteps = int(360 / self.direction_resolution[0])
        elSteps = int(90 / self.direction_resolution[1])
        (fitaz, fitel, minPhaseError) = sf.directionBruteForcePhases(positions, averagePhasePerAntenna, freq, azSteps = azSteps, elSteps = elSteps, showImage = self.doplot, verbose = True)

        print 'Best fit az = %f, el = %f' % (fitaz / deg2rad, fitel / deg2rad)
        print 'Phase error = %f' % minPhaseError

        modelphases = sf.phasesFromDirection(positions, (fitaz, fitel), freq)
        modelphases -= modelphases[0]
        modelphases = sf.phaseWrap(modelphases) # have to wrap again as we subtracted the ref. phase
        phaseDiff = averagePhasePerAntenna - modelphases
 
        rms_phase = measuredphases.std(axis=0) / np.sqrt(n)

        if self.doplot:
            plt.figure()
            plt.plot(modelphases, label='Modeled phases (plane wave)')
            plt.plot(averagePhasePerAntenna, label='Avg. measured phases')
            plt.legend()

            plt.figure()
            plt.plot(sf.phaseWrap(phaseDiff), label='Measured - expected phase')

            #plt.figure()
            plt.plot(rms_phase[self.pol::2], 'r', label='RMS phase noise')
            plt.plot( - rms_phase[self.pol::2], 'r')
            nanosecondPhase = twopi * freq * 1.0e-9
            plt.title('Phase difference between measured and best-fit modeled phase\n1 ns = %1.3f rad' % nanosecondPhase)
            plt.ylabel('Phase [rad]')
            plt.xlabel('Antenna number (RCU/2)')
            plt.legend()
              
"""                    
