"""Base class for data IO.
"""

class IOBase(object):
    """Base class for data IO.
    """

    def __init__(self):
        pass

    def setAntennaSelection(self, antennaIDs):
        """Sets the antenna selection used in subsequent calls to
        `getAntennaPositions`, `getFFTData`, `getTimeseriesData`.

        Arguments:
        *antennaIDs* Either Python list with index of the antenna as
                     known to self (integers (e.g. [1, 5, 6]))
                     Or list of IDs to specify a LOFAR dipole
                     (e.g. ['142000005', '3001008'])

        Return value:
        This method does not return anything.
        It raises a ValueError if antenna selection cannot be set
        to requested value (e.g. specified antenna not in file).
        """
        pass

    def setFrequencySelection(self, frequencies):
        """Sets the frequency selection used in subsequent calls to
        `getFFTData`.

        Arguments:
        *frequencies* FloatVector of frequencies in Hz
        
        Return value:
        This method does not return anything.
        It raises a ValueError if frequency selection cannot be set
        to requested values (e.g. specified frequency is not
        available for this blocksize.)
        """
        pass
    
    def setFrequenceRange(self, fmin, fmax):
        """Sets the frequency selection used in subsequent calls to
        `getFFTData`. The selection set is all frequencies available
        for the set blocksize in the range [fmin,fmax].
        
        Arguments:
        *fmin* minimum frequency in Hz
        *fmax* maximum frequency in Hz
        
        Return value:
        This method does not return anything.
        It raises a ValueError if frequency selection cannot be set
        to requested values (e.g. all frequencies out of range)
        """
        pass

    def getFrequencies(self):
        """Returns the frequencies that are appicable to the FFT data

        Arguments:
        None

        Return value:
        This method returns a FloatVector with the selected frequencies
        in Hz.
        """
        pass

    def getAntennaPositions(self):
        """Returns antenna positions for selected antennas, or all
        antennas if no selection was applied.

        Arguments:
        None

        Return value:
        a two dimensional array containing the Cartesian position of
        each antenna in meters in local coordinates from a predefined
        center.
        So that if `a` is the returned array `a[i]` is an array of
        length 3 with positions (x,y,z) of antenna i.
        """
        pass

    def getFFTData(self, block):
        """Returns FFT data for selected antennas.

        Arguments:
        *block* index of block to return data from.

        Return value:
        a two dimensional array containing the FFT data of the
        specified block for each of the selected antennae and
        for the selected frequencies.
        So that if `a` is the returned array `a[i]` is an array of
        length (number of frequencies) of antenna i.
        """
        pass

    def getTimeseriesData(self, block):
        """Returns timeseries data for selected antennas.

        Arguments:
        *block* index of block to return data from.

        Return value:
        a two dimensional array containing the timeseries data of the
        specified block for each of the selected antennae.
        So that if `a` is the returned array `a[i]` is an array of
        length blocksize of antenna i.
        """
        pass

    def getReferencePosition(self):
        """Returns reference position used for antenna position 
        coordinate system.

        Arguments:
        None

        Return value:
        a FloatVector with (lon,lat,height) in (rad,rad,m) of the WGS84 
        position of the center used for the antenna position coordinate
        system. 
        """
        pass

