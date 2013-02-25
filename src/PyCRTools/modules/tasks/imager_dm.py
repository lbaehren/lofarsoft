"""
Imager documentation
====================

.. moduleauthor:: Name of the module author <email address of the module author>
"""

from pycrtools.tasks import Task
from pycrtools.grid import CoordinateGrid
import pycrtools as cr
import pytmf
import numpy as np
import time

cr.tasks.__raiseTaskDeprecationWarning(__name__)

class ImagerDM(Task):
    """Imager task documentation.
    """

    parameters = {
        'image': {"default": None, "positional": 1},
        'data': {"default": None, "positional": 2},
        'mask': {"default": None, "positional": 3},
        'startblock': {"default": 0},
        'nblocks': {"default": 16},
        'ntimesteps': {"default": 1},
        'nfmin': {"default": None},
        'nfmax': {"default": None},
        'obstime': {"default": 0},
        'L': {"default": pytmf.deg2rad(6.869837540)},
        'phi': {"default": pytmf.deg2rad(52.915122495)},
        'NAXIS': {"default": 2},
        'NAXIS1': {"default": 90},
        'NAXIS2': {"default": 90},
        'CTYPE1': {"default": 'ALON-STG'},
        'CTYPE2': {"default": 'ALAT-STG'},
        'LONPOLE': {"default": 0.},
        'LATPOLE': {"default": 90.},
        'CRVAL1': {"default": 180.},
        'CRVAL2': {"default": 90.},
        'CRPIX1': {"default": 45.5},
        'CRPIX2': {"default": 45.5},
        'CDELT1': {"default": 2.566666603088E+00},
        'CDELT2': {"default": 2.566666603088E+00},
        'CUNIT1': {"default": 'deg'},
        'CUNIT2': {"default": 'deg'},
        'PC001001': {"default": 1.000000000000E+00},
        'PC002001': {"default": 0.000000000000E+00},
        'PC001002': {"default": 0.000000000000E+00},
        'PC002002': {"default": 1.000000000000E+00},
        'DM': {"default": None},
        'dt': {"default": None},
        'inversefft': {"default": False}
    }

    def init(self):
        """Initialize imager.
        """

        # Generate coordinate grid
        print "Generating grid"
        self.grid = CoordinateGrid(obstime=self.obstime,
                                 L=self.L,
                                 phi=self.phi,
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

        if self.nfmin != None and self.nfmax != None:
            self.frequencies = self.frequencies[range(self.nfmin, self.nfmax)]

        self.nfreq = len(self.frequencies)

        print "Frequency range:", self.frequencies[0], self.frequencies[-1], "Hz"

        # Calculate dispersion measure shifts if requested
        if self.DM:
            self.dispersion_shifts = cr.hArray(int, self.nfreq)
            cr.hDedispersionShifts(self.dispersion_shifts, self.frequencies, cr.hMin(self.frequencies).val(), self.DM, self.dt)

            print "Dedispersion shifts", self.dispersion_shifts

        # Get antenna positions
        self.antpos = self.data.getRelativeAntennaPositions()
        self.nantennas = int(self.antpos.shape()[0])

        # Calculate geometric delays for all sky positions for all antennas
        self.delays = cr.hArray(float, dimensions=(self.NAXIS1, self.NAXIS2, self.nantennas))
        cr.hGeometricDelays(self.delays, self.antpos, self.grid.cartesian, True)

        # Create plan for inverse FFT if needed
        if self.inversefft:
            self.blocksize = (self.nfreq - 1) * 2  # self.data["BLOCKSIZE"]
            self.plan = cr.FFTWPlanManyDftC2r(self.blocksize, self.NAXIS1 * self.NAXIS2, 1, self.nfreq, 1, self.blocksize, cr.fftw_flags.ESTIMATE)
            print "created inverse fft plan"
            self.t_image2 = cr.hArray(float, dimensions=(self.NAXIS1, self.NAXIS2, self.blocksize), fill=0.)

        # Initialize empty arrays
        self.scratchfft = self.data.empty("FFT_DATA")
        self.fftdata = cr.hArray(complex, dimensions=(self.nantennas, self.nfreq))
        self.t_image = cr.hArray(complex, dimensions=(self.NAXIS1, self.NAXIS2, self.nfreq), fill=0.)

    def run(self):
        """Run the imager.
        """

        # Get steps corresponding to mask
        mask = self.mask
        step = cr.hArray(int, list(cr.hCountZero(mask)))
        cr.hMaskToStep(step, mask)

        start = time.time()
        for tstep in range(self.ntimesteps):
            for block in range(self.startblock, self.startblock + self.nblocks):

                print "processing block:", block

                if self.nfmin != None and self.nfmax != None:
                    self.data.getFFTData(self.scratchfft, block)

                    self.fftdata[...].copy(self.scratchfft[..., self.nfmin:self.nfmax])
                else:
                    self.data.getFFTData(self.fftdata, block)

                cr.hFFTConvert(self.fftdata[...])
                np.save("indata", self.fftdata.toNumpy())

                print "reading done"

                self.t_image.fill(0.)

                print "beamforming started"
                cr.hBeamformImage(self.t_image, self.fftdata, self.frequencies, self.delays)
#                cr.hBeamformImage(self.t_image, self.fftdata, self.frequencies, self.antpos, self.grid.cartesian)
#                cr.hBeamformImage(self.t_image, self.fftdata, self.frequencies, self.antpos, self.grid.cartesian, step)
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
        print "total runtime:", end - start, "s"
