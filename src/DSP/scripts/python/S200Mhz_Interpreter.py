"""
This module provides the interpretation of three measurement sets (L2007_02797, L2007_03017, L2007_02789) which encompass a 24 hour survey, 1 second integration time, 1 Khz resolution of the frequencies 110-190 Mhz (200 Mhz mode). The structure of these measurement sets are different to the extent that its metadata cannot describe it. This module provides the correct interpretation to these measurement sets.
"""

import math
from HBA_I200Mhz import * # these contain the definitions of the HBA 200Mhz mode


# This array contains the absolute subband references (see HBA_I200Mhz) for each measurement set
MS_INFO = { "L2007_02797": {
            'MS_ABS_SUBBAND_START': 316,
            'MS_SUBBAND_COUNT'    : 36,
            'MS_ANTENNA_COUNT'    : 4},
            "L2007_03017": {
            'MS_ABS_SUBBAND_START': 172,
            'MS_SUBBAND_COUNT'    : 36,
            'MS_ANTENNA_COUNT'    : 4},
            "L2007_02789": {
            'MS_ABS_SUBBAND_START': 460,
            'MS_SUBBAND_COUNT'    : 36,
            'MS_ANTENNA_COUNT'    : 4}
         }

         
def getAbsoluteSubband(ms, subband, antenna):
   """
   Returns the absolute subband value of a given measurement set and its local subband and antenna number
   """
   return ((MS_INFO[ms]['MS_ABS_SUBBAND_START'] - antenna*MS_INFO[ms]['MS_SUBBAND_COUNT']) - subband)


def getAbsoluteFreq(ms, relsubband, antenna, channel=0):
   """
   Returns the absolute frequency value of a given measurement set and its local subband and antenna number
   """
   return getFreq(getAbsoluteSubband(ms, antenna, relsubband), channel)


def getMSByFreq(freq):
   """
   Returns the location of the measurement set where the data of the specified frequency is stored. The return value is a tuple of the form: (measurement set, subband, antenna) 
   """
   return(apply(getMSBySubband, getSubChnlByFreq(freq)))


def getMSBySubband(subband):
   """
   Returns the location of the measurement set where the data of the specified absolute subband and channel is stored. The return value is a tuple of the form: (measurement set, subband, antenna) 
   """
   for (msName, msInfo) in MS_INFO.items():
      if ((subband <= msInfo['MS_ABS_SUBBAND_START']) and (subband > msInfo['MS_ABS_SUBBAND_START']- msInfo['MS_ANTENNA_COUNT']* msInfo['MS_SUBBAND_COUNT'])):
         diff = msInfo['MS_ABS_SUBBAND_START'] - subband
         antenna = int(math.floor(diff / msInfo['MS_SUBBAND_COUNT']))
         relsubband = int(diff % msInfo['MS_SUBBAND_COUNT'])
         return (msName, antenna, relsubband)   


