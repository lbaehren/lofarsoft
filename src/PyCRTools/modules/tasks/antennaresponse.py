"""
AntennaResponse documentation
=============================

"""

from pycrtools.tasks import Task
import pycrtools as cr

class AntennaResponse(Task):
    """AntennaResponse task documentation.
    """

    parameters = dict(
        az = dict ( default = 0.0 ),
        el = dict ( default = 0.0 ),
        data = dict( default = None ),
        fftdata = dict( default = None ),
        frequencies = dict( default = None ),
        nantennas = dict ( default = None ),
        blocksize = dict( default = None ),
        nf = dict ( default = lambda self : self.blocksize / 2 + 1 ),
        antennaset = dict ( default = "LBA_OUTER" ),
        polarization = dict ( default = 0 ),
        nyquist_zone = dict( default = 1 ),
        sample_frequency = dict( default = 200.e6 ),
        gain = dict ( default = lambda self : cr.hArray(float, dimensions = (2, self.nf)) ),
    )

    def init(self):
        """Initialize.
        """

        # Calculate frequencies if not given
        if self.frequencies is None:
            self.frequencies=cr.hArray(float, self.nf)
            cr.hFFTFrequencies(self.frequencies, self.sample_frequency, self.nyquist_zone)

        # Create plan for forward FFT
        self.plan = cr.FFTWPlanManyDftR2c(self.blocksize, self.nantennas, 1, self.blocksize, 1, self.nf, cr.fftw_flags.ESTIMATE)

        # Create plan for inverse FFT
        self.iplan = cr.FFTWPlanManyDftC2r(self.blocksize, self.nantennas, 1, self.nf, 1, self.blocksize, cr.fftw_flags.ESTIMATE)

    def run(self):
        """Run.
        """

        print "Running AntennaResponse now (hope this works)."

        # Calculate gain factors
        cr.hLBAGain(self.gain, self.frequencies, self.az, self.el)

        # Compensate for scaling in inverse FFT
        cr.hMul(self.gain, self.blocksize)

        # Go to frequency domain
        if self.fftdata is None:

            # Create empty array
            self.fftdata = cr.hArray(complex, dimensions=(self.nantennas, self.nf))

            # Perform FFT
            cr.hFFTWExecutePlan(self.fftdata, self.data, self.plan)

        # Correct for antenna response
        if self.antennaset == "LBA_INNER":
            cr.hDiv(self.fftdata, self.gain[self.polarization])
        elif self.antennaset == "LBA_OUTER":
            cr.hDiv(self.fftdata, self.gain[int(not self.polarization)])
        else:
            raise ValueError("Antennaset not supported.")

        # Transform back to time domain
        cr.hFFTWExecutePlan(self.data, self.fftdata, self.iplan)

