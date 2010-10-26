"""Imager documentation
"""

import pdb
import pycrtools as cr
from pycrtools.interfaces import IO
from pytmf import deg2rad, rad2deg

def casaRefcodes(**kwargs):
    """Get CASA reference code and projection from standard WCS parameter
    parsing rules.
    """

    # Validity checks
    if 'CTYPE1' not in kwargs:
        raise ValueError("CTYPE1 keyword not found.")
    if 'CTYPE2' not in kwargs:
        raise ValueError("CTYPE2 keyword not found.")
    if len(kwargs['CTYPE1']) != 8:
        raise ValueError("CTYPE1 keyword has invalid length")
    if len(kwargs['CTYPE2']) != 8:
        raise ValueError("CTYPE2 keyword has invalid length")
    if kwargs['CTYPE1'][5:8] != kwargs['CTYPE2'][5:8]:
        raise ValueError("Projection for first and second axis do not match.")

    # Check for different coordinate types
    CTYPE1=kwargs['CTYPE1']
    CTYPE2=kwargs['CTYPE2']

    if CTYPE1[0:4] == 'RA--' and CTYPE2[0:4] == 'DEC-':
        # Coordinate type depends on equinox or epoch
        if 'EQUINOX' in kwargs:
            refcode = kwargs['EQUINOX']
        elif 'EPOCH' in kwargs:
            refcode = kwargs['EPOCH']
        else:
            refcode = 'J2000'
    elif CTYPE1[0:4] == 'ALON' and CTYPE2[0:4] == 'ALAT':
        refcode = 'AZEL'
    elif CTYPE1[0:4] == 'ELON' and CTYPE2[0:4] == 'ELAT':
        refcode = 'ECLIPTIC'
    elif CTYPE1[0:4] == 'GLON' and CTYPE2[0:4] == 'GLAT':
        refcode = 'GALACTIC'
    elif CTYPE1[0:4] == 'SLON' and CTYPE2[0:4] == 'SLAT':
        refcode = 'SUPERGAL'
    else:
        raise ValueError("Unknown coordinate system "+CTYPE2[0:4])

    # Check for projection
    if CTYPE1[5:8] in ['AZP', 'SZP', 'TAN', 'SIN', 'STG', 'ARC', 'ZPN', 'ZEA', 'AIR', 'CYP', 'CAR', 'MER', 'CEA', 'COP', 'COD', 'COE', 'COO', 'BON', 'PCO', 'SFL', 'PAR', 'AIT', 'MOL', 'CSC', 'QSC', 'TSC', 'HPX']:
        projection = CTYPE1[5:8]
    else:
        raise ValueError("Invalid projection "+CTYPE1[5:8])

    return refcode, projection

