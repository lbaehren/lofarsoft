"""This is the top level package for input/output of data using pycrtools.

Currently only LOFAR TBB data is supported.
In order to transparantly support both raw and calibrated data from any function or module one should always use the interfaces defined in the :mod:`~pycrtools.IO.interfaces` submodule.
"""

__all__ = ['TBB']

from interfaces import IOInterface
from TBB import TBBData, open, set, get, applySelection

