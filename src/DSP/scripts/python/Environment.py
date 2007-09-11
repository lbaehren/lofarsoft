# matplotlib (-pylab) MUST run with Tk or it will clash with the DSPlotter. Edit the matplotlibrc config file

from numpy import *

from MeasurementSet           import *                   # import the interface to a measurement set
from DynamicSpectrumPlotter   import *                   # plotting module with cross-sections
from HBA_I200Mhz              import *                   # import the 200Mhz definitions
from S200Mhz_Interpreter      import *                   # import the S200Mhz interpreter
from S200Mhz_MS               import S200Mhz_MS          # import the 1 sec 1 Khz interface
from S200Mhz_1min1Khz         import S200Mhz_1min1Khz    # import the 1 min 1 Khz interface
from S200Mhz_DSP              import S200Mhz_DSP         # import the 1 min 1 Khz plotter
from HistogramGenerator       import HistogramGenerator  # import histogram tools
from PercentileGenerator      import PercentileGenerator # import percentile tools
import BandpassFitters                                   # import bandpass extractor
import GalacticForegroundFitters
import Storage
import Flaggers
from FlagIO import FlagArray
from Tools import *

##ms = MeasurementSet('/raid/data/renting')
p  = BufferedDSPlotter(500)      # maintain a 500MB buffer
##s  = S200Mhz_1min1Khz('/raid/data/koning/data/1min1Khz.h5')
##sp = S200Mhz_DSP(s)
##st = Storage.Storage('/raid/data/koning/data/analysis.h5')

# import stuff into this namespace
