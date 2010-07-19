import os
import metadata
import doctest

# General settings
LOFARSOFT=os.environ["LOFARSOFT"].rstrip('/')+'/'

def getStationNames():
    """
    Get a list of station names.

    Tests:

    There should be at least one station available.
    >>> len(getStationNames()) > 0
    True

    Station CS001 is an existing station
    >>> "CS001" in getStationNames()
    True

    Station CS008 does not exist
    >>> "CS008" in getStationNames()
    False

    A remote station in the Netherlands
    >>> "RS205" in getStationNames()
    True

    A German station
    >>> "DE602" in getStationNames()
    True

    A (non-existing) station in the UK
    >>> "UK000" in getStationNames()
    False

    A (non-existing) French station
    >>> "FR000" in getStationNames()
    False
"""
    stationnames = []
    for filename in os.listdir(LOFARSOFT+"/data/calibration/AntennaArrays/"):
        if len(filename)>5 and filename[5]=="-":
            stationnames.append(filename[0:5])
    return stationnames

def getStationPosition(station, antennatype="LBA"):
    """
    Get the lon, lat and height of a station for a specific antennatype

    Example (and doctest)

    Position of the HBA
    >>> getStationPosition("CS001", "HBA")
    array([  6.86795028,  52.91120734,  50.156     ])

    Position of the LBA_INNER
    >>> getStationPosition("CS001", "LBA_INNER")
    array([  6.86763496,  52.91139796,  50.156     ])

    Position of the LBA_OUTER
    >>> getStationPosition("CS001", "LBA_OUTER")
    array([  6.86763496,  52.91139796,  50.156     ])
"""
    position = metadata.getStationPositions(station, antennatype)
    return position


def test():
    """
    Process various tests.
    """
    testAntennaPositions()

def testAntennaPositions():
    """
    Test
    """
    stationnames = getStationNames()

    antennatypes = ["LBA_INNER", "LBA_OUTER", "LBA_X", "LBA_Y", "HBA", "HBA_0", "HBA_1"]
    for stationname in stationnames:
        print "---[ %5s ]------------------------------------------------------------" %(stationname)
        for antennatype in antennatypes:
            position = getStationPosition(stationname, antennatype)
            print "   %-10s: %s" %(antennatype, position)

if __name__ == '__main__':
    if (doctest.testmod().failed==0):
        test()
