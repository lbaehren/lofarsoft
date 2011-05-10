# for module in hftools_modules:
#     __import__(module, globals(), locals(), ['...'])

from mVector import *
from mArray import *
from mMath import *
from mNumpy import *
from mFFT import *
from mFFTW import *
from mFilter import *
from mFitting import *
from mRF import *
from mImaging import *
from mIO import *
from mEndPointRadiation import *

import pycrtools.rftools as rf

# Add additional hftools methods
from _hftools import *

trackHistory.__doc__="""
Function for PYCRTOOLS module. Globally switch history tracking of
hArrays on (True) or off (False). Default at start-up is "on". This
setting is valid for all arrays created after the function is
called. If history tracking is on, you can use array.history() to see
a list of actions that were performed with the array."""
