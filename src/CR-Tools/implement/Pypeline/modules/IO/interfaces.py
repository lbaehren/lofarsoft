class IOInterface(object):
    """Base class for data IO.

    All functions or methods that use IO should expect to interface with
    objects of this type and all classes that implement IO should derive
    from this base class and overload it's functions.
    """

    def __init__(self):
        pass

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
        raise NotImplementedError("You are attempting to call a method that has not been implemented for this subclass of the IO interface.")

    def setFrequencySelection(self, frequencies):
        """Sets the frequency selection used in subsequent calls to
        `getFFTData`.

        Required Arguments:

        ============= =================================================
        Parameter     Description
        ============= =================================================
        *frequencies* :class:`~pycrtools.FloatVector` of frequencies
                      in Hz
        ============= =================================================

        Output:
        This method does not return anything.

        Raises:
        It raises a `ValueError` if frequency selection
        cannot be set to requested values (e.g. specified frequency is not
        available for this blocksize.)

        """
        raise NotImplementedError("You are attempting to call a method that has not been implemented for this subclass of the IO interface.")

    def setFrequencyRange(self, fmin, fmax):
        """Sets the frequency selection used in subsequent calls to
        `getFFTData`. The selection set is all frequencies available
        for the set blocksize in the range [fmin,fmax].

        Required Arguments:

        ============= =================================================
        Parameter     Description
        ============= =================================================
        *fmin*        minimum frequency in Hz
        *fmax*        maximum frequency in Hz
        ============= =================================================

        Output:
        This method does not return anything.

        Raises:
        It raises a `ValueError` if frequency selection
        cannot be set to requested values (e.g. all frequencies out of
        range)

        """
        raise NotImplementedError("You are attempting to call a method that has not been implemented for this subclass of the IO interface.")

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
        raise NotImplementedError("You are attempting to call a method that has not been implemented for this subclass of the IO interface.")

    def getFrequencies(self):
        """Returns the frequencies that are applicable to the FFT data

        Output:
        This method returns a FloatVector with the selected frequencies
        in Hz.
        """
        raise NotImplementedError("You are attempting to call a method that has not been implemented for this subclass of the IO interface.")

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
        raise NotImplementedError

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
        raise NotImplementedError

    def getFFTData(self, data, block):
        """Writes FFT data for selected antennas to data array.

        Required Arguments:

        ============= =================================================
        Parameter     Description
        ============= =================================================
        *data*        data array to write FFT data to.
        *block*       index of block to return data from.
        ============= =================================================

        Output:
        a two dimensional array containing the FFT data of the
        specified block for each of the selected antennae and
        for the selected frequencies.
        So that if `a` is the returned array `a[i]` is an array of
        length (number of frequencies) of antenna i.

        """
        raise NotImplementedError("You are attempting to call a method that has not been implemented for this subclass of the IO interface.")

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
        raise NotImplementedError("You are attempting to call a method that has not been implemented for this subclass of the IO interface.")

    def getReferencePosition(self):
        """Returns reference position used for antenna position
        coordinate system.

        Output:
        a FloatVector with (lon,lat,height) in (rad,rad,m) of the WGS84
        position of the center used for the antenna position coordinate
        system.
        """
        raise NotImplementedError("You are attempting to call a method that has not been implemented for this subclass of the IO interface.")

    def read(self, key, data, *args, **kwargs):
        """Generic read function supporting keyword arguments.

        Required Arguments:

        ============= =================================================
        Parameter     Description
        ============= =================================================
        *key*         Data type to read.
        *data*        array to write data to.
        ============= =================================================

        """
        raise NotImplementedError("You are attempting to call a method that has not been implemented for this subclass of the IO interface.")

