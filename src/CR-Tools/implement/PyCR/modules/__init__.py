"""CR-Tools from Python
"""

import numpy as np
import warnings

# Import module for temporary access to metadata
import metadata

# Try to import everything from the HFL
try:
    import hfl
    from hfl import *
except ImportError:
    warnings.warn('Could not import hfl module, algorithms from hfl will not be available.')

