from numpy import *

class FlagArray:
   """Reads flagging information from a binary array"""
   def __init__(self, flagarray=None, shape=None):
      """Either provide a previous FlagArray 'flagdata' or an array that can be interpreted as such. or provide the shape of the to be flagged data to create a new flagarray. Compression occurs over the first axis"""
      if flagarray is not None:
         if isinstance(flagarray, FlagArray) or (flagarray.max() > 1):
            self.fdata = flagarray
            self.shape = (flagarray.shape[0], flagarray.shape[1] * 8)
         else:
            self.shape = flagarray.shape
            #flagarray.resize(int(ceil(len(flagarray)/8.0))*8)
            self.fdata= array([(flagarray[:, i:i+8] * [1,2,4,8,16,32,64,128][:self.shape[1]-i]).sum(1) for i in xrange(0, self.shape[1], 8)], uint8).transpose()
      elif shape is not None:
         self.fdata = zeros((shape[0], int(ceil(shape[1]/8.0))), uint8)   
         self.shape = shape
   
         
      
         
   def __getSection(self, d1slice, d2slice):
      "Get a section from the flag array described by the two slices"
      ele = lambda ts: self.fdata[d1slice, int(ts / 8)] >> (ts % 8) & 1
      if isinstance(d2slice, slice):
         start = d2slice.start or 0
         end   = d2slice.stop or self.shape[1]
         step  = d2slice.step or 1
      else:
         start, end, step = d2slice, d2slice+1, 1
         
      if ((end-start)/step > 1):
         return array([ele(t) for t in xrange(start, end, step)]).transpose()
      else:
         return ele(start)
         
   def __setSection(self, d1slice, d2slice, value):
      "Set a section of the size described by the two slices with the 2-dim numpy array value. The array MUST contain 1s or 0s, or booleans"
      # f = lambda s,b : ((t&1<<b) ^ s<<b) ^ t
      def setele(ts, v):
         tsp = int(ts/8)
         tof = ts % 8
         # It is impossible to directly set a bit to a desired value. Instead we find out first
         # which bits need to be toggled and create a mask for that. The toggle mask can then 
         # be applied to the dataset.
         # - the toggle map is created by first &-masking the set with the bit we're interested in.
         #   Then we apply ^ (XOR) that map with the new desired values. This will result into a new
         # map in which 0 means that the bit at that position doesn't need to change, and 1 that it does
         # When we then ^ (XOR) this map with the original dataset, we've reached our goal.
         self.fdata[d1slice, tsp] = ((self.fdata[d1slice, tsp] & 1<<tof) ^ v<<tof) ^ self.fdata[d1slice, tsp]
         
      if isinstance(value, int): value = array([value])
      if isinstance(value, list): value = array(value)
      
      if isinstance(d2slice, slice):
         start = d2slice.start or 0
         end   = d2slice.stop or self.shape[1]
         step  = d2slice.step or 1
      else:
         start, end, step = d2slice, d2slice+1, 1
      if ((end-start)/step > 1):
         if value.ndim == 1: value = value[:, newaxis]
         map(setele, xrange(start,end,step), value)
      else:
         if value.ndim == 1: value = value[newaxis, :]
         setele(start, value[0])
      
   
   
   def __getitem__(self, key):
      if isinstance(key, tuple):
         return self.__getSection(key[0], key[1])
      else:
         return self.__getSection(key, slice(None, None, None))
      
   def __setitem__(self, key, value):   
      if isinstance(key, tuple):
         return self.__setSection(key[0], key[1], value)
      else:
         return self.__setSection(key, slice(None, None, None), value)
       
         
#            
#    def getHSlice(self, pos, length=None):
#       "Get a horizontal slice from the flag array, starting from pos. If length is set, it will return a 2-dimensional array of length items"
#       ele = lambda ts: self.fdata[:, int(ts / 8)] & self.shft[ts % 8]
#       if length is None:
#          return ele(pos).astype(bool)
#       else:
#          return array([ele(t) for t in xrange(pos, pos+length)]).astype(bool)
#      
#        
#    def setHSlice(self, pos, value):
#       "set a horizontal slice (2nd dim constant value) of the flag array at position pos. value should be a numpy array which can be interpreted as boolean values"
#       rpos = int(pos/8)
#       roff = pos % 8
#       self.fdata[:, rpos] = self.fdata[:, rpos] ^ (self.shft[roff] * value)
#       
#    def setVSlice(self, pos, value):
#       "set a verticial slice (1st dim constant value) at position pos. value should be a numpy arry which can be interpreted as boolean values"
#       value.resize(len(self.fdata[0])*8)
#       self.fdata[rpos] = [sum(value[i:i+8] * self.shft) for i in xrange(0, len(value), 8)]
# 
#             
#    def __getitem__(self, key):
#       if isinstance(key, int):
#          pass
#       elif isinstance(key, tuple):
#          if isinstance(key[0], slice):
#             
#          if key[0] == ':' and len(key) > 1:
#             rnge = map(int, key[1].split(':'))
#             if len(rnge) > 1:
#                return self.getHSlice(rnge[0], rnge[1]-rnge[0])
#             else:
#                return self.getHSlice(rnge[0])
#     
#            
#    def __setitem__(self, key, value):
#       if isinstance(key, int):
#          return relf.setVSlice(key, value)
#       elif isinstance(key, tuple):
#          if (key[0] == ':'): # assumes shape ":,int-value"
#             return self.setHSlice(int(eles[1]), value)
#       
 
