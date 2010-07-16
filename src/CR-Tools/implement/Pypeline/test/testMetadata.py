import metadata
import os
import doctest

# General settings
LOFARSOFT=os.environ["LOFARSOFT"].rstrip('/')+'/'

def getStationNames():
    """
    Get a list of station names.

    Tests:

    >>> len(getStationNames()) > 0
    True

    >>> "CS001" in getStationNames()
    True

    >>> "CS008" in getStationNames()
    False

    >>> "DE602" in getStationNames()
    True
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

    >>> getStationPosition("CS001", "HBA")
    array([  6.86795028,  52.91120734,  50.156     ])

    >>> getStationPosition("CS001", "LBA_INNER")
    array([  6.86763496,  52.91139796,  50.156     ])
    """
    position = metadata.getStationPositions(station, antennatype)
    return position

def test(antennatype="LBA_INNER"):
    """
    Test
    """
    stationnames = getStationNames()

    antennatypes = ["HBA", "LBA_INNER", "LBA_OUTER"]
    for stationname in stationnames:
        for antennatype in antennatypes:
            position = getStationPosition(stationname, antennatype)
            print "%.8s, %-10s: %s" %(stationname, antennatype, position)


if __name__ == '__main__':
    doctest.testmod()
    test()
