"""
Module documentation
====================

"""

from pycrtools.tasks import Task
import numpy as np
import pycrtools as cr
import matplotlib.pyplot as plt

class FindRFI(Task):
    """Find RFI by creating an average spectrum.
    """

    parameters = dict(
        filename = dict( default = None,
            doc = "Filename, used if no file object is given." ),
        f = dict( default = lambda self : cr.open(self.filename),
            doc = "File object." ),
        blocksize = dict ( default = lambda self : self.f["BLOCKSIZE"],
            doc = "Blocksize." ),
        nantennas = dict( default = lambda self : self.f["NOF_SELECTED_DIPOLES"],
            doc = "Number of selected antennas." ),
        nfreq = dict( default = lambda self : self.blocksize / 2 + 1,
            doc = "Number of selected antennas." ),
        timeseries_data = dict( default = lambda self : cr.hArray(float, dimensions=(self.nantennas, self.blocksize)),
            doc = "Timeseries data." ),
        fft_data = dict( default = lambda self : cr.hArray(complex, dimensions=(self.nantennas, self.nfreq)),
            doc = "Fourier transform of timeseries_data_resampled." ),
        fftwplan = dict( default = lambda self : cr.FFTWPlanManyDftR2c(self.blocksize, self.nantennas, 1, 1, 1, 1, cr.fftw_flags.MEASURE),
            doc = "Forward plan for FFTW, by default use a plan that works over all antennas at once and is measured for spead because this is applied many times." ),
        bad_antennas = dict( default = [],
            "Antennas found to be bad." ),
        good_antennas = dict( default = lambda self : [name for name in self.f["SELECTED_DIPOLES"] if name not in self.bad_antennas],
            "Antennas found to be good." ),
        dirty_channels = dict( default = [],
            doc = "List of channels found to contain RFI" ),
        save_plots = dict( default = False,
            doc = "Store plots" ),
        plot_prefix = dict( default = "",
            doc = "Prefix for plots" ),
        plotlist = dict( default = [],
            doc = "List of plots" ),
        plot_antennas = dict( default = lambda self : range(self.nantennas),
            doc = "Antennas to create plots for." ),
    )

    def run(self):
        """Run the task.
        """

        # Compute FFT
        #cr.hFFTWExecutePlan(self.fft_data, self.timeseries_data_resampled, self.fftwplan)

        if self.save_plots:

            # Average spectrum (uncleaned)
            plt.clf()

            ## Insert plot code here

            p = self.plot_prefix + "average_spectrum.png"

            plt.savefig(p)

            self.plotlist.append(p)

            # Average spectrum (cleaned)
            plt.clf()

            ## Insert plot code here

            p = self.plot_prefix + "average_spectrum_cleaned.png"

            plt.savefig(p)

            self.plotlist.append(p)

