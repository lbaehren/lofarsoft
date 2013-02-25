"""
StationBeamformer documentation
===============================

.. moduleauthor:: Name of the module author <email address of the module author>
"""

from pycrtools.tasks import Task
from pycrtools.grid import CoordinateGrid
import pycrtools as cr
import pytmf
import numpy as np
import time
import os

cr.tasks.__raiseTaskDeprecationWarning(__name__)

class StationBeamformer(Task):
    """Beamformer optimized to generate complex spectrum as binary file output for a single station.
    """

    parameters = dict(
        filename=dict(default="test.h5"),
        data=dict(default=lambda self: cr.open(self.filename, self.blocksize)),
        output_filename=dict(default=lambda self: self.filename.rstrip(".h5") + ".beam.dat"),
        antennas=dict(default=None, doc="List of antennas or string (odd/even)."),
        blocksize=dict(default=2 ** 16),
        startblock=dict(default=0),
        nblocks=dict(default=lambda self: self.data["MAXIMUM_READ_LENGTH"] / self.data["BLOCKSIZE"]),
        coordinates=dict(default=(pytmf.hms2deg(5, 34, 31.97), pytmf.dms2deg(22, 0, 52.0)), doc="Coordinates of point in (ra, dec)."),
        frequencies = dict(default=lambda self: self.data["FREQUENCY_DATA"]),
        nfreq = dict(default=lambda self: len(self.frequencies)),
        antpos = dict(default=lambda self: self.data["ANTENNA_POSITIONS"]),
        nantennas = dict(default=lambda self: int(self.antpos.shape()[0])),
        OBSTIME = dict(default=lambda self: self.data["TIME"][0]),
        OBSLON = dict(default=pytmf.deg2rad(6.869837540), doc="Observer longitude in radians"),
        OBSLAT = dict(default=pytmf.deg2rad(52.915122495), doc="Observer latitude in radians"),
    )

    def init(self):
        """Initialize imager.
        """

        # Set antenna selection
        if self.antennas:
            self.data["SELECTED_DIPOLES"] = self.antennas

        # Generate coordinate grid with only one pixel
        print "Calculating beam direction"
        self.grid = CoordinateGrid(OBSTIME=self.OBSTIME,
                                 L=self.OBSLON,
                                 phi=self.OBSLAT,
                                 obstime=self.OBSTIME,
                                 NAXIS=2,
                                 NAXIS1=1,
                                 NAXIS2=1,
                                 CTYPE1='RA---SIN',
                                 CTYPE2='DEC--SIN',
                                 LONPOLE=0.,
                                 LATPOLE=90.,
                                 CRVAL1=self.coordinates[0],
                                 CRVAL2=self.coordinates[1],
                                 CRPIX1=0.5,
                                 CRPIX2=0.5,
                                 CDELT1=1.0,
                                 CDELT2=1.0,
                                 CUNIT1="deg",
                                 CUNIT2="deg")

        # Calculate geometric delays for all antennas
        print "Calculating geometric delays"
        self.delays = cr.hArray(float, dimensions=(1, 1, self.nantennas))
        cr.hGeometricDelays(self.delays, self.antpos, self.grid.cartesian, True)

        # Initialize empty arrays
        self.fftdata = cr.hArray(complex, dimensions=(self.nantennas, self.nfreq))
        self.beam = cr.hArray(complex, dimensions=(1, 1, self.nfreq), fill=0.)

    def run(self):
        """Run the beamformer.
        """

        start = time.time()
        for block in range(self.startblock, self.startblock + self.nblocks):

            print "processing block", block, "of", self.nblocks

            # Get FFT data after applying Hanning filter and possibly Nyquist swapping
            self.data.getFFTData(self.fftdata, block)

            # Beamforming
            cr.hBeamformImage(self.beam, self.fftdata, self.frequencies, self.delays)

            # Append block to output file
            cr.hWriteFileBinaryAppend(self.beam, self.output_filename)

        end = time.time()
        print "total runtime:", end - start, "s"
