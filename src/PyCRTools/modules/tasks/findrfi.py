"""
Module documentation
====================

"""

from pycrtools.tasks import Task
import numpy as np
import pycrtools as cr
import matplotlib.pyplot as plt


def dirtyChannelsFromPhaseSpreads(spreads, flagwidth = 3, testplots=False):
    length = len(spreads)
    sorted = np.sort(spreads)
    # Sorted plot will have many entries below the uniform-random value of sqrt(2). 
    # They correspond to the RFI lines. Circumvent taking standard-deviation and peak-detection,
    # as the stddev is skewed by the very peaks we are trying to find... 
    # This is done by estimating the (noise) spread using the higher half of the data (no peaks!)
    # then have a criterion of > 3x noise to make an RF-line detection.
    medianSpread = np.median(spreads)
    noise = sorted[length * 0.95] - sorted[length / 2] # value at 95 percentile minus median
    
    dirtyChannels = np.where(spreads < medianSpread - 3*noise)[0]
    
    # In principle, that's all! The '3' may have to be tuned a little.
    
    # If needed, the dirty channels have to be 'fattened' to both sides to also cut the flanks 
    # of the spectral lines...
    
    # extend dirty channels to width 'flagwidth' (odd number needed ), to also capture the flanks of 
    # spectral lines
    extDirtyChannels = np.zeros(length)
    sideExtension = (flagwidth - 1) / 2
    for i in range(sideExtension, length - sideExtension):
        if i in dirtyChannels:
            extDirtyChannels[i-sideExtension:i+sideExtension+1] = 1
    
    extDirtyChannels = np.where(extDirtyChannels > 0)[0]
    
    print 'Median spread = %f' % medianSpread
    print 'Noise = %f' % noise
    print 'There are %d dirty channels' % len(dirtyChannels)
    dirtyChannels = np.array(extDirtyChannels)
    print 'There are %d dirty channels when extended by 1 channel to either side' % len(dirtyChannels)
    
    if testplots:       
        plt.figure()
        plt.plot(sorted)
        plt.axvline(x = len(dirtyChannels), ymin = 0, ymax = 1.83)
        plt.title("Sorted phase spread values. Vertical line = cutoff for 'dirty channels'")
        plt.ylabel("Spread value [rad]")
        plt.xlabel("index")
        
    return dirtyChannels


