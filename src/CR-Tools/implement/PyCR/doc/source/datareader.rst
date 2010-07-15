**************
Accessing data
**************

The DataReader object is used for access to HDF5 files via the CR-Tools DataReader class.

Normally created by calling dr = core.open(filename) the object *dr*
allows for access to the data via it's *read* method.

This class also provides a universal access mechanism to access metadata
belonging to the data file. 

Metadata is referenced by parameters or keys, a list of valid keys is
given by calling the *keys* method.
It is also possible to question the object if a key is valid using in::

    if 'parameter' in dr:
        print "Found"
    else:
        print "Not in file"

If a valid key is provided to the ``[]`` operator the corresponding
metadata is returned::

    nofantennas = dr['nofantennas']

Before the data is requested some parameters of the DataReader may
need to be set, this can be done using the (block, blocksize,
selectedantennas and shiftvector) attributes::

    # Set blocksize
    dr.blocksize = 512
    
    # Get blocksize
    blocksize = dr.blocksize

To actually get the data, the *read* method is called. Here one needs
to provide a numpy array of the correct size to store the data::

    fftlength = dr['fftlength']
    freqs = np.empty(fftlength)
    dr.read("frequency", freqs)

