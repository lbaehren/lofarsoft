"""
CRImager documentation
======================

.. moduleauthor:: Name of the module author <email address of the module author>

"""

from pycrtools.tasks import Task
from pycrtools.grid import CoordinateGrid
import pycrtools as cr
import pytmf
import numpy as np
import time
import pyfits
import os

cr.tasks.__raiseTaskDeprecationWarning(__name__)

def savefits(filename, array, overwrite=True, **kwargs):
    """Save image as standard FITS file.
    """

    # Convert image to Numpy array if required
    if cr.ishArray(array):
        array = array.toNumpy()

    # Create new FITS structure (array needs to be transposed for FITS convention)
    hdu = pyfits.PrimaryHDU(array.transpose())

    # Create FITS header
    hdr = hdu.header

    for key in sorted(kwargs.iterkeys()):

        # NAXIS are calculated by pyfits and need to match
        if not "NAXIS" in key:

            # Correct for ugly bug in casaviewer
            if "CTYPE" in key and "ALON" in kwargs[key]:
                hdr.update(key, "??LN-" + kwargs[key][5:])
            elif "CTYPE" in key and "ALAT" in kwargs[key]:
                hdr.update(key, "??LT-" + kwargs[key][5:])
            else:
                hdr.update(key, kwargs[key])

    # Check if file exists and overwrite if requested
    if not filename.endswith(".fits"):
        filename += ".fits"
    if os.path.isfile(filename) and overwrite:
        os.remove(filename)
    hdu.writeto(filename)