class CoordinateGrid(object):
    """Coordinate grid for images.

    Has the following attributes (where N is the number of pixels):

    *pixel* array of length N with pixel coordinates
            (e.g. 1, 1, 2, 2, ..., naxis1, naxis2)
    *world* array of length N with corresponding world coordinates
            (e.g. J2000, AzEl) in radians
    *azel* array of length N * 3 with corresponding azimuth, elevation,
            distance coordinates in radians
    *cartesian* array of length N * 3 with corresponding local Cartesian
                coordinates in meters
    """

    def __init__(self, **kwargs):
        """Creates a coordinate grid according to specified WCS parameters
        given as keyword arguments or as dictionary (using **imparam where
        imparam is a dictionary of image parameters, e.g. a FITS header).
        
        *NAXIS1* number of pixels allong first image axis
        *NAXIS2* number of pixels allong second image axis
        *CUNIT1* unit for 1st axis increment (rad/deg)
        *CUNIT2* unit for 2nd axis increment (rad/deg)
        *CRVAL1* world value corresponding to reference pixel on 1st axis
        *CRVAL1* world value corresponding to reference pixel on 2nd axis
        *CDELT1* increment (in intermediate world coordinates) for 1st axis
        *CDELT2* increment (in intermediate world coordinates) for 2nd axis
        *CTYPE1* coordinate system and projection accordint to FITS rules
        *CTYPE2* coordinate system and projection accordint to FITS rules
        *LONPOLE* longitude of pole
        *LATPOLE* latitude of pole
        """

        # Get image parameters
        self.naxis1=kwargs['NAXIS1']
        self.naxis2=kwargs['NAXIS2']
        self.crval1=kwargs['CRVAL1']
        self.crval2=kwargs['CRVAL2']
        self.cdelt1=kwargs['CDELT1']
        self.cdelt2=kwargs['CDELT2']
        self.crpix1=kwargs['CRPIX1']
        self.crpix2=kwargs['CRPIX2']
        self.lonpole=kwargs['LONPOLE']
        self.latpole=kwargs['LATPOLE']

        self.refcode, self.projection=casaRefcodes(**kwargs)

        print "Coordinate system:", self.refcode
        print "Projection:", self.projection

        # Check units of input
        if kwargs['CUNIT1'] != kwargs['CUNIT2']:
            raise ValueError("Units of axis not equal.")

        # Convert to radians if nessesary
        if kwargs['CUNIT1'] == 'deg':
            self.crval1=deg2rad(self.crval1)
            self.crval2=deg2rad(self.crval2)
            self.cdelt1=deg2rad(self.cdelt1)
            self.cdelt2=deg2rad(self.cdelt2)
            self.lonpole=deg2rad(self.lonpole)
            self.latpole=deg2rad(self.latpole)

        # Total number of pixels
        self.npix=self.naxis1*self.naxis2

        # Generate pixel grid for image
        self.pixel=[]
        N=0
        for i in range(1, self.naxis1+1):
            for j in range(1, self.naxis2+1):
                self.pixel.extend([float(i),float(j)])
        
        self.pixel=cr.hArray(self.pixel)
        self.world=self.pixel.new()
        
        # Generate world coordinates for image grid
        print "Calculating world coordinates"
        cr.hPixel2World(self.world, self.pixel, self.refcode,
            self.projection, self.crval1, self.crval2, self.cdelt1,
            self.cdelt2, self.crpix1, self.crpix2, self.lonpole,
            self.latpole)
        
        # Generate Cartesian coordinates for beamforming
        print "Calculating AzEl coordinates"
        self.getAzEl()

        print "Calculating Cartesian coordinates"
        self.cartesian=self.azel.new()
        
        cr.hCoordinateConvert(self.azel[...], cr.CoordinateTypes.AzElRadius,
            self.cartesian[...], cr.CoordinateTypes.Cartesian, False)

    def getAzEl(self):
        """Get Azimuth/Elevation/Distance coordinates corresponding to world
        coordinates.
        """

        self.azel=cr.hArray(float, dimensions=[self.npix,3])

        # Perform conversion for different coordinate systems
        # specifed by refcode
        if self.refcode == 'AZEL':
            self.world.reshape((self.npix,2))
#            self.azel=cr.hArray(float, dimensions=[self.npix,3])
            self.azel[...,0:2]=self.world[...]
            self.azel[...,2:3]=1.
#            pdb.set_trace()

#            # World coordinates are already in AzEl, just add distance
#            for i in range(self.npix):
#                self.azel[i,0]=self.world[2*i] # Azimuth
#                self.azel[i,1]=self.world[2*i+1] # Elevation
#                self.azel[i,2]=1. # Distance
        else:
            raise ValueError("Conversion of world coordinates to AzEl not yet implemented for "+self.refcode+" coordinate system")

class Imager(object):
    """Imager for LOFAR TBB data.
    """
    def __init__(self, **imparam):
        """Constructor.
        """
        
        # Generate grid
        self.grid = CoordinateGrid(**imparam)

    def addImage(self, image, io, startblock, nblocks, antennae):
        """Beamform fftdata and add to image.
        
        *image* 3d array of dimensions  x-pixel, y-pixel, frequency
        *fftdata* 2d array with fft data of dimensions antenna, frequency
        """

        # Check input
        if not isinstance(io, IO):
            raise ValueError("Parameter *io* does is not of the correct interface type *IO*")

        # Select antennae
        io.setAntennaSelection(antennae)

        # Get antenna positions
        antpos = io.getAntennaPosition()

        for block in range(startblock, startblock+nblocks):

            print "processing block:", block

            # Get FFT data for current block
            fftdata = io.getFFTData(block)

            # Call beamformer
            cr.hBeamformImage(image, fftdata, frequencies, antpos, self.grid.cartesian)

        return image

