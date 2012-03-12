"""
AntennaResponse documentation
=============================

"""

from pycrtools.tasks import Task
import pycrtools as cr
import pytmf

class AntennaResponse(Task):
    """AntennaResponse task documentation.
    """

    parameters = dict(
        fft_data = dict( default = None,
            doc = "FFT data." ),
        frequencies = dict( default = None,
            doc = "Frequencies." ),
        on_sky_polarizations = dict( default = lambda self : self.fft_data.new(), output = True,
            doc = "FFT data corrected for element response (contains on sky polarizations)." ),
        nantennas = dict( default = lambda self : self.fft_data.shape()[0],
            doc = "Number of antennas." ),
        antennaset = dict ( default = "LBA_OUTER",
            doc = "Antennaset." ),
        direction = dict( default = (0, 0),
            doc = "Direction in degrees." ),
    )

    def run(self):
        """Run.
        """

        print "Running AntennaResponse..."

        # Copy FFT data over for correction
        self.on_sky_polarizations.copy(self.fft_data)

        # Apply correction
        if "LBA" in self.antennaset:
            cr.hCalibratePolarizationLBA(self.on_sky_polarizations[0:self.nantennas:2,...], self.on_sky_polarizations[1:self.nantennas:2,...],
                self.frequencies, pytmf.deg2rad(self.direction[0]), pytmf.deg2rad(self.direction[1]))
        elif "HBA" in self.antennaset:
            cr.hCalibratePolarizationHBA(self.on_sky_polarizations[0:self.nantennas:2,...], self.on_sky_polarizations[1:self.nantennas:2,...],
                self.frequencies, pytmf.deg2rad(self.direction[0]), pytmf.deg2rad(self.direction[1]))
        else:
            raise ValueError("Invalid antennaset " + self.antennaset)

