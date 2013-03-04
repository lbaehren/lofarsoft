"""
Module documentation
====================

.. moduleauthor:: Pim Schellart <p.schellart@astro.ru.nl>
"""

from pycrtools.tasks import Task
from pycrtools.grid import CoordinateGrid
import pytmf
import numpy as np
import pycrtools as cr
import matplotlib.pyplot as plt


class MiniBeamformer(Task):
    """Beamformer is deliberately designed to be fast, lightweight and simple.

    Forms a beam in one given direction from a set of input antennas.
    """

    parameters = dict(
        fft_data=dict(default=None,
            doc="Input FFT data."),
        frequencies=dict(default=None,
            doc="Frequencies."),
        antpos=dict(default=None,
            doc="Antenna positions."),
        direction=dict(default=(0., 90.),
            doc="Pointing as (az, el) tuple in degrees in the LOFAR convention."),
        nantennas = dict(default=lambda self: self.fft_data.shape()[0],
            doc="Number of antennas."),
        nfreq = dict(default=lambda self: self.fft_data.shape()[1],
            doc="Number of frequencies."),
        blocksize = dict(default=lambda self: self.fft_data.shape()[1] - 1 * 2,
            doc="Number of samples in a block."),
        beamformed_fft = dict(default=lambda self: cr.hArray(complex, dimensions=(self.nfreq, )),
            doc = "Resulting beamformed FFT signal."),
    )

    def run(self):
        """Run the task.
        """

        # Calculate Cartesian coordinates for direction
        self.direction_cartesian = cr.hArray(pytmf.spherical2cartesian(1.0, 2 * np.pi - pytmf.deg2rad(self.direction[1]), 2 * np.pi - pytmf.deg2rad(self.direction[0])))

        # Do beamforming
        cr.hBeamformBlock(self.beamformed_fft, self.fft_data, self.frequencies, self.antpos, self.direction_cartesian)
