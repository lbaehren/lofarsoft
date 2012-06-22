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

def getLines(dirtychannels, blocksize_wanted, blocksize_used):
    # list of dirty channels in, convert blocksizes if needed.
    ch = np.array(dirtychannels) * 1.0
    ch /= float(blocksize_used / blocksize_wanted)
    ch = round(ch)

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
        event={default:None, doc:"Event data filename (.h5)"},
#        filefilter={default:None,doc:"Obtains results from subdirectories of these files (from results.py)"},
        doplot = {default:True, doc:"Produce output plots"},
        pol={default:0,doc:"0 or 1 for even or odd polarization"},
        maxlines = {default: 1, doc: "Max number of RF lines to consider"},
        lines = {default: None, doc: "(List of) RF line(s) to use, by frequency channel index"},
        dirtychannels = {default: None, doc: "List of 'dirty channels' as from cr_event pipeline. One radio transmitter line can be resolved into many 'dirty channels' if spectral resolution is high enough. The line has to be identified from this afterwards."},
        minSNR = {default: 50, doc: "Minimum required SNR of the line"},
        blocksize = {default: 8192, doc: "Blocksize of timeseries data, for FFTs"},
        smooth_width = {default: 16, doc: "Smoothing window for FFT, for determining base level (cheap way of doing what AverageSpectrum class does)"},
        direction_resolution = {default: [1, 5], doc: "Resolution in degrees [az, el] for direction search"},
         
        nofchannels = {default:96, doc: "nof channels (make ouput param!)"},
        results=p_(lambda self:gatherresults(self.topdir, self.maxspread, self.antennaSet),doc="Results dict containing cabledelays_database, antenna positions and names"),
        positions=p_(lambda self:obtainvalue(self.results,"positions"),doc="hArray of dimension [NAnt,3] with Cartesian coordinates of the antenna positions (x0,y0,z0,...)"),
        antid = p_(lambda self:obtainvalue(self.results,"antid"), doc="hArray containing strings of antenna ids"),
        names=p_(lambda self:obtainvalue(self.results,"names"),doc="hArray of dimension [NAnt] with the names or IDs of the antennas"),
        plot_finish={default: lambda self:cr.plotfinish(doplot=True,filename="cabledelays",plotpause=False),doc:"Function to be called after each plot to determine whether to pause or not (see ::func::plotfinish)"},
        plot_name={default:"cabledelays",doc:"Extra name to be added to plot filename."},
        nofantennas=p_(lambda self: self.positions.shape()[-2],"Number of antennas.",output=True),
        filetype={default:"png",doc:"extension/type of output file"},
        save_images = {default:False,doc:"Enable if images should be saved to disk in default folder"},
        generate_html = {default:False,doc:"Default output to altair webserver"}
        
        )
                
    def call(self):
        pass

    def run(self):

        twopi = 2 * np.pi
        deg2rad = twopi / 360.0
        
        # Fool the OS's disk cache by reading the entire file as binary
        # Hopefully reducing the ridiculously long I/O processing time (> 1 or 2 minutes for some 190 MB...)
        bytes_read = open(self.event, "rb").read()
        
        f = cr.open(self.event)
        
        
        
        #f = open('/Users/acorstanje/triggering/CR/L29590_D20110714T174749.986Z_CS005_R000_tbb.h5')
        #f = open('/Users/acorstanje/triggering/CR/L43181_D20120120T191949.209Z_CS003_R000_tbb.h5')
#        f = open('/Users/acorstanje/triggering/CR/L40797_D20111228T200122.223Z_CS007_R000_tbb.h5')
        #f = open('/Users/acorstanje/triggering/CR/L57524_D20120428T050021.699Z_CS030_R000_tbb.h5')
        #f = open('/Users/acorstanje/triggering/CR/L44792_D20120204T002716.906Z_CS003_R000_tbb.h5')
        f["BLOCKSIZE"] = self.blocksize
        # select channels with polarisation 'self.pol'
        selected_dipoles = [x for x in f["DIPOLE_NAMES"] if int(x) % 2 == self.pol]
        f["SELECTED_DIPOLES"] = selected_dipoles
        self.nofchannels = len(f["CHANNEL_ID"])
        print self.nofchannels

        # get calibration delays from file
        caldelays = hArray(f["DIPOLE_CALIBRATION_DELAY"]) # f[...] outputs list!
        # make calibration phases, i.e. convert delays to phases for every frequency
        freqs = f["FREQUENCY_DATA"]
        calphases = hArray(float, dimensions = [self.nofchannels, len(freqs)]) # check dim.
        cr.hDelayToPhase(calphases, freqs, caldelays) # check!

        if self.doplot:
            plt.figure()
            plt.title('Measured phases per antenna')
            plt.xlabel('Antenna number (RCU/2)')
            plt.ylabel('Phase [rad]')
        nblocks = min(f["DATA_LENGTH"]) / self.blocksize # Why is this not working?
        nblocks -= 10 # HACK
        
        print 'Processing %d blocks of length %d' % (nblocks, self.blocksize)
        ph = np.zeros(nblocks)
        averagePhasePerAntenna = np.zeros(self.nofchannels / 2) # do one polarisation; officially: look them up in channel ids!
        n = 0
        # for the time being take one frequency line
        if type(self.lines) == type([]):
            fchannel = self.lines[0] # known strong transmitter channel for this event (may vary +/- 5 channels?)
        elif self.lines: # either list or number assumed
            fchannel = self.lines
        freq = freqs[fchannel]
