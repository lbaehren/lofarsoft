.. _fileio:


File I/O
========


Opening and closing a file
--------------------------

Let us see how we can open a file. First define some variable names
that represent the file names of the files we are going to use::

    >>> filename_lofar = LOFARSOFT+"/data/lofar/VHECR_example_short.h5"

We can create a new file object, using the :meth:`open` method ,
which is an interface to the LOFAR CRTOOLS datareader class.

The following will open a data file and return a :class:`DataReader`
object::

    >>> datafile = open(filename_lofar)

The associated filename can be retrieved with::

    >>> datafile["FILENAME"]

The file will be automatically closed (and the object will be
destroyed), whenever the :class:`open` object is deleted, e.g. with
``datafile = 0``.


Setting and retrieving metadata
-------------------------------

Now we need to access the metadata in the file.

This can be done by providing a keyword to the datafile object, e.g. type::

    >>> datafile["FILENAME"]

to obtain the filename of the datafile object. A list of valid
keywords can be obtained by::

    >>> datafile.keys()

Note, that the results are returned as PythonObjects. Hence, this
makes use of the power of Python with automatic typing. For example::

    >>> datafile["FREQUENCY_RANGE"]

actually returns a list of frequency ranges.

Just for convenience let us define a number of variables that contain
essential parameters (we will later actually use different ones which
are automatically stored in the datafile object)::

    >>> obsdate = datafile["TIME"][0]          # Timestamp of the first event
    >>> filesize  = datafile["DATA_LENGTH"][0] # number of samples per dipole
    >>> blocksize = datafile["BLOCKSIZE"]      # Number of samples per block
    >>> nAntennas = datafile["NOF_DIPOLE_DATASETS"] # Number of antennas
    >>> antennaIDs = datafile["DIPOLE_NAMES"]  # List of antenna IDs
    >>> selectedAntennas = datafile["SELECTED_DIPOLES"] # List of selected antennas
    >>> nofSelectedAntennas = datafile["NOF_SELECTED_DATASETS"] # Number of selected antennas
    >>> fftlength = datafile["FFTSIZE"] # Length of an FFT block
    >>> sampleFrequency = datafile["SAMPLE_FREQUENCY"][0] # Sample frequency
    >>> maxblocksize=min(filesize,1024*1024); # Maximum blocksize we will use
    >>> nBlocks = filesize/blocksize; # Number of blocks

    obsdate => 1310809509
    filesize => 204800
    blocksize => 1024
    nAntennas => 96
    antennaIDs => ['003000000', '003000001', '003000002', '003000003', '003000004', '003000005', '003000006', '003000007', '003001008', '003001009', '003001010', '003001011', '003001012', '003001013', '003001014', '003001015', '003002016', '003002017', '003002018', '0030020
    19', '003002020', '003002021', '003002022', '003002023', '003003024', '003003025', '003003026', '003003027', '003003028', '003003029', '003003030', '003003031', '003004032', '003004033', '003004034', '003004035', '003004036', '003004037', '003004038', '003004039', '00
    3005040', '003005041', '003005042', '003005043', '003005044', '003005045', '003005046', '003005047', '003006048', '003006049', '003006050', '003006051', '003006052', '003006053', '003006054', '003006055', '003007056', '003007057', '003007058', '003007059', '003007060'
    , '003007061', '003007062', '003007063', '003008064', '003008065', '003008066', '003008067', '003008068', '003008069', '003008070', '003008071', '003009072', '003009073', '003009074', '003009075', '003009076', '003009077', '003009078', '003009079', '003010080', '00301
    0081', '003010082', '003010083', '003010084', '003010085', '003010086', '003010087', '003011088', '003011089', '003011090', '003011091', '003011092', '003011093', '003011094', '003011095']
    selectedAntennas => ['003000000', '003000001', '003000002', '003000003', '003000004', '003000005', '003000006', '003000007', '003001008', '003001009', '003001010', '003001011', '003001012', '003001013', '003001014', '003001015', '003002016', '003002017', '003002018', '0
    03002019', '003002020', '003002021', '003002022', '003002023', '003003024', '003003025', '003003026', '003003027', '003003028', '003003029', '003003030', '003003031', '003004032', '003004033', '003004034', '003004035', '003004036', '003004037', '003004038', '003004039
    ', '003005040', '003005041', '003005042', '003005043', '003005044', '003005045', '003005046', '003005047', '003006048', '003006049', '003006050', '003006051', '003006052', '003006053', '003006054', '003006055', '003007056', '003007057', '003007058', '003007059', '0030
    07060', '003007061', '003007062', '003007063', '003008064', '003008065', '003008066', '003008067', '003008068', '003008069', '003008070', '003008071', '003009072', '003009073', '003009074', '003009075', '003009076', '003009077', '003009078', '003009079', '003010080',
    '003010081', '003010082', '003010083', '003010084', '003010085', '003010086', '003010087', '003011088', '003011089', '003011090', '003011091', '003011092', '003011093', '003011094', '003011095']
    nofSelectedAntennas => 96
    fftlength => 513
    sampleFrequency => 200000000.0
    maxblocksize => 204800
    nBlocks => 200

As you can see the date is expressed in a not well interpretable
format, i.e. the nr. of seconds after January 1st, 1970.  To get a
human readable version of the observing date use the python time
module::

    >>> import time
    >>> time.asctime(time.localtime(obsdate))
    'Thu Feb 11 23:09:14 2010'

Fortunately, you do not have to do this all the time, since all the
parameters will be read out at the beginning and will be stored as
attributes to the file object.

To set the data attributes you can simply use the same attribute
naming as mentioned above, e.g.::

    >>> datafile["BLOCKSIZE"] = 2048



Reading in data
---------------

The next step is to actually read in data. This is done with the
:meth:`read` method.

Before this is done, one has to allocate the memory in which the data
is put. Although this requires one to program carefully and understand
the data structure, this improves speed and efficiency.

Let's first create a :class:`FloatArray` of the correct dimensions,
naming it ``fxdata`` and setting the unit to counts::

    >>> fxdata = hArray(float,[nofSelectedAntennas,blocksize],name="E-Field")
    >>> fxdata.setUnit("","Counts")

This is now a large vector filled with zeros.

Now we can read in the raw timeseries data, either by using
:func:`datafile.read` and a keyword, or use the :func:`read` method of
arrays, e.g.::

    >>> datafile.read("TIMESERIES_DATA", fxdata)

or::

    >>> fxdata.read(datafile, "TIMESERIES_DATA")

The types of data that can be read are ``TIMESERIES_DATA``,
``FFT_DATA``, ``FREQUENCY_DATA``, and ``TIME_DATA``. You can also use
these keywords with the ``EMPTY_`` prefix, which creates an empty
hArray of the correct type and size.

.. [Example of calculating an average spectrum]

Below is an example that shows how to read in data to calculate an
average spectrum::

    >>> fftdata = datafile["EMPTY_FFT_DATA"]
    >>> avspectrum = hArray(float, dimensions=fftdata, name="Average spectrum")
    >>> for block in range(nBlocks):
    >>>     datafile["BLOCK"] = block
    >>>     fftdata.read(datafile, "FFT_DATA")
    >>>     hSpectralPower(avspectrum[...], fftdata[...])



