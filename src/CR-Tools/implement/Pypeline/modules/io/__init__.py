"""This is the top level package for input/output of data using pycrtools.

It defines only one function `open` that is used to open all supported file
types.

"""

__all__ = ['tbb', 'interfaces']

import tbb
import pyfits
import pycrtools as cr
import __builtin__ # Needed because from io import * will cause overloading

def open(filename, *args, **kwargs):
    """Open a supported file type or fall back to Python built in open
    function.
    """

    # Get file extension to determine type
    ext = filename.split(".")[-1].strip().lower()

    if ext == "h5":
        # Open file with LOFAR TBB data
        return tbb.open(filename, *args, **kwargs)
    elif ext == "event":
        # Open LOPES Event file with pyfits
        return cr.crfile(filename, *args, **kwargs)
    elif ext == "fits":
        # Open FITS file with pyfits
        return pyfits.open(filename, *args, **kwargs)
    else:
        # Fall back to regular Python `open` function
        return __builtin__.open(filename, *args, **kwargs)

