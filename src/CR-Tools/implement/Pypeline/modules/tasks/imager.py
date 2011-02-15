"""Imager documentation.
"""

# What is exported by default
__all__ = ['imager']

from pycrtools.tasks import Task
from pycrtools.grid import CoordinateGrid
import pycrtools as cr
import pytmf

# Default all-sky image
wcs_default = {
    'NAXIS' : 2,
    'NAXIS1' : 90,
    'NAXIS2' : 90,
    'CTYPE1' : 'ALON-STG',
    'CTYPE2' : 'ALAT-STG',
    'LONPOLE' : 0.,
    'LATPOLE' : 90.,
    'CRVAL1' : 180.,
    'CRVAL2' : 90.,
    'CRPIX1' : 45.5,
    'CRPIX2' : 45.5,
    'CDELT1' : 2.566666603088E+00,
    'CDELT2' : 2.566666603088E+00,
    'CUNIT1' : 'deg',
    'CUNIT2' : 'deg',
    'PC001001' : 1.000000000000E+00,
    'PC002001' : 0.000000000000E+00,
    'PC001002' : 0.000000000000E+00,
    'PC002002' : 1.000000000000E+00
}

class Imager(Task):
    """Imager class documentation.
    """

    def call(self, #This function actually never gets called
	     image,
	     data,
	     startblock=0,
	     nblocks=16,
	     ntimesteps=1,
	     obstime=0, # Not a sensible default
	     L=pytmf.deg2rad(6.869837540),
	     phi=pytmf.deg2rad(52.915122495),
	     wcs=wcs_default):
         
	# Store reference to image and data
	self.image = image
	self.data = data

        # Store image parameters
        self.startblock = startblock
        self.nblocks = nblocks
        self.ntimesteps = ntimesteps
        self.obstime = obstime
	self.wcs=wcs
	self.phi=phi
	self.L=L
	    
    def init(self):

        """Initialize the imager.

        *data* IO object for accessing data
        *L* observatory longitude (default center of LOFAR CS002)
        *phi* observatory latitude (default center of LOFAR CS002)
        *wcs* FITS world coordinate system parameters used for grid
              (default all-sky image in AZEL)
        """

        # Start initialization
        print "Initializing imager - setting derived parameters (beter use WorkSpace)"

        # Generate coordinate grid
        print "Generating grid"
        self.grid=CoordinateGrid(obstime=obstime, L=self.L, phi=self.phi, **self.wcs)
        print "Grid generation finished"

        # Get frequencies
        self.frequencies=self.data.getFrequencies()
        self.nfreq = len(self.frequencies)

        print "Frequency range:", self.frequencies[0], self.frequencies[-1], "Hz"

        # Get antenna positions
        self.antpos=self.data.getRelativeAntennaPositions()
        self.nantennas=int(self.antpos.getDim()[0])

        # Calculate geometric delays for all sky positions for all antennas
        self.delays = cr.hArray(float, dimensions=(wcs['NAXIS1'], wcs['NAXIS2'], self.nantennas))
        cr.hGeometricDelays(self.delays, self.antpos, self.grid.cartesian, True)

        # Initialize empty arrays
        self.fftdata=cr.hArray(complex, dimensions=(self.nantennas, self.nfreq))
        self.t_image=cr.hArray(complex, dimensions=(wcs['NAXIS1'], wcs['NAXIS2'], self.nfreq), fill=0.)

    def run(self):
        """Run the imager.
        """

        for step in range(self.ntimesteps):
            for block in range(self.startblock, self.startblock+self.nblocks):
        
                print "processing block:", block
        
                self.data.getFFTData(self.fftdata, block)
        
                print "reading done"
        
                self.t_image.fill(0.)
        
                print "beamforming started"
                cr.hBeamformImage(self.t_image, self.fftdata, self.frequencies, self.delays)
                print "beamforming done"
                
                cr.hAbsSquareAdd(self.image[step], self.t_image)
        
            self.startblock += self.nblocks

# Create a TaskLauncher
#imager = TaskLauncher(Imager)
 
