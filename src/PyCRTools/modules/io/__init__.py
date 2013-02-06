"""This is the top level package for input/output of data using pycrtools.

It defines only one function `open` that is used to open all supported file
types.

"""

__all__ = ['tbb', 'interfaces', 'open', 'beam']

import os
import tbb
import beam
import pyfits
import pycrtools as cr
import __builtin__  # Needed because from io import * will cause overloading


def open(filename, *args, **kwargs):
    """Open a supported file type or fall back to Python built in open function.

    ================== ==================================================
    Filename extension File type
    ================== ==================================================
    h5                 LOFAR TBB file
    event              LOPES file (**not functioning**)
    fits               FITS file
    beam               Beam data (tasks/beamformer.py output)
    *other*            Open with regular python :func:`open` function.
    ================== ==================================================

    """
    if isinstance(filename, list):
        if filename[0].endswith(".tbb"):
            return tbb.open(filename, *args, **kwargs)
        else:
            filename = [os.path.expandvars(os.path.expanduser(f)) for f in filename]
            ext = set()
            for f in filename:
                ext.add(f.split(".")[-1].strip().lower())
            if len(ext) != 1:
                raise ValueError("Multiple extensions")
            else:
                ext = list(ext)[0]
    else:
        filename = os.path.expandvars(os.path.expanduser(filename))
        # Get file extension to determine type
        ext = filename.split(".")[-1].strip().lower()

    if ext == "h5":
        # Open file with LOFAR TBB data
        return tbb.open(filename, *args, **kwargs)
    elif ext == "event":
        # Open LOPES Event file with pyfits
        return cr.crfile(filename, *args, **kwargs)
    elif ext == "beam":
        # Open file with beam data
        return beam.open(filename, *args, **kwargs)
    elif ext == "fits":
        # Open FITS file with pyfits
        return pyfits.open(filename, *args, **kwargs)
    else:
        # Fall back to regular Python `open` function
        return __builtin__.open(filename, *args, **kwargs)
