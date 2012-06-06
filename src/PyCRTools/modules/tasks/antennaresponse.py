"""
AntennaResponse documentation
=============================

"""

from pycrtools.tasks import Task
import pycrtools as cr
import pytmf

class AntennaResponse(Task):
    """Calculates and unfolds the LOFAR (LBA or HBA) antenna response.

    Given an array with *fft_data* and a *direction* as (Azimuth, Elevation) the Jones matrix
    containing the LOFAR (LBA or HBA) antenna response is calculated.

    Mixing the two instrumental polarizations by multiplying with the inverse
    Jones matrix gives the resulting on-sky polarizations (e.g. perpendicular to the direction and
    parallel to and perpendicular to the horizon).

    Note that, due to the intrinsic symmetry of the antenna configuration, the direction of Azimuth (CW or CCW)
    is not relevant.
    """

    parameters = dict(
        fft_data = dict( default = None,
            doc = "FFT data." ),
        frequencies = dict( default = None,
            doc = "Frequencies." ),
        direction = dict( default = (0, 0),
            doc = "Direction in degrees as a (Azimuth, Elevation) tuple." ),
        nantennas = dict( default = lambda self : self.fft_data.shape()[0],
            doc = "Number of antennas." ),
        antennaset = dict ( default = "LBA_OUTER",
            doc = "Antennaset." ),
        inverse_jones_matrix = dict( default = lambda self : cr.hArray(complex, dimensions = (self.frequencies.shape()[0], 2, 2)),
            doc = "Inverse Jones matrix for each frequency." ),
        on_sky_polarization = dict( default = lambda self : self.fft_data.new(), output = True,
            doc = "FFT data corrected for element response (contains on sky polarizations)." ),
        normalize = dict( default = True,
            doc = "Normalize to frequency response in zenith." ),
        test_with_unity_matrix = dict ( default = False,
            doc = "Use unity inverse Jones matrix for testing purposes." ),
    )

    def run(self):
        """Run.
        """

        # Copy FFT data over for correction
        self.on_sky_polarization.copy(self.fft_data)

        # Get inverse Jones matrix for each frequency
        if self.test_with_unity_matrix:
            print "[AntennaResponse] using unity inverse Jones matrix for unfolding"
            cr.hGetUnityInverseJonesMatrix(self.inverse_jones_matrix)
        elif "LBA" in self.antennaset:
            if self.normalize:
                cr.hGetNormalizedInverseJonesMatrixLBA(self.inverse_jones_matrix, self.frequencies,
                    pytmf.deg2rad(self.direction[0]), pytmf.deg2rad(self.direction[1]))
            else:
                cr.hGetInverseJonesMatrixLBA(self.inverse_jones_matrix, self.frequencies,
                    pytmf.deg2rad(self.direction[0]), pytmf.deg2rad(self.direction[1]))
        elif "HBA" in self.antennaset:
            cr.hGetInverseJonesMatrixHBA(self.inverse_jones_matrix, self.frequencies,
                pytmf.deg2rad(self.direction[0]), pytmf.deg2rad(self.direction[1]))
        else:
            raise ValueError("Invalid antennaset " + self.antennaset)

        # Unfold the antenna response and mix polarizations according to the Jones matrix to get the on-sky polarizations
        cr.hGetOnSkyPolarizations(self.on_sky_polarization[0:self.nantennas:2,...], self.on_sky_polarization[1:self.nantennas:2,...], self.inverse_jones_matrix)

