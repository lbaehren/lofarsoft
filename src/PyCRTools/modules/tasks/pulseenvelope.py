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
    maxima will be computed.
    """

    parameters = dict(
        timeseries_data = dict( default = None,
            doc = "Timeseries data." ),
        fft_data = dict( default = lambda self : cr.hArray(complex, dimensions=(self.nantennas, self.nfreq)),
            doc = "Fourier transform of timeseries_data. Calculated if not given." ),
        nantennas = dict( default = lambda self : self.timeseries_data.shape()[0],
            doc = "Number of antennas." ),
        blocksize = dict( default = lambda self : self.timeseries_data.shape()[1],
            doc = "Blocksize (e.g. number of samples per antenna)." ),
        nfreq = dict( default = lambda self : self.blocksize / 2 + 1,
            doc = "Number of frequencies." ),
        pulse_start = dict( default = 0,
            doc = "Start of pulse window." ),
        pulse_end = dict( default = lambda self : self.blocksize,
            doc = "End of pulse window." ),
        pulse_width = dict( default = lambda self : self.pulse_end - self.pulse_start,
            doc = "Width of pulse window." ),
        sampling_frequency = dict(default = 200.e6,
            doc = "Sampling frequency of timeseries_data." ),
        resample_factor = dict( default = 1,
            doc = "Resample factor." ),
        delays = dict( default = lambda self : cr.hArray(float, self.timeseries_data.shape()[0]), output = True,
            doc = "Delays corresponding to the position of the maximum of the envelope relative to the first antenna." ),
        hilbertt = dict( default = lambda self : self.timeseries_data.new(), workarray = True,
            doc = "Hilbert transform of *fft_data*." ),
        envelope = dict( default = lambda self : self.timeseries_data.new(), workarray = True,
            doc = "Envelope calculated using Hilbert transform." ),
        envelope_resampled = dict( default = lambda self : cr.hArray(float, dimensions = (self.nantennas, self.pulse_width * self.resample_factor)), workarray = True,
            doc = "Envelope (up/down)sampled using *resample_factor*." ),
    )

    def init(self):
        """Initialize the task.
        """

        # Create FFTW plans
        self.fftplan = cr.FFTWPlanManyDftR2c(self.blocksize, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE)
        self.invfftplan = cr.FFTWPlanManyDftC2r(self.blocksize, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE)

    def run(self):
        """Run the task.
        """

        # Compute FFT
        cr.hFFTWExecutePlan(self.fft_data[...], self.timeseries_data[...], self.fftplan)

        # Apply Hilbert transform
        cr.hApplyHilbertTransform(self.fft_data[...])

        # Get inverse FFT
        cr.hFFTWExecutePlan(self.hilbertt[...], self.fft_data[...], self.invfftplan)
        self.hilbertt /= self.blocksize

        # Get envelope
        cr.hSquareAdd(self.envelope, self.hilbertt)
        cr.hSquareAdd(self.envelope, self.timeseries_data)
        cr.hSqrt(self.envelope)

        # Resample envelope and find maximum
        cr.hFFTWResample(self.envelope_resampled[...], self.envelope[..., self.pulse_start:self.pulse_end])

        # Find maxima
        self.maxpos = self.envelope_resampled[...].maxpos()

        # Convert to delay
        self.delays[:] = self.maxpos[:]
        self.delays /= (self.sampling_frequency * self.resample_factor)
        self.delays -= self.delays[0]

