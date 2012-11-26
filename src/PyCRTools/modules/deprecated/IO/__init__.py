"""This is the top level package for input/output of data using pycrtools.

Currently only LOFAR TBB data is supported.
In order to transparantly support both raw and calibrated data from any function or module one should always use the interfaces defined in the :mod:`~pycrtools.IO.interfaces` submodule.
"""

__all__ = ['TBB']

from interfaces import IOInterface
from TBB import TBBData, open, set, get, applySelection
import os


def getFilenames(datadir, root="", extension=".h5"):
    """getFilenames(directory,extension) -> [file1.ext, file2.ext, ...]

    Returns a list of all files in a directory which start with a
    certain string ('root') and end with a specified extension. The
    filenames are returned with their pathname added. If empty strings
    are provided all files will be returned.
    """
    tempfilenames = os.listdir(datadir)
    lext = len(extension)
    lroot = len(root)
    filenames = []
    for f in tempfilenames:
        if (f[:lroot] == root) & (f[-lext:] == extension):
            filenames.append(os.path.join(datadir, f))
    return filenames