class CRImager(Task):
    """CRImager task documentation.
    """

    parameters = dict(
        output=dict(default=None),
        data=dict(default=None),
        fftdata=dict(default=None),
        frequencies=dict(default=None),
        antpos=dict(default=None),
        blocksize=dict(default=None),
        nf=dict(default=lambda self: self.blocksize / 2 + 1),
        nantennas=dict(default=lambda self: self.antpos.shape()[0]),
        nyquist_zone=dict(default=1),
        sample_frequency=dict(default=200.e6),
        image=dict(default=lambda self: np.zeros(shape=(self.blocksize, self.NAXIS1, self.NAXIS2), dtype=float),
            doc="Array to hold output image, needs to be a numpy array because otherwise conversion to FITS will require twice the memory."),
        frequency_domain_image = dict(default=lambda self: cr.hArray(complex, dimensions=(self.NAXIS1, self.NAXIS2, self.nf), fill=0.)),
        time_domain_image=dict(default = lambda self: cr.hArray(float, dimensions=(self.NAXIS1, self.NAXIS2, self.blocksize), fill=0.)),
        OBSTIME=dict(default=None),
        OBSLON=dict(default=pytmf.deg2rad(6.869837540), doc="Observer longitude in radians"),
        OBSLAT=dict(default=pytmf.deg2rad(52.915122495), doc="Observer latitude in radians"),
        NAXIS=dict(default=3),
        NAXIS1=dict(default=180),
        NAXIS2=dict(default=180),
        NAXIS3=dict(default=1),
        LONPOLE=dict(default=0.),
        LATPOLE=dict(default=90.),
        CTYPE1=dict(default='ALON-STG'),
        CTYPE2=dict(default='ALAT-STG'),
        CTYPE3=dict(default='TIME'),
        CRVAL1=dict(default=180.),
        CRVAL2=dict(default=90.),
        CRVAL3=dict(default=0.),
        CRPIX1=dict(default=lambda self: float(self.NAXIS1) / 2),
        CRPIX2=dict(default=lambda self: float(self.NAXIS2) / 2),
        CRPIX3=dict(default=0.),
        CDELT1=dict(default=-1.),
        CDELT2=dict(default=1.),
        CDELT3=dict(default=lambda self: 1. / self.sample_frequency),
        CUNIT1=dict(default='deg'),
        CUNIT2=dict(default='deg'),
        CUNIT3=dict(default='s'),
        PC001001=dict(default=1.000000000000E+00),
        PC002001=dict(default=0.000000000000E+00),
        PC001002=dict(default=0.000000000000E+00),
        PC002002=dict(default=1.000000000000E+00)
    )

    def init(self):
        """Initialize imager.
        """

        # Generate coordinate grid
        self.grid = CoordinateGrid(obstime=self.OBSTIME,
                                 L=self.OBSLON,
                                 phi=self.OBSLAT,
                                 NAXIS=self.NAXIS,
                                 NAXIS1=self.NAXIS1,
                                 NAXIS2=self.NAXIS2,
                                 CTYPE1=self.CTYPE1,
                                 CTYPE2=self.CTYPE2,
                                 LONPOLE=self.LONPOLE,
                                 LATPOLE=self.LATPOLE,
                                 CRVAL1=self.CRVAL1,
                                 CRVAL2=self.CRVAL2,
                                 CRPIX1=self.CRPIX1,
                                 CRPIX2=self.CRPIX2,
                                 CDELT1=self.CDELT1,
                                 CDELT2=self.CDELT2,
                                 CUNIT1=self.CUNIT1,
                                 CUNIT2=self.CUNIT2,
                                 PC001001=self.PC001001,
                                 PC002001=self.PC002001,
                                 PC001002=self.PC001002,
                                 PC002002=self.PC002002)
        print self.grid

        # Calculate frequencies if not given
        if self.frequencies is None:
            self.frequencies = cr.hArray(float, self.nf)
            cr.hFFTFrequencies(self.frequencies, self.sample_frequency, self.nyquist_zone)

        # Calculate geometric delays for all sky positions for all antennas
        self.delays = cr.hArray(float, dimensions=(self.NAXIS1, self.NAXIS2, self.nantennas))
        cr.hGeometricDelays(self.delays, self.antpos, self.grid.cartesian, True)

        # Create plan for forward FFT
        self.plan = cr.FFTWPlanManyDftR2c(self.blocksize, self.nantennas, 1, self.blocksize, 1, self.nf, cr.fftw_flags.ESTIMATE)

        # Create plan for inverse FFT
        self.iplan = cr.FFTWPlanManyDftC2r(self.blocksize, self.NAXIS1 * self.NAXIS2, 1, self.nf, 1, self.blocksize, cr.fftw_flags.ESTIMATE)

    def run(self):
        """Run the imager.
        """

        # Make sure image array is contiguous
        if not self.image.flags["C_CONTIGUOUS"]:
            self.image = np.ascontiguousarray(self.image)

        # Go to frequency domain
        if self.fftdata is None:

            # Create empty array
            self.fftdata = cr.hArray(complex, dimensions=(self.nantennas, self.nf))

            # Perform FFT
            cr.hFFTWExecutePlan(self.fftdata, self.data, self.plan)

            # Swap Nyquist zone if needed
            self.fftdata[...].nyquistswap(self.nyquist_zone)

            # Convert FFT
            cr.hFFTConvert(self.fftdata[...])

        # Beamforming
        cr.hBeamformImage(self.frequency_domain_image, self.fftdata, self.frequencies, self.delays)

        # Transform back to time domain
        cr.hFFTWExecutePlan(self.time_domain_image, self.frequency_domain_image, self.iplan)

        # Calculate power for each pixel
        cr.hSquareAddTransposed(self.image, self.time_domain_image, self.blocksize)

        # Save image to disk
        if self.output is not None:
            self.image = self.image.reshape((self.blocksize, self.NAXIS1, self.NAXIS2))
            self.image = np.rollaxis(self.image, 0, 3)

            savefits(self.output, self.image,
                     OBSLON=self.OBSLON,
                     OBSLAT=self.OBSLAT,
                     CTYPE1=self.CTYPE1,
                     CTYPE2=self.CTYPE2,
                     CTYPE3=self.CTYPE3,
                     LONPOLE=self.LONPOLE,
                     LATPOLE=self.LATPOLE,
                     CRVAL1=self.CRVAL1,
                     CRVAL2=self.CRVAL2,
                     CRVAL3=self.CRVAL3,
                     CRPIX1=self.CRPIX1,
                     CRPIX2=self.CRPIX2,
                     CRPIX3=self.CRPIX3,
                     CDELT1=self.CDELT1,
                     CDELT2=self.CDELT2,
                     CDELT3=self.CDELT3,
                     CUNIT1=self.CUNIT1,
                     CUNIT2=self.CUNIT2,
                     CUNIT3=self.CUNIT3,
                     PC001001=self.PC001001,
                     PC002001=self.PC002001,
                     PC001002=self.PC001002,
                     PC002002=self.PC002002)
