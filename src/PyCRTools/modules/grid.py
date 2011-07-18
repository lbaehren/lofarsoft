"""Module for creating and working with coordinate grids.

.. moduleauthor:: Pim Schellart <P.Schellart@astro.ru.nl>
"""

from datetime import datetime
import pycrtools as cr
from pytmf import deg2rad, rad2deg, gregoriandate2jd
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
    *j2000* array of length N with corresponding J2000 coordinates in radians
            (equal to world if grid type is J2000)
    *azel* array of length N * 3 with corresponding azimuth, elevation,
            distance coordinates in radians
    *cartesian* array of length N * 3 with corresponding local Cartesian
                coordinates in meters
    """

    def __init__(self, obstime=None, ut1_utc=0., L=None, phi=None, **kwargs):
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
        *CTYPE1* coordinate system and projection according to FITS rules
        *CTYPE2* coordinate system and projection according to FITS rules
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

            print "CRVAL1", self.crval1
            print "CRVAL2", self.crval2

        self.obstime = obstime
        self.ut1_utc = ut1_utc
        self.L = L
        self.phi = phi

        # If conversion from J2000 to AZEL is required check if all
        # required parameters are provided
        if self.refcode != 'AZEL' and None in [obstime, L, phi]:
            raise ValueError("Conversion from J2000 to AZEL required but do not have telescope position and observation time.")
        elif self.refcode != 'AZEL' and not isinstance(obstime, datetime):
            # Assume obstime is UNIX timestamp and convert to datetime object
            self.obstime = datetime.utcfromtimestamp(obstime)

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

        # Generate (intermediate) J2000 coordinates
        print "Calculating J2000 coordinates"
        self.__calculateJ2000()

        # Generate Cartesian coordinates for beamforming
        print "Calculating AzEl coordinates"
        self.__calculateAzEl()

        print "Calculating Cartesian coordinates"
        self.cartesian=self.azel.new()

        cr.hCoordinateConvert(self.azel[...], cr.CoordinateTypes.AzElRadius,
            self.cartesian[...], cr.CoordinateTypes.Cartesian, False)

    def __calculateJ2000(self):
        """Get corresponding J2000 coordinates.
        """
        if self.refcode == 'J2000':
            self.j2000 = self.world
        elif self.refcode == 'AZEL':
            self.j2000 = None
        else:
            raise ValueError("Conversion of world coordinates to J2000 not yet implemented for "+self.refcode+" coordinate system")

    def __calculateAzEl(self):
        """Get Azimuth/Elevation/Distance coordinates corresponding to world
        coordinates.
        """

        self.azel=cr.hArray(float, dimensions=[self.npix,3])

        # Perform conversion for different coordinate systems
        # specifed by refcode
        if self.refcode == 'AZEL':
            self.world.reshape((self.npix,2))
            self.azel[...,0:2]=self.world[...]
            self.azel[...,2:3]=1.
        else:
            # Convert from J2000 to AZEL
            temp = self.j2000.new()

            # Get time in Julian days
            Y = self.obstime.year
            M = self.obstime.month
            D = float(self.obstime.day)
            h = float(self.obstime.hour)
            m = float(self.obstime.minute)
            s = float(self.obstime.second) + float(self.obstime.microsecond) * 1.e-6

            self.utc = gregoriandate2jd(Y, M, D + ((h + m / 60. + s / 3600.) / 24.))

            cr.hEquatorial2Horizontal(temp, self.j2000, self.utc, self.ut1_utc, self.L, self.phi)

            temp.reshape((self.npix,2))
            self.azel[...,0:2]=temp[...]
            self.azel[...,2:3]=1.

    def __repr__(self):
        """Return string representation.
        """

        s = "Grid ("
        s += "REFCODE " + str(self.refcode)
        s += "PROJECTION " + str(self.projection) + " "
        s += "NAXIS1 " + str(self.naxis1) + " "
        s += "NAXIS2 " + str(self.naxis2) + " "
        s += "CRVAL1 " + str(self.crval1) + " "
        s += "CRVAL2 " + str(self.crval2) + " "
        s += "CDELT1 " + str(self.cdelt1) + " "
        s += "CDELT2 " + str(self.cdelt2) + " "
        s += "CRPIX1 " + str(self.crpix1) + " "
        s += "CRPIX2 " + str(self.crpix2) + " "
        s += "LONPOLE " + str(self.lonpole) + " "
        s += "LATPOLE " + str(self.latpole) + " "
        s += "OBSTIME " + str(self.obstime) + " "
        s += "UT1_UTC " + str(self.obstime) + " "
        s += "L " + str(self.obstime) + " "
        s += "PHI " + str(self.obstime)
        s += ")"

        return s

