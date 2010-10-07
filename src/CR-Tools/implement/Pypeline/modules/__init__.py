"""A Python library for analysing LOFAR TBB data.
"""

# Import core functionality
import core

# Make core functionality available in local namespace
from core import *

# Ensure from pycrtools import * works correctly
# for all functions exported from core
__all__ = [func for func in dir(core) if "__" not in  func]