class FindRFI(Task):
    """Find RFI by creating an average spectrum and doing phase magic.
    """

    parameters = dict(
        filename = dict( default = None,
            doc = "Filename, used if no file object is given." ),
        f = dict( default = lambda self : cr.open(self.filename),
            doc = "File object." ),
        blocksize = dict ( default = lambda self : self.f["BLOCKSIZE"],
            doc = "Blocksize." ),
        nantennas = dict( default = lambda self : self.f["NOF_SELECTED_DATASETS"],
            doc = "Number of selected antennas." ),
        nfreq = dict( default = lambda self : self.blocksize / 2 + 1,
            doc = "Number of frequencies in FFT." ),
        nofblocks = dict( default = -1, doc = "Number of data blocks to process. Set to -1 for entire file." ),
#        timeseries_data = dict( default = lambda self : cr.hArray(float, dimensions=(self.nantennas, self.blocksize)),
#            doc = "Timeseries data." ),
        fft_data = dict( default = lambda self : cr.hArray(complex, dimensions=(self.nantennas, self.nfreq)),
            doc = "Fourier transform of timeseries_data_resampled." ),
        #fftwplan = dict( default = lambda self : cr.FFTWPlanManyDftR2c(self.blocksize, self.nantennas, 1, 1, 1, 1, cr.fftw_flags.MEASURE),
        #    doc = "Forward plan for FFTW, by default use a plan that works over all antennas at once and is measured for spead because this is applied many times." ),
        bad_antennas = dict( default = [],
            doc = "Antennas found to be bad.", output = True ),
        good_antennas = dict( default = lambda self : [name for name in self.f["SELECTED_DIPOLES"] if name not in self.bad_antennas],
            doc = "Antennas found to be good.", output = True ),
        dirty_channels = dict( default = [],
            doc = "List of channels found to contain RFI", output = True ),
        average_spectrum = dict( default = None, doc = "Average power spectrum, in ADC units, uncleaned (raw data). Median over antennas is taken; dimension = [nfreq].", output = True ),
        cleaned_spectrum = dict( default = None, doc = "Cleaned power spectrum. Equals average_spectrum, but with dirty_channels set to zero.", output = True ),
        phase_average = dict( default = None, doc = "Average phases per antenna, per frequency. Can be passed to the RF calibration Task. Dimension = nantennas x nfreq", output = True ),
        median_phase_spreads = dict( default = None, doc = "Median over antennas, of the phase spread measure from all blocks. Dimension = [nfreq]", output = True ),
        
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
        
        if self.filename:
            self.f = cr.open(self.filename, blocksize = self.blocksize) # assume blocksize has been set as well
            
        #print self.fft_data

        nblocks = self.f["MAXIMUM_READ_LENGTH"] / self.blocksize
        if self.nofblocks > 0:
            nblocks = min(nblocks, self.nofblocks) # cap to maximum read length
        
#        nblocks = min(f["DATA_LENGTH"]) / self.blocksize # Why is this not working?
#        nblocks -= 5 # HACK -- why needed?

#        if self.nofblocks:
#            nblocks = min(self.nofblocks, nblocks) # limit to # blocks given in params
        if self.verbose:
            print 'Processing %d blocks of length %d' % (nblocks, self.blocksize)
               
        incphasemean = cr.hArray(complex, dimensions = self.fft_data) # this leaks memory, per Task instance (?)
        incphase = cr.hArray(complex, dimensions = incphasemean)
        
        avgspectrum = cr.hArray(float, dimensions = self.fft_data)
        spectrum = cr.hArray(complex, dimensions = self.fft_data)
        magspectrum = cr.hArray(complex, dimensions = self.fft_data)
        
        n = 0
        skippedblocks = 0
        for i in range(nblocks):
        # accumulate list of arrays of phases, from spectrum of all antennas of every block
        # have to cut out the block with the pulse... autodetect is best? 
            if self.verbose:
                print 'Doing block %d of %d' % (i, nblocks)
            self.f["BLOCK"] = i
            
            # want to discard blocks with spiky signals... see AverageSpectrum? Or a simple max/sigma test?
#            x = f["TIMESERIES_DATA"]
#            maxx = x.max()[0]
#            stdx = x.stddev()[0]
            self.f.getFFTData(self.fft_data, block = i, hanning = True)
            spectrum = self.fft_data / self.f["BLOCKSIZE"] # normalize back to ADC units
#            magspectrum[..., 0] = 0.0 # Want to do that here? Debug / test phase handling / div by 0
#            magspectrum[..., 1] = 0.0
            magspectrum.copy(spectrum)
            magspectrum.abs()
#            magspectrum += 1e-9

            incphase.copy(spectrum)
            incphase /= (magspectrum + 1.0e-9) # divide abs-magnitude out to get exp(i phi). Avoid div by 0            
            incphaseRef = cr.hArray(incphase[0].vec()) # improve? i.e. w/o memory leak. Phases of channel 0 for all freqs
            # Note that this assumes that antenna 0 has valid data...! What to do when it doesn't?
            incphase /= incphaseRef           
            incphase[..., 0] = 1.0 # reject DC component and first harmonic.
            incphase[..., 1] = 1.0

            incphasemean += incphase # accumulate Sum( exp(i phi) ) for all blocks

            magspectrum.square()
            avgspectrum += magspectrum # accumulate average spectrum

            if np.isnan(incphase.sum()[0]): # FIX
                print 'Warning: NaN found! Skipping block'
                skippedblocks += 1 
                continue
                
                            
        nblocks -= skippedblocks
        print '# valid blocks = %d' % nblocks
        if nblocks == 0:
            print 'Error: all blocks have been skipped!'
            # may want to exit here
        
        incPhaseRMS = cr.hArray(float, dimensions = incphasemean)
#        incPhaseAvg = hArray(float, dimensions = incphasemean)
        self.phase_average = cr.hArray(float, dimensions = incphasemean)
        
        cr.hComplexToPhase(self.phase_average, incphasemean)
        incphasemean.abs()
        incPhaseRMS.copy(incphasemean) 
        incPhaseRMS *= -1 / float(nblocks)
        incPhaseRMS += 1
        incPhaseRMS.sqrt()
        incPhaseRMS *= np.sqrt(2.0) # check...
        phaseRMS = incPhaseRMS
                
        x = phaseRMS.toNumpy()
        medians = np.median(x, axis=0)
        medians[0] = 1.0
        medians[1] = 1.0 
        self.median_phase_spreads = cr.hArray(medians)
        #print ' There are %d medians' % len(medians)        
        
        # Get average spectrum, median over antennas for every freq channel
        y = np.median(avgspectrum.toNumpy(), axis=0)        
        self.average_spectrum = cr.hArray(y)
        logspectrum = np.log(y)

        # Get 'dirty channels' for output, and to show in plot
        # Extend dirty channels to both sides, especially when having large blocksizes
        flagwidth = 1 + self.blocksize / 4096 
        self.dirty_channels = dirtyChannelsFromPhaseSpreads(medians, flagwidth = flagwidth, testplots=False)

        # Get cleaned spectrum
        cleanedspectrum = np.copy(y)
        cleanedspectrum[self.dirty_channels] = np.float('nan')
        self.cleaned_spectrum = cr.hArray(cleanedspectrum)

        # Compute FFT
        #cr.hFFTWExecutePlan(self.fft_data, self.timeseries_data_resampled, self.fftwplan)
        
        # Get baseline per antenna of the average spectrum
#        calcbaseline1 = trerun("CalcBaseline",1,averagespectrum_good_antennas,pardict=par,invert=False,HanningUp=False,normalize=False,doplot=0)
#        amplitudes=hArray(copy=calcbaseline1.baseline)

        #Get a measure of the total power (actually sqrt thereof)
        #received in each antenna (without RFI) and in the entire
        #station.

#        antennas_power=hArray(amplitudes[...].mean())

        if self.save_plots:

            # Average spectrum (uncleaned)
            plt.clf()
            # plot dirty channels into spectrum plot, in red
            x = self.f["FREQUENCY_DATA"].toNumpy() * 1e-6
#            plt.figure()
            plt.plot(x, logspectrum, c='b')
            dirtyspectrum = np.zeros(len(logspectrum))
            dirtyspectrum += np.float('nan') # min(logspectrum)
            dirtyspectrum[self.dirty_channels] = logspectrum[self.dirty_channels]
            plt.plot(x, dirtyspectrum, 'x', c = 'r', markersize=8)
            plt.title('Median-average spectrum of all antennas, with flagging')
            plt.xlabel('Frequency [MHz]')
            plt.ylabel('log-spectral power [adc units]')
#            self.plot_finish(filename=self.plot_name + "-avgspectrum_withflags",filetype=self.filetype)
 
            p = self.plot_prefix + "average_spectrum.png"

            plt.savefig(p)

            self.plotlist.append(p)

            # Average spectrum (cleaned)
            plt.figure() 
#            plt.clf()

            ## Insert plot code here
            cleanedspectrum = np.log(cleanedspectrum)
            plt.plot(x, cleanedspectrum, c = 'b')
            plt.title('Median-average spectrum of all antennas, cleaned')
            plt.xlabel('Frequency [MHz]')
            plt.ylabel('log-spectral power [adc units]')
            p = self.plot_prefix + "average_spectrum_cleaned.png"

            plt.savefig(p)

            self.plotlist.append(p)

