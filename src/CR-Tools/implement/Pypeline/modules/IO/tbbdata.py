"""Module documentation.
"""

import pycrtools as cr
from pycrtools import metadata as md

# This class implements the IO interface
from interfaces import IOInterface

class TBBData(IOInterface):
    """This class provides an interface to single file Transient Buffer
    Board data.
    """

    def __init__(self, filename, blocksize):
        """Constructor.
        """

        # Store blocksize for readout
        self.__blocksize = blocksize

        # Open file
        self.__file = cr.TBBData(filename)

        # Get a list of antenna IDs for which data is stored in the file
        self.__dipoleNames= self.__file.dipoleNames()

        # How many dipoles are there
        self.__nofDipoles = len(self.__dipoleNames)

        # Select all antennas by default
        self.__selectedDipoles = self.__dipoleNames

        # Align data
        self.__alignment_offset = cr.hArray(self.__file.alignment_offset())

        # Get antenna set
        self.__antennaSet = self.__file.antenna_set()

        # Get Nyquist zone for each antenna
        self.__nyquist_zone = self.__file.nyquist_zone()

        # Generate scrach arrays
        self.__makeScratch()

        # Mark file as opened
        self.closed = False

    def __repr__(self):
        """Display summary when printed.
        """

        return self.__file.summary().strip()

    def keys(self):
        """Returns list of valid keywords.
        """

        return ["FILENAME", "ANTENNA_SET", "NYQUIST_ZONE", "TIME", "SAMPLE_NUMBER", "SAMPLE_FREQUENCY_VALUE", "SAMPLE_FREQUENCY_UNIT", "DATA_LENGTH"]

    def __getitem__(self, key):
        """Implements keyword access.
        """

        if key not in self.keys():
            raise KeyError("Invalid keyword")

        elif key is "FILENAME":
            return self.__file.filename()
        elif key is "ANTENNA_SET":
            return self.__antennaSet
        elif key is "NYQUIST_ZONE":
            return self.__file.nyquist_zone()
        elif key is "TIME":
            return self.__file.time()
        elif key is "SAMPLE_NUMBER":
            return self.__file.sample_number()
        elif key is "SAMPLE_FREQUENCY_VALUE":
            return self.__file.sample_frequency_value()
        elif key is "SAMPLE_FREQUENCY_UNIT":
            return self.__file.sample_frequency_unit()
        elif key is "DATA_LENGTH":
            return self.__file.data_length()

    def __contains__(self, key):
        """Allows inquiry if key is implemented.
        """

        return key in self.keys()

    def __makeScratch(self):
        """Create scratch arrays.
        """

        # Create scratch array
        self.__scratch = cr.hArray(float, dimensions=(self.__file.nofSelectedDatasets(), self.__blocksize))

    def setAntennaSelection(self, selection):
        """Sets the antenna selection used in subsequent calls to
        `getAntennaPositions`, `getFFTData`, `getTimeseriesData`.

        Required Arguments:

        =========== =================================================
        Parameter   Description
        =========== =================================================
        *selection* Either Python list with index of the antenna as
                    known to self (integers (e.g. ``[1, 5, 6]``))
                    Or list of IDs to specify a LOFAR dipole
                    (e.g. ``['142000005', '3001008']``)
        =========== =================================================

        Output:
        This method does not return anything.

        Raises:
        It raises a `ValueError` if antenna selection cannot be set
        to requested value (e.g. specified antenna not in file).

        """
        if not isinstance(selection, list):
            raise ValueError("Selection needs to be a list.")

        if not len(selection)>0:
            raise ValueError("No antennas selected.")

        if isinstance(selection[0], int):
            # Selection by antenna number
            self.__selectedDipoles = [self.__dipoleNames[i] for i in selection if i < self.__nofDipoles]

        elif isinstance(selection[0], str):
            # Selection by antenna ID
            self.__selectedDipoles = [antennaID for antennaID in selection if antennaID in self.__dipoleNames]

        else:
            raise ValueError("No antenna ID or number found.")

        # Apply selection
        self.__file.selectDipoles(self.__selectedDipoles)

        # Check if selection was succesful
        if self.__file.nofSelectedDatasets() != len(selection):
            raise Exception("Not all antennas in selection are in file.")

        # Generate scrach arrays
        self.__makeScratch()

    def getTimeseriesData(self, data, block):
        """Returns timeseries data for selected antennas.

        Required Arguments:

        ============= =================================================
        Parameter     Description
        ============= =================================================
        *data*        data array to write timeseries data to.
        *block*       index of block to return data from.
        ============= =================================================

        Output:
        a two dimensional array containing the timeseries data of the
        specified block for each of the selected antennae.
        So that if `a` is the returned array `a[i]` is an array of
        length blocksize of antenna i.

        """

        cr.hReadTimeseriesData(data, self.__alignment_offset+block*self.__blocksize, self.__blocksize, self.__file)

    def getFFTData(self, data, block, hanning=True):
        """Writes FFT data for selected antennas to data array.

        Required Arguments:

        ============= =================================================
        Parameter     Description
        ============= =================================================
        *data*        data array to write FFT data to.
        *block*       index of block to return data from.
        *hanning*     apply Hannnig filter to timeseries data before
                      the FFT.
        ============= =================================================

        Output:
        a two dimensional array containing the FFT data of the
        specified block for each of the selected antennae and
        for the selected frequencies.
        So that if `a` is the returned array `a[i]` is an array of
        length (number of frequencies) of antenna i.

        """

        # Get timeseries data
        self.getTimeseriesData(self.__scratch, block)

        # Apply Hanning filter
        if hanning:
            self.__scratch[...].applyhanningfilter()

        # Perform FFT
        data[...].fftcasa(self.__scratch[...], self.__nyquist_zone[0])

    def getRelativeAntennaPositions(self):
        """Returns relative antenna positions for selected antennas, or all
        antennas if no selection was applied.

        Output:
        a two dimensional array containing the Cartesian position of
        each antenna in meters in local coordinates from a predefined
        center.
        So that if `a` is the returned array `a[i]` is an array of
        length 3 with positions (x,y,z) of antenna i.
        """

        return md.get("RelativeAntennaPositions", self.__selectedDipoles, self.__antennaSet, True)

    def getITRFAntennaPositions(self):
        """Returns antenna positions for selected antennas, or all
        antennas if no selection was applied.

        Output:
        a two dimensional array containing the Cartesian position of
        each antenna in meters in local coordinates from a predefined
        center.
        So that if `a` is the returned array `a[i]` is an array of
        length 3 with positions (x,y,z) of antenna i.
        """

        return md.get("AbsoluteAntennaPositions", self.__selectedDipoles, self.__antennaSet, True)

    def close(self):
        """Closes file and sets the data attribute `.closed` to
        True. A closed object can no longer be used for I/O operations.
        `close()` may be called multiple times without error.
        """

        # Call file object destructor which should close the file
        del self.__file

        self.closed = True

def open(filename, blocksize=1024):
    """Open file containing Transient Buffer Board data.
    """

    return TBBData(filename, blocksize)

