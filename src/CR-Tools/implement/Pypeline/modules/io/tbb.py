"""This module implements the primary interface for reading LOFAR TBB data.

It contains one function `open` that is used to open an HDF5 file containing LOFAR TBB data and returns a :class:`~TBBData` file object.

.. moduleauthor:: Pim Schellart <P.Schellart@astro.ru.nl>
"""

import numpy as np
import pycrtools as cr
from pycrtools import metadata as md

# This class implements the IO interface
from interfaces import IOInterface

class TBBData(IOInterface):
    """This class provides an interface to single file Transient Buffer
    Board data.
    """

    def __init__(self, filename, blocksize=1024):
        """Constructor.
        """

        # Store blocksize for readout
        self.__blocksize = blocksize

        # Open file
        self.__file = cr.TBBData(filename)

        # Get a list of antenna IDs for which data is stored in the file
        self.__dipoleNames= self.__file.dipoleNames()

        # How many dipoles are there
        self.__nofDipoleDatasets = self.__file.nofDipoleDatasets()

        # Select all antennas by default
        self.__selectedDipoles = self.__file.dipoleNames()

        # Find reference antenna
        self.__refAntenna = self.__file.alignment_reference_antenna()

        # Selection dependent initialization
        self.init_selection()

        # Mark file as opened
        self.closed = False

    def init_selection(self):
        """Selection dependent initialization.
        """
        
        # Align data
        self.__alignment_offset = cr.hArray(self.__file.alignment_offset(self.__refAntenna))

        # Get antenna set
        self.__antenna_set = self.__file.antenna_set()

        # Get Nyquist zone for each antenna
        self.__nyquist_zone = self.__file.nyquist_zone()

        # Get Sample frequency value and units for each antenna
        self.__sample_frequency_value = self.__file.sample_frequency_value()
        self.__sample_frequency_unit = self.__file.sample_frequency_unit()

        # Calculate frequencies for FFT
        self.__nfmin = None # Frequency range minimum index
        self.__nfmax = None # Frequency range maximum index
        self.__calculateFrequencies()

        # Generate scrach arrays
        self.__makeScratch()

    def __repr__(self):
        """Display summary when printed.
        """

        return self.__file.summary().strip()

    def keys(self):
        """Returns list of valid keywords.
        """

        return ["FILENAME", "ANTENNA_SET", "NYQUIST_ZONE", "TIME", "SAMPLE_NUMBER", "SAMPLE_FREQUENCY_VALUE", "SAMPLE_FREQUENCY_UNIT", "DATA_LENGTH", "NOF_STATION_GROUPS", "NOF_DIPOLE_DATASETS", "NOF_SELECTED_DATASETS", "DIPOLE_NAMES", "SELECTED_DIPOLES", "CHANNEL_ID", "GROUPTYPE", "FILEDATE", "FILETYPE", "TELESCOPE", "OBSERVER", "CLOCK_FREQUENCY", "CLOCK_FREQUENCY_UNIT", "FILTER_SELECTION", "TARGET", "SYSTEM_VERSION", "PIPELINE_NAME", "PIPELINE_VERSION", "NOTES", "PROJECT_ID", "PROJECT_TITLE", "PROJECT_PI", "PROJECT_CO_I", "PROJECT_CONTACT", "OBSERVATION_ID", "OBSERVATION_START_MJD", "OBSERVATION_START_TAI", "OBSERVATION_START_UTC", "OBSERVATION_END_MJD", "OBSERVATION_END_TAI", "OBSERVATION_END_UTC", "OBSERVATION_NOF_STATIONS", "OBSERVATION_STATION_LIST", "OBSERVATION_FREQUENCY_MAX", "OBSERVATION_FREQUENCY_MIN", "OBSERVATION_FREQUENCY_CENTER", "OBSERVATION_FREQUENCY_UNIT"]

    def __getitem__(self, key):
        """Implements keyword access.
        """

        if key not in self.keys():
            raise KeyError("Invalid keyword")

        elif key is "FILENAME":
            return self.__file.filename()
        elif key is "ANTENNA_SET":
            return self.__file.antenna_set()
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
        elif key is "NOF_STATION_GROUPS":
            return self.__file.nofStationGroups()
        elif key is "NOF_DIPOLE_DATASETS":
            return self.__file.nofDipoleDatasets()
        elif key is "NOF_SELECTED_DATASETS":
            return self.__file.nofSelectedDatasets()
        elif key is "DIPOLE_NAMES":
            return self.__file.dipoleNames()
        elif key is "SELECTED_DIPOLES":
            return self.__file.selectedDipoles()
        elif key is "CHANNEL_ID":
            return self.__file.channelID()
        elif key is "FILETYPE":
            return self.__file.filetype()
        elif key is "FILEDATE":
            return self.__file.filedate()
        elif key is "TELESCOPE":
            return self.__file.telescope()
        elif key is "OBSERVER":
            return self.__file.observer()
        elif key is "CLOCK_FREQUENCY":
            return self.__file.clockFrequency()
        elif key is "CLOCK_FREQUENCY_UNIT":
            return self.__file.clockFrequencyUnit()
        elif key is "FILTER_SELECTION":
            return self.__file.filterSelection()
        elif key is "TARGET":
            return self.__file.target()
        elif key is "SYSTEM_VERSION":
            return self.__file.systemVersion()
        elif key is "PIPELINE_NAME":
            return self.__file.pipelineName()
        elif key is "PIPELINE_VERSION":
            return self.__file.pipelineVersion()
        elif key is "NOTES":
            return self.__file.notes()
        elif key is "PROJECT_ID":
            return self.__file.projectID()
        elif key is "PROJECT_TITLE":
            return self.__file.projectTitle()
        elif key is "PROJECT_PI":
            return self.__file.projectPI()
        elif key is "PROJECT_CO_I":
            return self.__file.projectCoI()
        elif key is "PROJECT_CONTACT":
            return self.__file.projectContact()
        elif key is "OBSERVATION_ID":
            return self.__file.observationID()
        elif key is "OBSERVATION_START_MJD":
            return self.__file.startMJD()
        elif key is "OBSERVATION_START_TAI":
            return self.__file.startTAI()
        elif key is "OBSERVATION_START_UTC":
            return self.__file.startUTC()
        elif key is "OBSERVATION_END_MJD":
            return self.__file.endMJD()
        elif key is "OBSERVATION_END_TAI":
            return self.__file.endTAI()
        elif key is "OBSERVATION_END_UTC":
            return self.__file.endUTC()
        elif key is "OBSERVATION_NOF_STATIONS":
            return self.__file.nofStations()
        elif key is "OBSERVATION_STATION_LIST":
            return self.__file.stationList()
        elif key is "OBSERVATION_FREQUENCY_MIN":
            return self.__file.frequencyMin()
        elif key is "OBSERVATION_FREQUENCY_MAX":
            return self.__file.frequencyMax()
        elif key is "OBSERVATION_FREQUENCY_CENTER":
            return self.__file.frequencyCenter()
        elif key is "OBSERVATION_FREQUENCY_UNIT":
            return self.__file.frequencyUnit()

    def __setitem__(self, key, value):
        if key not in self.keys():
            raise KeyError("Invalid keyword")

        elif key is "SELECTED_DIPOLES":
            self.setAntennaSelection(value)
        else:
            raise KeyError(str(key) + " cannot be set")

    def __contains__(self, key):
        """Allows inquiry if key is implemented.
        """

        return key in self.keys()

    def __calculateFrequencies(self):
        """Calculate frequencies for FFT depending on sample frequency,
        nyquist zone and blocksize.
        """

        self.__frequencies = cr.hArray(float, self.__blocksize/2+1)

        # Calculate sample frequency in Hz
        sampleFrequency = self.__sample_frequency_value[0]

        if self.__sample_frequency_unit[0] == 'MHz':
            sampleFrequency *= 1e6
        elif self.__sample_frequency_unit[0] == 'GHz':
            sampleFrequency *= 1e9

        cr.hFFTFrequencies(self.__frequencies, sampleFrequency, self.__nyquist_zone[0])

    def __makeScratch(self):
        """Create scratch arrays.
        """

        # Create scratch array
        self.__scratch = cr.hArray(float, dimensions=(self.__file.nofSelectedDatasets(), self.__blocksize))
        self.__scratchFFT = cr.hArray(complex, dimensions=(self.__file.nofSelectedDatasets(), self.__blocksize / 2 + 1))

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
            self.__selectedDipoles = [self.__dipoleNames[i] for i in selection if i < self.__nofDipoleDatasets]

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

        # Selection dependent initialization
        self.init_selection()

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

        # For selected frequency range
        if self.__nfmin != None and self.__nfmax != None:
            # Perform FFT
            self.__scratchFFT[...].fftcasa(self.__scratch[...], self.__nyquist_zone[0])

            data[...].copy(self.__scratchFFT[..., self.__nfmin:self.__nfmax])

        else:
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

        return md.get("RelativeAntennaPositions", self.__selectedDipoles, self.__antenna_set, True)

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

        return md.get("AbsoluteAntennaPositions", self.__selectedDipoles, self.__antenna_set, True)

    def setFrequencyRangeByIndex(self, nfmin, nfmax):
        """Sets the frequency selection used in subsequent calls to
        `getFFTData`.
        If **frequencies** is the array of frequencies available for the
        selected blocksize, then subsequent calls to `getFFTData` will
        return data corresponding to frequencies[nfmin:nfmax].

        Required Arguments:

        ============= =================================================
        Parameter     Description
        ============= =================================================
        *nfmin*       minimum frequency as index into frequency array
        *nfmax*       maximum frequency as index into frequency array
        ============= =================================================

        Output:
        This method does not return anything.

        Raises:
        It raises an `IndexError` if frequency selection cannot be set
        to requested values (e.g. index out of range)

        """

        if not isinstance(nfmin, int) or not isinstance(nfmax, int):
            raise ValueError("Provided values for nfmin and nfmax are not of the correct type 'int'.")

        elif not 0 <= nfmin <= nfmax < len(self.__frequencies):
            raise ValueError("Invalid frequency range.")

        else:
            self.__nfmin = nfmin
            self.__nfmax = nfmax

    def getFrequencies(self):
        """Returns the frequencies that are appicable to the FFT data

        Arguments:
        None

        Return value:
        This method returns a FloatVector with the selected frequencies
        in Hz.

        """
        
        frequencies = self.__frequencies

        if self.__nfmin != None and self.__nfmax != None:
            frequencies=frequencies[range(self.__nfmin, self.__nfmax)]

        return frequencies

    def empty(self, key):
        """Return empty array for keyword data.
        """

        if key == "TIMESERIES_DATA":
            return cr.hArray(float, dimensions=(self.__file.nofSelectedDatasets(), self.__blocksize))
        elif key == "FFT_DATA":
            return cr.hArray(complex, dimensions=(self.__file.nofSelectedDatasets(), self.__blocksize / 2 + 1))
        else:
            raise KeyError("Unknown key: " + str(key))

    def read(self, key, data, *args, **kwargs):
        """Generic read function supporting keyword arguments.

        Required Arguments:

        ============= =================================================
        Parameter     Description
        ============= =================================================
        *key*         Data type to read, one of:
                      *TIMESERIES_DATA*
                      *FFT_DATA*
        *data*        array to write data to.
        ============= =================================================

        """

        if key == "TIMESERIES_DATA":
            return self.getTimeseriesData(data, *args, **kwargs)
        if key == "FFT_DATA":
            return self.getFFTData(data, *args, **kwargs)
        else:
            raise KeyError("Unknown key: " + str(key))

    def close(self):
        """Closes file and sets the data attribute `.closed` to
        True. A closed object can no longer be used for I/O operations.
        `close()` may be called multiple times without error.
        """

        # Call file object destructor which should close the file
        del self.__file

        self.closed = True

def open(filename, *args, **kwargs):
    """Open file with LOFAR TBB data.
    """

    return TBBData(filename, *args, **kwargs)

