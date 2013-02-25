"""
Module documentation
====================

.. moduleauthor:: Name of the module author <email address of the module author>
"""

from pycrtools.tasks import Task
from pycrtools.grid import CoordinateGrid
import pytmf
import numpy as np
import pycrtools as cr
import matplotlib.pyplot as plt
from scipy.optimize import fmin_powell

cr.tasks.__raiseTaskDeprecationWarning(__name__)

class DirectionFitBF(Task):
    """Find direction by maximizing the beamformed signal using a simplex fit.
    """

    parameters = dict(
        fft_data=dict(default=None,
            doc="Input FFT data."),
        frequencies=dict(default=None,
            doc="Frequencies."),
        antpos=dict(default=None,
            doc="Antenna positions."),
        start_direction=dict(default=(0., 90.),
            doc="Pointing as (az, el) tuple in degrees in the LOFAR convention."),
        maxiter=dict(default=100,
            doc="Maximum number of iterations in simplex fit."),
        xtol=dict(default=1.0,
            doc="Convergence is assumed when direction varies less than this value in each direction between iterations."),
        nantennas = dict(default=lambda self: self.fft_data.shape()[0],
            doc="Number of antennas."),
        nfreq = dict(default=lambda self: self.fft_data.shape()[1],
            doc="Number of frequencies."),
        blocksize = dict(default=lambda self: 2 * (self.nfreq - 1),
            doc="Number of samples in a block."),
        beamformed_fft = dict(default=lambda self: cr.hArray(complex, dimensions=(self.nfreq, )),
            doc = "Resulting beamformed FFT signal."),
        ifftwplan = dict(default=lambda self:cr.FFTWPlanManyDftC2r(self.blocksize, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE),
            doc = "FFTW plan for inverse transform."),

        fit_direction=dict(default=None, output=True,
            doc="Pointing as (az, el) tuple in degrees in the LOFAR convention."),
        beamformed_timeseries=dict(default=lambda self: cr.hArray(float, dimensions=(self.blocksize)), output = True,
            doc="Timeseries data."),
        delays=dict(default=lambda self: cr.hArray(float, self.nantennas),
            doc="Delays used for beamforming"),
    )

    def run(self):
        """Run the task.
        """

        def negative_bf_signal(direction):

            # Calculate Cartesian coordinates for direction
            self.direction_cartesian = cr.hArray(pytmf.spherical2cartesian(1.0, 2 * np.pi - pytmf.deg2rad(direction[1]), 2 * np.pi - pytmf.deg2rad(direction[0])))

            # Calculate geometric delays for all sky positions for all antennas
            cr.hGeometricDelays(self.delays, self.antpos, self.direction_cartesian, True)

            # Do beamforming
            self.beamformed_fft.fill(0.)
            cr.hBeamformBlock(self.beamformed_fft, self.fft_data, self.frequencies, self.delays)

            # Go to timeseries
            cr.hFFTWExecutePlan(self.beamformed_timeseries, self.beamformed_fft, self.ifftwplan)

            # Get absolute value squared
            cr.hSquare(self.beamformed_timeseries)

            return -1 * cr.hMax(self.beamformed_timeseries).val()

        self.fit_direction = fmin_powell(negative_bf_signal, np.asarray(self.start_direction), maxiter=self.maxiter, xtol=1.0)
