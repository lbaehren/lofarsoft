"""
Imager documentation
====================

"""

from pycrtools.tasks import Task
from pycrtools.grid import CoordinateGrid
import pycrtools as cr
import pytmf
import numpy as np
import time
import pyfits
import os

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

    parameters = {
        'data' : { "default" : None, "positional" : 1 },
        'image' : { "default" : None },
        'output' : { "default" : "out.fits" },
        'startblock' : { "default" : 0 },
        'nblocks' : { "default" : 16 },
        'ntimesteps' : { "default" : lambda self : self.data["MAXIMUM_READ_LENGTH"] / (self.nblocks * self.data["BLOCKSIZE"]) },
        'intgrfreq' : { "default" : False, "doc" : "Output frequency integrated image." },
        'inversefft' : { "default" : False },
        'mask' : { "default" : None },
        'FREQMIN' : { "default" : None },
        'FREQMAX' : { "default" : None },
        'OBSTIME' : { "default" : lambda self : self.data["TIME"][0] },
        'DM' : { "default" : None },
        'OBSLON' : { "default" : pytmf.deg2rad(6.869837540), "doc" : "Observer longitude in radians" },
        'OBSLAT' : { "default" : pytmf.deg2rad(52.915122495), "doc" : "Observer latitude in radians" },
        'NAXIS' : { "default" : 4 },
        'NAXIS1' : { "default" : 90 },
        'NAXIS2' : { "default" : 90 },
        'NAXIS3' : { "default" : 1 },
        'NAXIS4' : { "default" : 1 },
        'LONPOLE' : { "default" : 0. },
        'LATPOLE' : { "default" : 90. },
        'CTYPE1' : { "default" : 'ALON-STG' },
        'CTYPE2' : { "default" : 'ALAT-STG' },
        'CTYPE3' : { "default" : 'FREQ' },
        'CTYPE4' : { "default" : 'TIME' },
        'CRVAL1' : { "default" : 180. },
        'CRVAL2' : { "default" : 90. },
        'CRVAL3' : { "default" : 0. },
        'CRVAL4' : { "default" : 0. },
        'CRPIX1' : { "default" : lambda self : float(self.NAXIS1) / 2 },
        'CRPIX2' : { "default" : lambda self : float(self.NAXIS2) / 2 },
        'CRPIX3' : { "default" : 0. },
        'CRPIX4' : { "default" : 0. },
        'CDELT1' : { "default" : 2.566666603088E+00 },
        'CDELT2' : { "default" : 2.566666603088E+00 },
        'CDELT3' : { "default" : 0.0+00 },
        'CDELT4' : { "default" : 0.0+00 },
        'CUNIT1' : { "default" : 'deg' },
        'CUNIT2' : { "default" : 'deg' },
        'CUNIT3' : { "default" : 'Hz' },
        'CUNIT4' : { "default" : 's' },
        'PC001001' : { "default" : 1.000000000000E+00 },
        'PC002001' : { "default" : 0.000000000000E+00 },
        'PC001002' : { "default" : 0.000000000000E+00 },
        'PC002002' : { "default" : 1.000000000000E+00 }
    }

    def init(self):
        """Initialize imager.
        """

        # Generate coordinate grid
        print "Generating grid"
        self.grid=CoordinateGrid(OBSTIME=self.OBSTIME,
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
        self.frequencies=self.data.getFrequencies()

        self.frequency_slice = None
        if self.FREQMIN != None and self.FREQMAX != None:
            frequency_slice = cr.hArray(int, 2)
            cr.hFindSequenceBetweenOrEqual(frequency_slice, self.frequencies, self.FREQMIN, self.FREQMAX, 0, 0)
            self.frequencies=self.frequencies[frequency_slice[0]:frequency_slice[1]]

        self.nfreq = len(self.frequencies)

        print "Frequency range:", self.frequencies[0], self.frequencies[-1], "Hz"

        # Calculate dispersion measure shifts if requested
        if self.DM:
            self.dispersion_shifts = cr.hArray(int, self.nfreq)
            cr.hDedispersionShifts(self.dispersion_shifts, self.frequencies, cr.hMin(self.frequencies).val(), self.DM, self.CDELT4)

            print "Dedispersion shifts", self.dispersion_shifts

        # Get steps corresponding to mask if given
        if self.mask != None:
            self.step = cr.hArray(int, cr.hCountZero(self.mask))
            cr.hMaskToStep(self.step, self.mask)

        # Get antenna positions
        self.antpos=self.data.getRelativeAntennaPositions()
        self.nantennas=int(self.antpos.getDim()[0])

        # Calculate geometric delays for all sky positions for all antennas
        self.delays = cr.hArray(float, dimensions=(self.NAXIS1, self.NAXIS2, self.nantennas))
        cr.hGeometricDelays(self.delays, self.antpos, self.grid.cartesian, True)

        # Create plan for inverse FFT if needed
        if self.inversefft:
            self.blocksize = (self.nfreq - 1) * 2#self.data["BLOCKSIZE"]
            self.plan = cr.FFTWPlanManyDftC2r(self.blocksize, self.NAXIS1 * self.NAXIS2, 1, self.nfreq, 1, self.blocksize, cr.fftw_flags.ESTIMATE)
            print "created inverse fft plan"
            self.t_image2=cr.hArray(float, dimensions=(self.NAXIS1, self.NAXIS2, self.blocksize), fill=0.)

        # Initialize empty arrays
        self.scratchfft = self.data.empty("FFT_DATA")
        self.fftdata=cr.hArray(complex, dimensions=(self.nantennas, self.nfreq))
        self.t_image=cr.hArrayrcomplex, dimensions=(self.NAXIS1, self.NAXIS2, self.nfreq), fill=0.)

        # Create image array if none is given as input
        if not image:
            if self.intgrfreq:
                self.image = np.zeros(shape=(self.ntimesteps, self.NAXIS1, self.NAXIS2), dtype=float)
            else:
                self.image = np.zeros(shape=(self.ntimesteps, self.NAXIS1, self.NAXIS2, self.nfreq), dtype=float)

        # Create image array if none is given as input
        if not self.image:
            if self.intgrfreq:
                self.image = np.zeros(shape=(self.ntimesteps, self.NAXIS1, self.NAXIS2), dtype=float)
            else:
                self.image = np.zeros(shape=(self.ntimesteps, self.NAXIS1, self.NAXIS2, self.nfreq), dtype=float)

    def run(self):
        """Run the imager.
        """

        start = time.time()
        for tstep in range(self.ntimesteps):
            for block in range(self.startblock, self.startblock+self.nblocks):

                print "processing block:", block

                if self.frequency_slice != None:
                    self.data.getFFTData(self.scratchfft, block)

                    self.fftdata[...].copy(self.scratchfft[..., self.frequency_slice[0]:self.frequency_slice[1]])
                else:
                    self.data.getFFTData(self.fftdata, block)

                cr.hFFTConvert(self.fftdata[...])
                np.save("indata", self.fftdata.toNumpy())

                print "reading done"

                self.t_image.fill(0.)

                print "beamforming started"
                cr.hBeamformImage(self.t_image, self.fftdata, self.frequencies, self.delays)
#                cr.hBeamformImage(self.t_image, self.fftdata, self.frequencies, self.antpos, self.grid.cartesian)
#                cr.hBeamformImage(self.t_image, self.fftdata, self.frequencies, self.antpos, self.grid.cartesian, self.step)
                print "beamforming done"

                if self.DM:
                    cr.hShiftedAbsSquareAdd(self.image, self.t_image, self.dispersion_shifts + tstep)
                elif self.inversefft:
                    cr.hFFTWExecutePlan(self.t_image2, self.t_image, self.plan)
                    cr.hSquareAdd(self.image[tstep], self.t_image2)
                else:
                    cr.hAbsSquareAdd(self.image[tstep], self.t_image)

            self.startblock += self.nblocks

        end = time.time()
        print "total runtime:", end-start, "s"

        # Save image to disk

        self.image = np.rollaxis(self.image, 0, 3)

        savefits(self.output, self.image, 
                    OBSLON=self.OBSLON,
                    OBSLAT=self.OBSLAT,
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

