from MeasurementSet  import MeasurementSet
from numpy           import *

class BandpassExtractor:
   "Finds the bandpass for a given measurement set"
   
   def __init__(self, ms):
      "ms is a measurement set"
      self.ms = ms
      
   def extractBandpassFromData(self, tdata, take=0.1):
      """
      The expected shape of data is a numpy 2-dim array (chnl, time). take is the amount of clean records to derive the bandpass from.
      """
      data = tdata.transpose()
      datam = data.mean(1)
      datac = datam.copy()
      # data which is close to RFI (in time), is less desirable, so we bleed RFI to neighbouring channels
      for i in range(1,len(datam)):
         if (datac[i-1] > datac[i]): datac[i]+= 0.8*(datac[i-1]-datac[i])
      for i in range(len(datam)-2,-1,-1):
         if (datac[i+1] > datac[i]): datac[i]+= 0.8*(datac[i+1]-datac[i])
      # now sort the list
      slist = datac.argsort()
      # trim zero mean values
      slist = [s for s in slist if data[s].mean() > 0]
      takeam = int(take * len(data))
      datset = data[slist[:takeam]].mean(0)
      return datset
      
   extractBandpassFromData = classmethod(extractBandpassFromData)
      