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
    # This holds especially when processing large blocks (high spectral resolution) in not-so-long datasets
    # e.g. 10 blocks of length 65536.
    # Currently we use a flagwidth of 1 + blocksize / 4096.
    
    # extend dirty channels to width 'flagwidth' (odd number needed), to also capture the flanks of 
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
        freq_range = dict( default = None, doc = "Frequency range to consider; everything outside the range is flagged as 'bad'. Give as tuple, e.g. (30, 80)" ), 
        fft_data = dict( default = lambda self : cr.hArray(complex, dimensions=(self.nantennas, self.nfreq)),
            doc = "Fourier transform of timeseries_data_resampled." ),
        #fftwplan = dict( default = lambda self : cr.FFTWPlanManyDftR2c(self.blocksize, self.nantennas, 1, 1, 1, 1, cr.fftw_flags.MEASURE),
        #    doc = "Forward plan for FFTW, by default use a plan that works over all antennas at once and is measured for speed because this is applied many times." ),
        bad_antennas = dict( default = [],
            doc = "Antennas found to be bad.", output = True ),
        good_antennas = dict( default = lambda self : [name for name in self.f["SELECTED_DIPOLES"] if name not in self.bad_antennas],
            doc = "Antennas found to be good.", output = True ),
        dirty_channels = dict( default = [],
            doc = "List of channels found to contain RFI", output = True ),
        median_average_spectrum = dict( default = None, doc = "Average power spectrum, in ADC units, uncleaned (raw data). Median over antennas is taken; dimension = [nfreq].", output = True ),
        median_cleaned_spectrum = dict( default = None, doc = "Cleaned power spectrum. Equals average_spectrum, but with dirty_channels set to zero.", output = True ),
        average_spectrum = dict( default = None, doc = "Average power spectrum (ADC units) per antenna, uncleaned. Dim = [nantennas, nfreq]", output = True ),
        cleaned_spectrum = dict( default = None, doc = "Cleaned power spectrum per antenna.", output = True ),
        phase_average = dict( default = None, doc = "Average phases per antenna, per frequency. Can be passed to the RF calibration Task. Dimension = nantennas x nfreq", output = True ),
        median_phase_spreads = dict( default = None, doc = "Median over antennas, of the phase spread measure from all blocks. Dimension = [nfreq]", output = True ),
        antennas_cleaned_power = dict(default = None, doc = "Cleaned power (sum of squares) from cleaned spectrum, per antenna. ", output = True ),
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
            # Note: No hanning window as we want to measure power from spectrum
            # in the same units as power from timeseries. Applying a window gives (at least) a scale factor
            # difference!
            # But no window makes the cleaning less effective... :(
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
        avgspectrum /= float(nblocks) # normalize
        avgspectrum[..., 0] = 0.0
#        avgspectrum[..., 1] = 0.0 # zero out DC and 1st harmonic (why is 1st harmonic so strong?)
        
        self.average_spectrum = cr.hArray(avgspectrum) # hArray to output param
        
        incPhaseRMS = cr.hArray(float, dimensions = incphasemean)
#        incPhaseAvg = hArray(float, dimensions = incphasemean)
        self.phase_average = cr.hArray(float, dimensions = incphasemean)
        
        cr.hComplexToPhase(self.phase_average, incphasemean)
        incphasemean.abs()
        incPhaseRMS.copy(incphasemean) 
        incPhaseRMS *= -1 / float(nblocks)
        incPhaseRMS += 1
        incPhaseRMS.sqrt()
        incPhaseRMS *= np.sqrt(2.0) # check...???
        phaseRMS = incPhaseRMS
                
        x = phaseRMS.toNumpy()
        medians = np.median(x, axis=0)
        medians[0] = 1.0
        medians[1] = 1.0 
        self.median_phase_spreads = cr.hArray(medians)
        #print ' There are %d medians' % len(medians)        
        
        # Get average spectrum, median over antennas for every freq channel
        y = np.median(avgspectrum.toNumpy(), axis=0)        
        self.median_average_spectrum = cr.hArray(y)
        logspectrum = np.log(y)

        # Get 'dirty channels' for output, and to show in plot
        # Extend dirty channels to both sides, especially when having large blocksizes
        flagwidth = 1 + self.blocksize / 4096 
        dirty_channels = dirtyChannelsFromPhaseSpreads(medians, freq_range = self.freq_range, flagwidth = flagwidth, testplots=False)
        # if a frequency range was given, flag everything outside the range as 'dirty'
        

        # Get cleaned spectrum per antenna
        cleanedspectrum = cr.hArray(copy = avgspectrum)
