from HBA_I200Mhz import *
from S200Mhz_Interpreter import *
import MeasurementSet
import os


class S200Mhz_MS:
   """
   This class provides an interface to the data of the 200Mhz survey (L2007_02797, L2007_03017, L2007_02789) at 1 second, 1 Khz resolution. It should have access to the specified data sets (preferably in trimmed form in which only the autocorrelations remain - for faster access).
   """
   
   def __init__(self, path):
      """
      path is the directory containing the measurement sets L2007_02797, L2007_03017, L2007_02789
      """
      self.path = path
      self.__browsePath()
      
      
   def __browsePath(self):
      """
      looks for the measurement sets in this path and checks which data is available
      """
      msNames = [key for key,value in MS_INFO.items()]
      self.__msAvailable = {}
      
      for element in [ms for ms in os.listdir(self.path) if ms in msNames]:
         self.__msAvailable[element] = []
         for subband in [sb for sb in os.listdir(self.path + os.sep + element) if sb[:2] == 'SB' and sb[-3:] == '.MS']:
            subbandNo = int(subband.split('.')[0][2:])
            self.__msAvailable[element].append(subbandNo)
            
      
   
   def listAvailableSubbands(self):
      """
      Returns a list of the available subbands
      """
      dat = [getAbsoluteSubband(key, relsub, a) for a in range(4) for key,val in self.__msAvailable.items() for relsub in val]
      dat.sort()
      return dat

   
   def getMSFileBySubband(self, subband):
      """
      Returns a tuple containing a MeasurementSet object and the antenna number; Which is the location of the data of the specified subband
      """
      (msName, antenna, relsubband) = getMSBySubband(subband)
      flocation = self.path + os.sep + msName + os.sep + 'SB' + str(relsubband) + '.MS'
      msFile = MeasurementSet.MeasurementSet(flocation)
      return (msFile, antenna)
      
   
   
   def getSubband(self, subband, **pars):
      """
      obtain data from the specified subband
      time    : a tuple of the form (start, end, step). If none provided, it selects all time elements
      channels: a tuple of the form (start, end, step) or an array containing channel numbers. If none provided, it selects all channels
      polars  : an array listing the polarizations to return. If none provided, it selects all polarizations
      """
      (msFile, antenna) = self.getMSFileBySubband(subband)
      blstring = '%02d%02d' % (antenna+1, antenna+1)
      return msFile.getBaselineData(blstring, **pars)

   