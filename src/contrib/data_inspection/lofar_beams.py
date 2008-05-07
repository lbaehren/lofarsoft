# scripts to calculate antenna beam from sarod's analytical droopy dipole model.  uses ctypes to read shared-object library.

import ctypes, math
import coordinates_mod as co

path = '/app/usg_dal/lib/python/'

def lba(frequency,altitude,azimuth,normalized=True):
    """Calculates the LBA antenna beam.  Returns the E**2 value (normalized by value at zenith by default.
    Takes frequency in Hz, altitude and azimuth in degrees. """

# load shared-object library
    beam = ctypes.cdll.LoadLibrary(path + 'beam_dr.so')
    beam.test_double.restype = ctypes.c_double
    carray = ctypes.c_double * 4

# set parameters of droopy dipole:  height, length, alpha (w.r.t vertical), phi_0 (orientation relative to ha=0, i think).
    parameters = carray(1.706, 1.38*math.sin(math.pi/4), math.pi/4, 0.)

# calculate beam at zenith and at alt,az
    beam_ampl_zenith = beam.test_double(parameters, carray(0., frequency, math.pi/4, math.pi/2))
    beam_ampl = beam.test_double(parameters, carray(0., frequency, azimuth*math.pi/180., altitude*math.pi/180.))
    if normalized:
        return beam_ampl/beam_ampl_zenith
    else:
        return beam_ampl


def hba_phi(frequency,altitude,azimuth):
    """Calculates the HBA antenna beam phi term.  Returns the Ephi value.
    Takes frequency in Hz, altitude and azimuth in degrees. """

    print "Note:  This is broken at the moment, since importing the HBA beam model requires using complex numbers.  This option does not yet exist for the ctypes python package."

# load shared-object library
    beam = ctypes.cdll.LoadLibrary(path + 'hba_beam_phi.so')
    beam.test_complex.restype = ctypes.c_double
    carray = ctypes.c_double * 4

# set parameters of hba dipole:  height, length, alpha (w.r.t vertical), phi_0 (orientation relative to ha=0, i think).  all but the last are ignored for hba.
    parameters = carray(1.706, 1.38*math.sin(math.pi/4), math.pi/4, 0.)

# calculate beam at alt,az
    beam_phi = beam.test_complex(parameters, carray(0., frequency, azimuth*math.pi/180., altitude*math.pi/180.))
    return beam_phi


def hba_theta(frequency,altitude,azimuth):
    """Calculates the HBA antenna beam theta term.  Returns the Etheta value.
    Takes frequency in Hz, altitude and azimuth in degrees. """

    print "Note:  This is broken at the moment, since importing the HBA beam model requires using complex numbers.  This option does not yet exist for the ctypes python package."

# load shared-object library
    beam = ctypes.cdll.LoadLibrary(path + 'hba_beam_theta.so')
    beam.test_complex.restype = ctypes.c_double
    carray = ctypes.c_double * 4

# set parameters of hba dipole:  height, length, alpha (w.r.t vertical), phi_0 (orientation relative to ha=0, i think).  all but the last are ignored for hba.
    parameters = carray(1.706, 1.38*math.sin(math.pi/4), math.pi/4, 0.)

# calculate beam and at alt,az
    beam_theta = beam.test_complex(parameters, carray(0., frequency, azimuth*math.pi/180., altitude*math.pi/180.))
    return beam_theta


def hba(frequency,altitude,azimuth,normalized=True):
    """Calculates the HBA antenna beam.  Returns E**2, optionally normalized to zenith value.
    Takes frequency in Hz, alt and azi in degrees."""

    print "Note:  This is broken at the moment, since importing the HBA beam model requires using complex numbers.  This option does not yet exist for the ctypes python package."

    Ephi = hba_phi(frequency,altitude,azimuth)
    Etheta = hba_theta(frequency,altitude,azimuth)

    beam_ampl_zenith = math.sqrt(Ephi**2 + Etheta**2)  # ?
    beam_ampl = math.sqrt(Ephi**2 + Etheta**2)  # ?

    if normalized:
        return beam_ampl/beam_ampl_zenith
    else:
        return beam_ampl