#        calphases = (twopi * freq) * caldelays

        #measuredphases = np.zeros((nblocks, self.nofchannels))
        phaseblocks = [] # list of arrays of phases (over N freq channels) per antenna
        avgspectrum = hArray(float, dimensions = f["FFT_DATA"])
        spectrum = hArray(complex, dimensions = f["FFT_DATA"])
        for i in range(nblocks):
        # accumulate list of arrays of phases, from spectrum of all antennas of every block
        # have to cut out the block with the pulse... autodetect is best? 
            print 'Doing block %d of %d' % (i, nblocks)
            f["BLOCK"] = i
            
#            x = f["TIMESERIES_DATA"]
#            maxx = x.max()[0]
#            stdx = x.stddev()[0]

            magspectrum = f["FFT_DATA"] / f["BLOCKSIZE"] # normalize
            magspectrum[..., 0] = 0.0
            magspectrum[..., 1] = 0.0
            spectrum.copy(magspectrum)
            magspectrum.abs()
            magspectrum.square()
            avgspectrum += magspectrum

            magspectrum.par.logplot = 'y'
        #    magspectrum.par.xvalues = freqs / 1.0e6
        #    magspectrum.par.xvalues.fillrange(1, 1) # want channel numbers
            
            if self.doplot and (i == 3):
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
            phases = cr.hArray(float, dimensions = spectrum) # dim. 48 x 4096 typically; N_ant x N_freqs
            cr.hComplexToPhase(phases, spectrum)
            # Apply calibration phases
            phases += calphases
            # subtract reference phase, which is phases[0, ...]
            for i in range(1, self.nofchannels):
                phases[i, ...] -= phases[0, ...] # any way to do this without loop?
            # wrap phases again into [-pi, pi]
            cr.hPhaseWrap(phases, phases) # make that an in-place function in mMath...
            
            phaseblocks.append(phases)
                   
        phaseSum = hArray(float, dimensions = phaseblocks[0]) # dim. N_ant x N_freq (e.g. 48 x 4096)
        wrapped = hArray(float, dimensions = phaseSum)
        phaseRMS = hArray(float, dimensions = phaseSum) # RMS per antenna per freq channel
        phizero = phaseblocks[0] # subtract that from all phases; then wrap, then add to sum, then average, then add phi[0] again, then wrap... Done! Hope it works.
        n = 0
        for phases in phaseblocks:
            print 'Mean: Doing block %d' % n
            n += 1
            # do this without loop! Also RMS. Then, no RAM storage of all the spectra is needed!
            # Calculate mean: first calculate sum, then wrap everything into (-pi, pi), then... ???
            wrapped.copy(phases)
            wrapped -= phizero
            cr.hPhaseWrap(wrapped, wrapped)
            
            phaseSum += wrapped
            
        phaseAvg = phaseSum / len(phaseblocks)
        phaseAvg += phizero
        cr.hPhaseWrap(phaseAvg, phaseAvg)
        
        # now do RMS. Sum (phi - mu)^2 where phi - mu is wrapped first
        n = 0
        for phases in phaseblocks:
            print 'RMS: doing block %d' % n
            n += 1
            wrapped = phases - phaseAvg
            cr.hPhaseWrap(wrapped, wrapped)
            wrapped.square()
            phaseRMS += wrapped
            
        phaseRMS /= (len(phaseblocks) - 1)
        phaseRMS.sqrt()
        
        # to numpy for transpose, then take median RMS over antennas
        # Median is resistant to outliers and gives a good indication of the line phase quality over all antennas
        
        x = phaseRMS.toNumpy()
        medians = np.median(x, axis=0)
        print ' There are %d medians' % len(medians)
   
        print ' Median is lowest at channel %d, value = %f' % (medians[2:-1].argmin(), medians[2:-1].min())
        y = avgspectrum.toNumpy()[0]
        print ' Spectrum is highest at channel %d, log-value = %f' % (y.argmax(), np.log(y.max()))
        
        plt.figure()
        plt.plot(np.log(y), c='b')
        plt.plot(medians, c='r')
        plt.title('Average spectrum of one antenna versus median-RMS phase stability')
        plt.xlabel('Frequency channel')
        plt.ylabel('Blue: log-spectral power [adc units]\nRed: RMS phase stability [rad]')
        
        bestchannel = medians[2:-1].argmin()

        # diagnostic test, plot avg phase, also plot all / many individual measured phases
        # at the frequency that gives the best phase stability
        plt.figure()
        x = phaseAvg.toNumpy()[:, bestchannel]
        #...
        plt.plot(x)
        
        
        # find direction of this source; plot image to check quality of fit.
"""
        print 'Find direction of source based on averaged phases per antenna...'
        bestchannel = medians[2:-1].argmin()
        freqs = f["FREQUENCY_DATA"]
        freq = freqs[int(bestchannel)]
        
        allpositions = f["ANTENNA_POSITIONS"].toNumpy()
        positions = allpositions.ravel() # only pol 'polarisation'
        # NB. Indexing to the end is done by thisArray[start::step] !
        
        azSteps = int(360 / self.direction_resolution[0])
        elSteps = int(90 / self.direction_resolution[1])
        (fitaz, fitel, minPhaseError) = sf.directionBruteForcePhases(positions, phaseAvg.toNumpy()[:, bestchannel], freq, azSteps = azSteps, elSteps = elSteps, showImage = self.doplot, verbose = True)

        print 'Best fit az = %f, el = %f' % (fitaz / deg2rad, fitel / deg2rad)
        print 'Phase error = %f' % minPhaseError
        
             
                       

                    

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
