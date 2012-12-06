"""
AntennaResponse documentation
=============================

"""

from pycrtools.tasks import Task
import pycrtools as cr
import numpy as np
import os


class AntennaResponse(Task):
    """Calculates and unfolds the LOFAR (LBA or HBA) antenna response.

    Given an array with *instrumental_polarization* and a *direction* as (Azimuth, Elevation) the Jones matrix
    containing the LOFAR (LBA or HBA) antenna response is calculated.

    Mixing the two instrumental polarizations by multiplying with the inverse
    Jones matrix gives the resulting on-sky polarizations (e.g. perpendicular to the direction and
    parallel to and perpendicular to the horizon).

    Note that, due to the intrinsic symmetry of the antenna configuration, the direction of Azimuth (CW or CCW)
    is not relevant.
    """

    parameters = dict(
        instrumental_polarization=dict(default=None,
            doc="FFT data."),
        on_sky_polarization=dict(default=lambda self: self.instrumental_polarization.new(), output=True,
            doc="FFT data corrected for element response (contains on sky polarizations)."),
        frequencies=dict(default=None,
            doc="Frequencies in Hz."),
        direction=dict(default=(0, 0),
            doc="Direction in degrees as a (Azimuth, Elevation) tuple."),
        nantennas = dict(default=lambda self: self.instrumental_polarization.shape()[0],
            doc="Number of antennas."),
        antennaset = dict(default="LBA_OUTER",
            doc="Antennaset."),
        jones_matrix = dict(default=lambda self: cr.hArray(complex, dimensions=(self.frequencies.shape()[0], 2, 2)),
            doc = "Jones matrix for each frequency."),
        inverse_jones_matrix=dict(default = lambda self: cr.hArray(complex, dimensions=(self.frequencies.shape()[0], 2, 2)),
            doc="Inverse Jones matrix for each frequency."),
        backwards=dict(default=False,
            doc="Apply antenna response backwards (e.g. without inverting the Jones matrix)."),
    )

    def run(self):
        """Run.
        """

        # Copy FFT data over for correction
        self.on_sky_polarization.copy(self.instrumental_polarization)

        # Read tables with antenna model simulation
        vt = np.loadtxt(os.environ["LOFARSOFT"] + "/data/lofar/antenna_response_model/LBA_Vout_theta.txt", skiprows=1)
        vp = np.loadtxt(os.environ["LOFARSOFT"] + "/data/lofar/antenna_response_model/LBA_Vout_phi.txt", skiprows=1)

        cvt = cr.hArray(vt[:, 3] + 1j * vt[:, 4])
        cvp = cr.hArray(vp[:, 3] + 1j * vp[:, 4])

        fstart = 10.0 * 1.e6
        fstep = 1.0 * 1.e6
        fn = 101
        tstart = 0.0
        tstep = 5.0
        tn = 19
        pstart = 0.0
        pstep = 10.0
        pn = 37

        # Get inverse Jones matrix for each frequency
        print "obtaining Jones matrix"
        for i, f in enumerate(self.frequencies):
            cr.hGetJonesMatrix(self.jones_matrix[i], f, self.direction[0], self.direction[1], cvt, cvp, fstart, fstep, fn, tstart, tstep, tn, pstart, pstep, pn)

        print "inverting Jones matrix"
        if not self.backwards:
            cr.hInvertComplexMatrix2D(self.inverse_jones_matrix, self.jones_matrix)

        # Unfold the antenna response and mix polarizations according to the Jones matrix to get the on-sky polarizations
        if not self.backwards:
            print "unfolding antenna pattern"
            cr.hMatrixMix(self.on_sky_polarization[0:self.nantennas:2, ...], self.on_sky_polarization[1:self.nantennas:2, ...], self.inverse_jones_matrix)
        else:
            print "unfolding antenna pattern (backwards)"
            cr.hMatrixMix(self.on_sky_polarization[0:self.nantennas:2, ...], self.on_sky_polarization[1:self.nantennas:2, ...], self.jones_matrix)