#        import pdb; pdb.set_trace()
        cleanedspectrum[..., cr.hArray(self.dirty_channels)] = 0.0 # seems not to work with hArrays as cleanedspectrum[..., cr.hArray(self.dirty_channels)] = 0.0
#        plt.figure()
#        plt.plot(np.log(cleanedspectrum.toNumpy()[0]))
#        plt.plot(np.log(cleanedspectrum.toNumpy()[20]))
        self.cleaned_spectrum = cleanedspectrum
        #cleanedspectrum[self.dirty_channels] = np.float('nan')
#        self.cleaned_spectrum = cr.hArray(cleanedspectrum)

        # Get median cleaned spectrum (over all antennas)
        median_cleaned_spectrum = np.copy(y)
        median_cleaned_spectrum[self.dirty_channels] = 0.0
        self.median_cleaned_spectrum = cr.hArray(median_cleaned_spectrum)
        # Compute FFT
        #cr.hFFTWExecutePlan(self.fft_data, self.timeseries_data_resampled, self.fftwplan)
        
        # Get baseline per antenna of the average spectrum
#        pardict = dict(plot_finish = Pause, 
#                       FitBaseline = dict(ncoeffs=80,numin=30,numax=85,fittype="POLY",splineorder=3),
#                       CalcBaseline = dict(baseline=False) ) # Make sure baseline is recreated when the task is run a second time

#        print "---> Fit a baseline to the average spectrum"
#        avgspectrum.xvalues = self.f["FREQUENCY_DATA"]
#        fitbaseline = cr.trerun("FitBaseline","",avgspectrum, extendfit=0.5,pardict=pardict, frequency = avgspectrum.xvalues, doplot=0)

#        calcbaseline1 = cr.trerun("CalcBaseline",1, avgspectrum,pardict = pardict,invert=False,HanningUp=False,normalize=False,doplot=0)
#        amplitudes=hArray(copy=calcbaseline1.baseline)

        #Get a measure of the total power (actually sqrt thereof)
        #received in each antenna (without RFI) and in the entire
        #station.
        
        # Sum up power for avg spectrum
        # Subtract dirty channels
        cleaned_power = 2 * cr.hArray(self.cleaned_spectrum[...].sum() )
#        dirty_power = 2 * cr.hArray(avgspectrum[...].sum() ) - cleaned_power
#        total_power = 2 * cr.hArray(avgspectrum[...].sum() )
        
        self.antennas_cleaned_power = cleaned_power
        print self.antennas_cleaned_power
        print ' --- '
#        print total_power
#        total_power.sqrt()
#        print total_power
#        cleaned_power.sqrt()
#        print cleaned_power
#        print dirty_power
       # antennas_power[...] -= cr.hArray(avgspectrum[..., cr.hArray(self.dirty_channels)].sum())
#        print cleaned_power
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
            log_cleanedspectrum = np.log(median_cleaned_spectrum)
            plt.plot(x, log_cleanedspectrum, c = 'b')
            plt.title('Median-average spectrum of all antennas, cleaned')
            plt.xlabel('Frequency [MHz]')
            plt.ylabel('log-spectral power [adc units]')
            p = self.plot_prefix + "average_spectrum_cleaned.png"

            plt.savefig(p)

            self.plotlist.append(p)

