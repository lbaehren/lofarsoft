"""
On autocorrelations, the galactic foreground has a very distinct shape which can be fit and divided from the signal. This is for instance helpful for RFI detection.
Much like the BandpassFitters, this module contains three types of functions: 
   Extractor: extracts the galactic foreground and creates a mean signal
   Fitter   : fits a function to the mean signal
   Filter   : takes in a dataset and divides out the foreground signal
   
   a fitter and a filter can be generated using generateFitter and generateFilter
"""
from numpy import *


class GalacticForegroundExtractor:
   "base class. Makes itself callable so that it behaves like a regular function"
   def __call__(self, data):
      return self.extract(data)
   
   
class MeanExtractor:
   "generates a mean signal taken across all channels"
   def extract(self, data):
      return data.mean(0)
   
   
class SmartMeanExtractor(GalacticForegroundExtractor):
   """
   A more clever version of the MeanExtractor. It attempts to collect a clean subset of the array and generate a clean signal based on that subset. The incoming data MUST be bandpass filtered.
   """
   def __init__(self, samplesize=0.5, spread=False, bleed=False):
      """
      samplesize: the relative size of the clean subset; 0 < samplesize <= 1
      """
      self.samplesize = samplesize
      
   def extract(self, data):
      "required input: data - a 2-dim array (channel, time) of floats"
      # get the mean
      sbm = data.mean(1)
      slist = sbm.argsort()
      # trim zeros
      slist = [s for s in slist if data[s].mean() > 0]
      takeam = int(self.samplesize * len(data))
      datset = data[slist[:takeam]].mean(0)
      return datset
   
   
class GalacticForegroundFitter:
   "base class. Makes itself callable so that it behaves like a regular function"
   def __call__(self, data):
      return self.fit(data)   

class PolyFitter(GalacticForegroundFitter):
   "performs a regular polynomial fit"
   def __init__(self, polycount=9):
      "polycount is the number of polynomial coefficients to use for the fit"
      self.polycount = polycount
   
   def fit(self, data):
      thefit = poly1d(polyfit(arange(len(data)), data, self.polycount), r=0)(arange(len(data)))
      # thefit *= 1/(thefit.mean())
      return thefit
      

def generateFilter(fitter = None):
   if fitter == None: fitter = defaultFitter
   return lambda data: data / fitter(data)

def generateFitter(*args):
   """Generates a chain of fitters on top of an extractor. Each argument should be a Fitter instance and only the first one can be an Extractor. For example:
   fitter = generateFitter(SmartMeanExtractor(), PolyFitter())
   """
   a = lambda data: reduce(lambda a,b: b(a), [data] + list(args))
   return a
      


defaultFitter = generateFitter(SmartMeanExtractor(), PolyFitter())
defaultFilter = generateFilter(defaultFitter)
   