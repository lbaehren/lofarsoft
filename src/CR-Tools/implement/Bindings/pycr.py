"""CR-Tools from Python
"""

import warnings

# Import everything from the C++ module
from _pycr import *

# Try to import everything from the TTL
try:
    import ttl
    from ttl import *
except ImportError:
    warnings.warn('Could not import ttl module, algorithms from ttl will not be available.')

## -------------------------------------------------------------------------
#  Datareader

#  Set up metaclass for adding functionality to the DataReader class

BoostPythonMetaclass = DataReader.__class__

class __injector(object):
    class __metaclass__(BoostPythonMetaclass):
        def __init__(self, name, bases, dict):
            for b in bases:
                if type(b) not in (self, type):
                    for k,v in dict.items():
                        setattr(b,k,v)
            return type.__init__(self, name, bases, dict)

# Inject methods into the DataReader class
class __extended_DataReader(__injector, DataReader):
    """Object used for access to HDF5 files via the CR-Tools DataReader
    class.

    Normally created by calling f = pycr.open(filename) the object 'f'
    allows for access to the data via it's `read` method.

    This class also provides a universal access mechanism to access metadata
    belonging to the data file. 

    Metadata is referenced by parameters or keys, a list of valid keys is
    given by calling the `keys` method.
    It is also possible to question the object if a key is valid using in:

    if 'parameter' in f:
        print "Found"
    else:
        print "Not in file"

    If a valid key is provided to the [] operator the corresponding
    metadata is returned:

    nofantennas = f['nofantennas']

    Before the data is requested some parameters of the DataReader may
    need to be set, this can be done using the (block, blocksize,
    selectedantennas and shiftvector) attributes:

    # Set blocksize
    f.blocksize = 512

    # Get blocksize
    blocksize = f.blocksize

    To actually get the data, the `read` method is called. Here one needs
    to provide a numpy array of the correct size to store the data.

    fftlength = f['fftlength']
    freqs = np.empty(fftlength)
    f.read("frequency", freqs)

    """

    __antennas = None
    __keys = None

    def keys(self):
        """Returns list of parameters in datafile.
        """
        if not self.__keys:
            self.__keys = [k.strip().lower() for k in self.crFileGetParameter('keywords').split(',')]

        return self.__keys

    def __getitem__(self, key):
        """Operator [], for f['parameter'] returning parameter from
        datafile."""
        if isinstance(key, str) and key in self.keys():
            return self.crFileGetParameter(key)
        else:
            raise KeyError(key)

    def __contains__(self, key):
        """Operator for testing if parameter is in datafile.

        For the following usage:

        f=open('filename')
        if 'parameter' in f:
            print f['parameter']

        """
        if not isinstance(key, str):
            raise KeyError(key)
        elif key in self.keys():
            return True
        else:
            return False

    ## ---------------------------------------------------------------------
    #  Attributes

    # block
    def __getBlock(self):
        return self.crFileGetParameter('block')

    def __setBlock(self, value):
        self.crFileSetParameter('block', value)

    block = property(__getBlock, __setBlock)

    # blocksize
    def __getBlockSize(self):
        return self.crFileGetParameter('blocksize')

    def __setBlockSize(self, value):
        self.crFileSetParameter('blocksize', value)

    blocksize = property(__getBlockSize, __setBlockSize)

    # shiftvector
    def __getShiftVector(self):
        return self.crFileGetParameter('shiftvector')

    def __setShiftVector(self, value):
        self.crFileSetParameter('shiftvector', value)

    shiftvector = property(__getShiftVector, __setShiftVector)

    # selectedantennas
    def __getSelectedAntennas(self):
        return self.crFileGetParameter('selectedantennas')

    def __setSelectedAntennas(self, value):
        self.crFileSetParameter('selectedantennas', value)

    selectedantennas = property(__getSelectedAntennas, __setSelectedAntennas)

def open(filename, filetype=None):
    """Open a HDF5 file, returns a DataReader object. This is the
    preferred way to open a HDF5 file.

    *filename* name of datafile
    *filetype* can be 'LOPESEvent', 'LOFAR_TBB' or 'tbbctl'
    """

    if filetype:
        if filetype in ['LOPESEvent', 'LOFAR_TBB', 'tbbctl']:
            dr = crFileOpen(filename, filetype)
        else:
            raise KeyError('Unknown filetype, '+str(filetype))
    else:
        dr = crFileOpen(filename)

    return dr

