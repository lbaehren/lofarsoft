"""
A number of tools useful in calibrating radio data
"""

import math
import pycrtools as cr

deg = math.pi / 180.
pi2 = math.pi / 2.


def makeAZELRDictGrid(AZ, EL, Distance, nx=3, ny=3, offset=5 * deg):
    """
    Make a list containing direction dicts that represent a grid of
    ``nx * ny`` pointings around a central value specified by azimuth,
    elevation (in radians), and distance (in meter). ``offset`` is the
    offset between the pointings in radians (default: 5*deg).

    """
    return [dict(az=AZ + (x - nx / 2) * offset, el=EL + (y - ny / 2) * offset, r=Distance) for y in range(ny) for x in range(nx)]


def TimeBeamIncoherent(timeseries_data, incoherent_sum=None):
    """
    **Usage:**

    ``TimeBeamIncoherent(timeseries_data[nAntennas,blocklen],incoherent_sum=None) -> incoherent_sum.squareadd(timeseries_data)``

    returns ``incoherent_sum[blocklen]``

    **Description:**

    Calculate the incoherent sum of the timeseries data of multiple
    antennas. Will square the E-Field and add all antennas into one
    time series, thus containing the 'incoherent power'.

    **Example:**
    ::

        file = cr.open('/Users/falcke/LOFAR/usg/data/lofar/oneshot_level4_CS017_19okt_no-9.h5') #
        file['BLOCKSIZE'] = 2**int(round(log(file['DATA_LENGTH'][0],2)))
        file['SELECTED_DIPOLES']=[f for f in file['DIPOLE_NAMES'] if int(f)%2==1] # select uneven antenna IDs
        timeseries_data = TimeBeamIncoherent(file['TIMESERIES_DATA'])
        timeseries_data.plot()

    """
    dims = timeseries_data.shape()
    if not incoherent_sum:
        incoherent_sum = cr.hArray(float, [dims[-1]], fill=0.0)
    incoherent_sum.squareadd(timeseries_data)
    return incoherent_sum
