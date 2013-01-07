"""
Module documentation
====================

"""

from pycrtools.tasks import Task
import numpy as np
import pycrtools as cr
import matplotlib.pyplot as plt

class CrossCorrelateAntennas(Task):
    """
    **Usage:**

    ``Task = trun("CrossCorrelateAntennas", timeseries_data[n_dataset, blocksize], refant=0) -> Task.crosscorr_data (i.e., strength of cross-correlation as a function of time)``

    To provide only fft data use:

    ``Task = trun("CrossCorrelateAntennas", fft_data[n_dataset, blocksize / 2 + 1], refant=0) -> Task.crosscorr_data (i.e., strength of cross-correlation as a function of time)``

    **Description:**

    Calculates the cross-correlation of a number of time series data
    sets with respective to a reference data set.

    The data set is a matrix of N equal-length timeseries. If not
    reference data set is provided, a reference antenna from the input
    time series matrix is specified (by default = 0, i.e. all data is
    correlated with the first data set, then the first cross
    correlation output will actually be an autocorrelation).

    Make sure N is even. This is not tested for uneven N.

    NB: This has also not yet been tested for NyquistZone 2!

    If ``timeseries_data == None``, then one needs to provide
    ``fft_data`` instead and the fft will not be (re-)calculated. Note
    that the function uses fftw convention!

    """

    parameters = dict(
        timeseries_data=dict(default=None,
            doc="Timeseries data, if None fft_data must be given."),
        fft_data=dict(default=lambda self: cr.hArray(complex, (self.timeseries_data.shape()[0], self.timeseries_data.shape()[1] / 2 + 1)),
            doc="FFT of the input timeseries data",
            workarray=True),
        refant=dict(default=0,
            doc="Which data set in ``data`` to use as reference data set if no ``reference_data`` provided."),
        oversamplefactor=dict(default=1,
            doc="Oversample the final cross-correlation by so many sub-steps. If equal to one then no oversampling."),
        reference_data=dict(default=None,
            doc="Reference data set to cross-correlate data with. If ``None`` then use data from reference antenna ``refant``."),
        fft_reference_data=dict(default=lambda self: cr.hArray(complex, self.fft_data.shape()[1]),
            doc="FFT of the reference data, dimensions ``[N data sets, (data length)/2+1]``. If no extra reference antenna array is provided, cross correlate with reference antenna in data set",
            workarray=True),
        crosscorr_data_orig=dict(default=lambda self: cr.hArray(float, (self.fft_data.shape()[0], (self.fft_data.shape()[1] - 1) * 2)),
            doc="Scratch cross correlation vector in original size to hold intermediate results",
            workarray=True),
        crosscorr_data=dict(default=lambda self: cr.hArray(float, (self.fft_data.shape()[0], (self.fft_data.shape()[1] - 1) * 2 * max(self.oversamplefactor, 1))),
                doc="Output array of dimensions ``[N data sets, data length * oversamplefactor]`` containing the cross correlation.",
                output=True),
        blocksize=dict(default=lambda self: (self.fft_data.shape()[1] - 1) * 2,
            doc="Length of the data for each antenna"),
        fftplan=dict(default=lambda self: cr.FFTWPlanManyDftR2c(self.blocksize, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE),
            doc="Memory and plan for FFT",
            output=False,
            workarray=True),
        invfftplan=dict(default=lambda self: cr.FFTWPlanManyDftC2r(self.blocksize, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE),
            doc="Memory and plan for inverse FFT",
            output=False,
            workarray=True)
    )

    def run(self):
        if self.timeseries_data:
            cr.hFFTWExecutePlan(self.fft_data[...], self.timeseries_data[...], self.fftplan)
        if not self.reference_data:
            self.fft_reference_data.copy(self.fft_data[self.refant])
        else:
            cr.hFFTWExecutePlan(self.fft_reference_data[...], self.reference_data[...], self.fftplan)

        self.fft_data[...].crosscorrelatecomplex(self.fft_reference_data, True)

        self.fft_data /= self.crosscorr_data.shape()[-1]

        if self.oversamplefactor > 1:
            self.shift = 1.0 / self.oversamplefactor
            for i in range(self.oversamplefactor):
                cr.hFFTWExecutePlan(self.crosscorr_data_orig[...], self.fft_data[...], self.invfftplan)

                # distribute with gaps in between and shift by a fraction
                self.crosscorr_data[...].redistribute(self.crosscorr_data_orig[...], i, self.oversamplefactor)

                # apply a sub-sample shift to the FFT data
                self.fft_data[...].shiftfft(self.fft_data[...], self.shift)
        else:
            cr.hFFTWExecutePlan(self.crosscorr_data[...], self.fft_data[...], self.invfftplan)

class FindPulseDelay(Task):
    """Calculate pulse delay by finding maxima in a trace.

    This may require some additional manipulation to the data to give a meaningfull result,
    for instance the trace may be the cross correlation signal.
    """

    parameters = dict(
        trace=dict(default=None,
            doc="array containing a trace as a function of time for each antenna"),
        refant=dict(default=None,
            doc="reference antenna"),
        sampling_frequency=dict(default=200.e6,
            doc="sampling frequency in Hz."),
        maxpos=dict(default = lambda self: cr.hArray(int, self.trace.shape()[0]),
            doc = "position of pulse maximum.",
            output = True),
        delay=dict(default = lambda self: cr.hArray(float, self.trace.shape()[0]),
            doc = "pulse delays in seconds.",
            output = True),
    )

    def run(self):

        # Take absolute value
        temp = self.trace.new()
        temp[...].copy(self.trace[...])
        temp.abs()
        print temp[0].max(), temp[0].maxpos()
        print temp[1].max(), temp[1].maxpos()

        # Calculate position
        self.maxpos[...] = temp[...].maxpos()

        # Convert to time delay
        self.delay[...].copy(self.maxpos[...])
        print self.maxpos
        print self.delay
        self.delay /= self.sampling_frequency
        print self.delay

        # Shift delay to be relative to reference antenna
        if self.refant is not None:
            print "using reference antenna", self.refant
            self.delay -= self.delay[self.refant]

