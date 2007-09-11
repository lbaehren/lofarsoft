from HBA_I200Mhz import *
from S200Mhz_Interpreter import *
# HDF5 file support
import tables

class S200Mhz_1min1Khz:
   """
   This class provides an interface to the 1 minute, 1 Khz summary of the 110-190Mhz survey (L2007_02797, L2007_03017, L2007_02789), containing amplitudes only.
   """

   def __init__(self, file):
      """
      Creates the interface to the given datafile
      """
      self.dataFile = file
      self.__load()
      
   def __del__(self):
      self.h5F.close()
      
   def __load(self):
      """
      open the hdf5 file and see what's in there
      """
      self.h5F = tables.openFile(self.dataFile, mode='r')
      self.__fillRelAvail()
      self.__fillAbsAvail()
         
   def __fillRelAvail(self):
      """
      create an index of the available data sets, directly named after their MS source (see S200MhzInterpreter)
      """
      self.relAvail = {}
      for s in self.h5F.listNodes(self.h5F.root):
         parts = s.name.split('_')
         # form of antenna name: (Y)_(MS)_SB(SB)_A(A)_P(P)
         MS,SB,A,P = parts[0]+'_'+parts[1], int(parts[2][2:]), int(parts[3][1:]), int(parts[4][1:])
         if not self.relAvail.has_key(MS):
            self.relAvail[MS] = {}
         if not self.relAvail[MS].has_key(SB):
            self.relAvail[MS][SB] = {}
         if not self.relAvail[MS][SB].has_key(A):
            self.relAvail[MS][SB][A] = []
         self.relAvail[MS][SB][A].append(P)
                 
   def __fillAbsAvail(self):
      """
      Create an array of the available subbands from the absolute 200Mhz perspective
      """
      self.absAvail = {}
      for (MSName, subInfo) in self.relAvail.items():
         for (relSub, antInfo) in subInfo.items():
            for (ant, pInfo) in antInfo.items():
               sub = getAbsoluteSubband(MSName, relSub, ant)
               self.absAvail[sub] = pInfo
   
   def getAllSetNames(self):
      """
      Returns a list of the names of the datasets available in the HDF5 file
      """
      return [s.name for s in self.h5F.listNodes(self.h5F.root)]
         
   def getSet(self, s):
      """
      Returns the data of a given set name. Note that the first channel contains no information (zeros)
      """
      data = self.h5F.getNode(self.h5F.root, s).read()
      return data
   
   def getRelSet(self, MS, SB, A, P=0):
      """
      Returns the data of the set specified by measurement set name, relative subband, antenna, polarization
      """
      return self.getSet('%s_SB%02d_A%d_P%d' % (MS, SB, A, P))
   
   def getAbsSet(self, SB, P=0):
      """
      Returns the data of the set specified by absolute subband and polarization
      """
      (MS, ant, sub) = getMSBySubband(SB)
      return self.getRelSet(MS, sub, ant, P)
   
   def getSubband(self, SB, P=0):
      """
      Returns the data of the set specified by absolute subband and polarization
      """
      return self.getAbsSet(SB, P).transpose()
   
   def listFreqs(self):
      """
      Returns a list of the frequency coverage of the dataset
      """
      sbs = self.absAvail.keys()
      sbs.sort()
      result = []
      last = sbs[0]
      prev = sbs[0]
      for sb in sbs:
         if (sb-prev) > 1:
            result.append((getFreq(last), getFreq(prev, 255)))
            last = sb
         prev = sb
      if last is not sb:
         result.append((getFreq(last), getFreq(prev, 255)))
      result.sort()
      return result

   def listSubbands(self):
      "Returns a sorted list of available subband numbers"
      lst = self.absAvail.keys()
      lst.sort()
      return lst
      