"""
AntennaResponse documentation
=============================

"""

import numpy as np
from pycrtools.tasks import Task
import pycrtools as cr

def frequencies(nf, sample_frequency, nyquist_zone):
    """Calculate frequencies."""

    frequencies = cr.hArray(float, nf)

    cr.hFFTFrequencies(frequencies, sample_frequency, nyquist_zone)

    return frequencies

class AntennaResponse(Task):
    """AntennaResponse task documentation.
    """

    parameters = dict(
        az = dict ( default = 0.0 ),
        el = dict ( default = 0.0 ),
        data = dict( default = None ),
        fftdata = dict( default = lambda self : cr.hArray(complex, dimensions=(self.nantennas, self.nf)) ),
        frequencies = dict( default = lambda self : frequencies(self.nf, self.sample_frequency, self.nyquist_zone) ),
        nantennas = dict ( default = None ),
        blocksize = dict( default = None ),
        nf = dict ( default = lambda self : self.blocksize / 2 + 1 ),
        antennaset = dict ( default = "LBA_OUTER" ),
        polarization = dict ( default = 0 ),
        nyquist_zone = dict( default = 1 ),
        sample_frequency = dict( default = 200.e6 ),
        gain = dict ( default = lambda self : cr.hArray(float, dimensions = (2, self.nf)) ),
        plan = dict ( default = lambda self : cr.FFTWPlanManyDftR2c(self.blocksize, self.nantennas, 1, self.blocksize, 1, self.nf, cr.fftw_flags.ESTIMATE) ),
        iplan = dict ( default = lambda self : cr.FFTWPlanManyDftC2r(self.blocksize, self.nantennas, 1, self.nf, 1, self.blocksize, cr.fftw_flags.ESTIMATE) ),
    )

    def run(self):
        """Run.
        """

        print "Running AntennaResponse now (hope this works)."

        # Calculate gain factors
        cr.hLBAGain(self.gain, self.frequencies, self.az, self.el)

        # Compensate for scaling in inverse FFT
        cr.hMul(self.gain, self.blocksize)

        # Go to frequency domain
        print "AntennaResponse: Converting to frequency domain."

        # Perform FFT
        print self.fftdata.shape(), self.data.shape()
        cr.hFFTWExecutePlan(self.fftdata, self.data, self.plan)

        # Correct for antenna response
        if self.antennaset == "LBA_INNER":
            cr.hDiv(self.fftdata, self.gain[self.polarization])
        elif self.antennaset == "LBA_OUTER":
            cr.hDiv(self.fftdata, self.gain[int(not self.polarization)])
        else:
            raise ValueError("Antennaset not supported.")

        print "AntennaResponse: Converting to time domain."
        print self.fftdata.shape(), self.data.shape()
        print self.plan

        np.save("freq-"+str(self.polarization), self.frequencies.toNumpy())
        np.save("gain-"+str(self.polarization), self.gain.toNumpy())

        # Transform back to time domain
        cr.hFFTWExecutePlan(self.data, self.fftdata, self.iplan)

