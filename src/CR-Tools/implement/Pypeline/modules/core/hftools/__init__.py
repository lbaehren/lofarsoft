# hftools_modules = ["mVector",
#                    "mArray",
#                    "mMath",
#                    "mNumpy",
#                    "mFFT",
#                    "mFilter",
#                    "mFitting",
#                    "mRF",
#                    "mImaging",
#                    "mIO"
#                    ]

# for module in hftools_modules:
#     __import__(module, globals(), locals(), ['...'])

from vector import *
from array import *
from math import *
from numpy import *
from fft import *
from filter import *
from fitting import *
from rf import *
from imaging import *
from io import *

# Add additional hftools methods
from _hftools import *

trackHistory.__doc__="""
Function for PYCRTOOLS module. Globally switch history tracking of
hArrays on (True) or off (False). Default at start-up is "on". This
setting is valid for all arrays created after the function is
called. If history tracking is on, you can use array.history() to see
a list of actions that were performed with the array."""
