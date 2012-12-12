"""
Imager documentation
====================

"Representations of celestial coordinates in FITS"
http://adsabs.harvard.edu/abs/2002A%26A...395.1077C

"""

from pycrtools.tasks import Task
from pycrtools.grid import CoordinateGrid
import pycrtools as cr
import pytmf
import numpy as np
import time
import pyfits
import os
import pdb
# pdb.set_trace()


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


class Imager(Task):
    """Imager task documentation.
    """

    parameters = dict(
        data=dict(default=None,
                     positional=1),
        image=dict(default=None),
        output=dict(default="out.fits"),
        startblock=dict(default=0),
        nblocks=dict(default=1),
        ntimesteps=dict(default=lambda self: self.data["MAXIMUM_READ_LENGTH"] / (self.nblocks * self.data["BLOCKSIZE"])),
        intgrfreq=dict(default=False,
                          doc="Output frequency integrated image."),
        inversefft=dict(default=False),
        rfi_remove=dict(default=None, doc='List of frequency indices to remove.'),
        FREQMIN=dict(default=None),
        FREQMAX=dict(default=None),
        OBSTIME=dict(default=lambda self: self.data["TIME"][0]),
        OBSLON=dict(default=pytmf.deg2rad(6.869837540),
                       doc="Observer longitude in radians"),
        OBSLAT=dict(default=pytmf.deg2rad(52.915122495),
                       doc="Observer latitude in radians"),
        NAXIS=dict(default=4),
        NAXIS1=dict(default=90),
        NAXIS2=dict(default=90),
        NAXIS3=dict(default=1),
        NAXIS4=dict(default=1),
        LONPOLE=dict(default=0.),
        LATPOLE=dict(default=90.),
        CTYPE1=dict(default='ALON-STG'),
        CTYPE2=dict(default='ALAT-STG'),
        CTYPE3=dict(default='FREQ'),
        CTYPE4=dict(default='TIME'),
        CRVAL1=dict(default=180.),
        CRVAL2=dict(default=90.),
        CRVAL3=dict(default=0.),
        CRVAL4=dict(default=0.),
        CRPIX1=dict(default=lambda self: float(self.NAXIS1) / 2),
        CRPIX2=dict(default=lambda self: float(self.NAXIS2) / 2),
        CRPIX3=dict(default=0.),
        CRPIX4=dict(default=0.),
        CDELT1=dict(default=2.566666603088E+00),
        CDELT2=dict(default=2.566666603088E+00),
        CDELT3=dict(default=lambda self: self.data["FREQUENCY_INTERVAL"][0]),
        CDELT4=dict(default=lambda self: self.nblocks * self.data["BLOCKSIZE"] * self.data["SAMPLE_INTERVAL"][0]),
        CUNIT1=dict(default='deg'),
        CUNIT2=dict(default='deg'),
        CUNIT3=dict(default='Hz'),
        CUNIT4=dict(default='s'),
        PC001001=dict(default=1.000000000000E+00),
        PC002001=dict(default=0.000000000000E+00),
        PC001002=dict(default=0.000000000000E+00),
        PC002002=dict(default=1.000000000000E+00)
    )

    def init(self):
        """Initialize imager.
        """

        # Generate coordinate grid
        print "Generating grid"

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
        print "Grid generation finished"
        print self.grid

        # Get frequencies
        self.frequencies = self.data.getFrequencies()

        self.frequency_slice = None
        if self.FREQMIN != None and self.FREQMAX != None:
            self.frequency_slice = cr.hArray(int, 2)
            cr.hFindSequenceBetweenOrEqual(self.frequency_slice, self.frequencies, self.FREQMIN, self.FREQMAX, 0, 0)
            self.frequencies = cr.hArray(list(self.frequencies[self.frequency_slice[0]:self.frequency_slice[1]].vec()))

        self.nfreq = len(self.frequencies)

        print "Frequency range:", self.frequencies[0], self.frequencies[-1], "Hz"

        # Get antenna positions
        self.antpos = self.data.getRelativeAntennaPositions()
        self.nantennas = int(self.antpos.shape()[0])

        # Calculate geometric delays for all sky positions for all antennas
        self.delays = cr.hArray(float, dimensions=(self.NAXIS1, self.NAXIS2, self.nantennas))
        cr.hGeometricDelays(self.delays, self.antpos, self.grid.cartesian, True)

        # Create plan for inverse FFT if needed
        if self.inversefft:
            self.blocksize = (self.nfreq - 1) * 2
            self.plan = cr.FFTWPlanManyDftC2r(self.blocksize, self.NAXIS1 * self.NAXIS2, 1, self.nfreq, 1, self.blocksize, cr.fftw_flags.ESTIMATE)
            print "created inverse fft plan"
            self.t_image2 = cr.hArray(float, dimensions=(self.NAXIS1, self.NAXIS2, self.blocksize), fill=0.)

        # Initialize empty arrays
        self.scratchfft = self.data.empty("FFT_DATA")
        self.fftdata = cr.hArray(complex, dimensions=(self.nantennas, self.nfreq))
        self.t_image = cr.hArray(complex, dimensions=(self.NAXIS1, self.NAXIS2, self.nfreq), fill=0.)

        # Create image array if none is given as input
        if self.image == None:
            if self.intgrfreq:
                self.image = np.zeros(shape=(self.ntimesteps, self.NAXIS1, self.NAXIS2), dtype=float)
            elif self.inversefft:
                self.image = np.zeros(shape=(self.ntimesteps, self.blocksize, self.NAXIS1, self.NAXIS2), dtype=float)
            else:
                self.image = np.zeros(shape=(self.ntimesteps, self.NAXIS1, self.NAXIS2, self.nfreq), dtype=float)

    def run(self):
        """Run the imager.
        """

        start = time.time()
        for tstep in range(self.ntimesteps):
            print "processing timestep: %d/%d" % (tstep, self.ntimesteps)

            for block in range(self.startblock, self.startblock + self.nblocks):

                if self.frequency_slice != None:
                    self.data.getFFTData(self.scratchfft, block)

                    self.fftdata[...].copy(self.scratchfft[..., self.frequency_slice[0]:self.frequency_slice[1]])
                else:
                    self.data.getFFTData(self.fftdata, block)

                cr.hFFTConvert(self.fftdata[...])

                # Revoming RFI
                if self.rfi_remove:
                    self.fftdata[..., self.rfi_remove] = 0
                print "reading done"

                print "beamforming started"
                self.t_image.fill(0.)
                cr.hBeamformImage(self.t_image, self.fftdata, self.frequencies, self.delays)

                if self.intgrfreq:
                    cr.hFrequencyIntegratedImage(self.image[tstep], self.t_image)                    
                elif self.inversefft:
                    cr.hFFTWExecutePlan(self.t_image2, self.t_image, self.plan)
                    cr.hSquareAddTransposed(self.image[tstep], self.t_image2, self.blocksize)
                else:
                    cr.hAbsSquareAdd(self.image[tstep], self.t_image)

                print "beamforming done"

            self.startblock += self.nblocks

        end = time.time()
        print "total runtime:", end - start, "s"

        # Save image to disk

        if self.inversefft:
            self.image = self.image.reshape((self.ntimesteps * self.blocksize, self.NAXIS1, self.NAXIS2))
            self.image = np.rollaxis(self.image, 0, 3)
        elif self.intgrfreq:
            self.image = np.rollaxis(self.image, 0, 3)
        else:
            self.image = np.rollaxis(self.image, 0, 4)

        if self.inversefft:
            savefits(self.output, self.image,
                        OBSLON=self.OBSLON,
                        OBSLAT=self.OBSLAT,
                        CTYPE1=self.CTYPE1,
                        CTYPE2=self.CTYPE2,
                        CTYPE3="TIME",
                        LONPOLE=self.LONPOLE,
                        LATPOLE=self.LATPOLE,
                        CRVAL1=self.CRVAL1,
                        CRVAL2=self.CRVAL2,
                        CRVAL3=0.,
                        CRPIX1=self.CRPIX1,
                        CRPIX2=self.CRPIX2,
                        CRPIX3=0.,
                        CDELT1=self.CDELT1,
                        CDELT2=self.CDELT2,
                        CDELT3=self.data["SAMPLE_INTERVAL"][0],
                        CUNIT1=self.CUNIT1,
                        CUNIT2=self.CUNIT2,
                        CUNIT3="s",
                        PC001001=self.PC001001,
                        PC002001=self.PC002001,
                        PC001002=self.PC001002,
                        PC002002=self.PC002002)

        elif self.intgrfreq:
            savefits(self.output, self.image,
                        OBSLON=self.OBSLON,
                        OBSLAT=self.OBSLAT,
                        CTYPE1=self.CTYPE1,
                        CTYPE2=self.CTYPE2,
                        CTYPE3="TIME",
                        LONPOLE=self.LONPOLE,
                        LATPOLE=self.LATPOLE,
                        CRVAL1=self.CRVAL1,
                        CRVAL2=self.CRVAL2,
                        CRVAL3=0.,
                        CRPIX1=self.CRPIX1,
                        CRPIX2=self.CRPIX2,
                        CRPIX3=0.,
                        CDELT1=self.CDELT1,
                        CDELT2=self.CDELT2,
                        CDELT3=self.CDELT4,
                        CUNIT1=self.CUNIT1,
                        CUNIT2=self.CUNIT2,
                        CUNIT3="s",
                        PC001001=self.PC001001,
                        PC002001=self.PC002001,
                        PC001002=self.PC001002,
                        PC002002=self.PC002002)

        else:
            savefits(self.output, self.image,
                        OBSLON=self.OBSLON,
                        OBSLAT=self.OBSLAT,
                        CTYPE1=self.CTYPE1,
                        CTYPE2=self.CTYPE2,
                        CTYPE3=self.CTYPE3,
                        CTYPE4=self.CTYPE4,
                        LONPOLE=self.LONPOLE,
                        LATPOLE=self.LATPOLE,
                        CRVAL1=self.CRVAL1,
                        CRVAL2=self.CRVAL2,
                        CRVAL3=self.CRVAL3,
                        CRVAL4=self.CRVAL4,
                        CRPIX1=self.CRPIX1,
                        CRPIX2=self.CRPIX2,
                        CRPIX3=self.CRPIX3,
                        CRPIX4=self.CRPIX4,
                        CDELT1=self.CDELT1,
                        CDELT2=self.CDELT2,
                        CDELT3=self.CDELT3,
                        CDELT4=self.CDELT4,
                        CUNIT1=self.CUNIT1,
                        CUNIT2=self.CUNIT2,
                        CUNIT3=self.CUNIT3,
                        CUNIT4=self.CUNIT4,
                        PC001001=self.PC001001,
                        PC002001=self.PC002001,
                        PC001002=self.PC001002,
                        PC002002=self.PC002002)
