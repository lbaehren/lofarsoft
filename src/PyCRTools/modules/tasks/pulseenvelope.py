"""
Module documentation
====================

"""

from pycrtools.tasks import Task
from pycrtools.grid import CoordinateGrid
import pycrtools as cr

class PulseEnvelope(Task):
    """Calculate pulse envelope using Hilbert transform.

    Optionally the envelope will be (up/down)sampled using the *resample_factor* and the *delays* between the
    maxima will be computed with respect to a reference antenna that is either given or taken to be the one
    with the highest signal to noise.
    """

    parameters = dict(
        timeseries_data = dict( default = None,
            doc = "Timeseries data." ),
        nantennas = dict( default = lambda self : self.timeseries_data.shape()[0],
            doc = "Number of antennas." ),
        pulse_start = dict( default = 0,
            doc = "Start of pulse window." ),
        pulse_end = dict( default = lambda self : self.timeseries_data.shape[1],
            doc = "End of pulse window." ),
        pulse_width = dict( default = lambda self : self.pulse_end - self.pulse_start,
            doc = "Width of pulse window." ),
        resample_factor = dict( default = 1,
            doc = "Resample factor." ),
        pulse_width_resampled = dict( default = lambda self : self.pulse_width * self.resample_factor,
            doc = "Width of pulse window after resampling." ),
        timeseries_data_resampled = dict( default = lambda self : cr.hArray(float, dimensions=(self.nantennas, self.pulse_width_resampled)),
            doc = "Resampled timeseries data." ),
        nfreq = dict( default = lambda self : self.pulse_width_resampled / 2 + 1,
            doc = "Number of frequencies." ),
        fft_data = dict( default = lambda self : cr.hArray(complex, dimensions=(self.nantennas, self.nfreq)),
            doc = "Fourier transform of timeseries_data_resampled." ),
        sampling_frequency = dict(default = 200.e6,
            doc = "Sampling frequency of timeseries_data." ),
        hilbertt = dict( default = lambda self : self.timeseries_data_resampled.new(), workarray = True,
            doc = "Hilbert transform of *fft_data*." ),
        envelope = dict( default = lambda self : self.timeseries_data_resampled.new(), workarray = True,
            doc = "Envelope calculated using Hilbert transform." ),
        fftwplan = dict( default = lambda self : cr.FFTWPlanManyDftR2c(self.pulse_width_resampled, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE) ),
        ifftwplan = dict( default = lambda self : cr.FFTWPlanManyDftC2r(self.pulse_width_resampled, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE) ),
        snr = dict( default = None, output = True,
            doc = "Signal to noise ratio of pulse maximum." ),
        refant = dict( default = lambda self : self.snr.maxpos().val(),
            doc = "Reference antenna for delays, taken as antenna with highest signal to noise." ),
        delays = dict( default = lambda self : cr.hArray(float, self.nantennas), output = True,
            doc = "Delays corresponding to the position of the maximum of the envelope relative to the first antenna." ),
        nsigma = dict( default = 7,
            doc = "Number of standard deviations that pulse needs to be above noise level." ),
        antennas_with_significant_pulses = dict( default = lambda self : [i for i in range(self.nantennas) if self.snr[i] > self.nsigma],
            doc = "Indices of antennas with pulses more than nsigma above the noise limit." ),
    )

    def run(self):
        """Run the task.
        """

        # Resample singal
        cr.hFFTWResample(self.timeseries_data_resampled[...], self.timeseries_data[..., self.pulse_start:self.pulse_end])

        # Compute FFT
        cr.hFFTWExecutePlan(self.fft_data[...], self.timeseries_data_resampled[...], self.fftwplan)

        # Apply Hilbert transform
        cr.hApplyHilbertTransform(self.fft_data[...])

        # Get inverse FFT
        cr.hFFTWExecutePlan(self.hilbertt[...], self.fft_data[...], self.ifftwplan)
        self.hilbertt /= self.pulse_width_resampled

        # Get envelope
        self.envelope.fill(0)
        cr.hSquareAdd(self.envelope, self.hilbertt)
        cr.hSquareAdd(self.envelope, self.timeseries_data_resampled)
        cr.hSqrt(self.envelope)

        # Find maxima
        self.maxpos = self.envelope[...].maxpos()

        # Find signal to noise ratio
        self.snr = cr.hArray((self.envelope[...].max() - self.envelope[...].mean()) / self.envelope[...].stddev())

        # Convert to delay
        self.delays[:] = self.maxpos[:]
        self.delays /= (self.sampling_frequency * self.resample_factor)
        self.delays -= self.delays[self.refant]

