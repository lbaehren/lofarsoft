#
#  triggered-pulsefitting.py - fit the direction of the triggered pulse
#  
#
#  Created by Andreas Horneffer on Aug. 1, 2010.
#  Copyright (c) 2010, Andreas Horneffer. All rights reserved.

"""
This module supplies differnt funtcions to fit the direction of a triggered
pulse

"""
import os
import pycr as cr
import numpy as np
import matplotlib.pyplot as plt
import matching as match
import srcfind as sfind
from scipy.optimize import fmin

rad2deg = 180./np.pi
#-------------------------------------------------------------------------------

#------------------------------------------------------------ simplexPositionFit
def simplexPositionFit(cr_fft, antenna_positions, start_position, ant_indices, 
                       cr_freqs, FarField=False, blocksize=-1):
  ants = cr_fft.shape[0]
  fftlength = cr_fft.shape[1]
  if (blocksize<=0):
    blocksize = 2*(fftlength-1)
  else:
    if (fftlength != (blocksize/2+1)):
      raise ValueError, '(fftlength != (blocksize/2+1))'
  azel = np.zeros( (3) );
  cartesian = np.zeros( (3) );
  delays =  np.zeros( (ants) );
  weights = np.empty( (ants, fftlength), dtype=complex )
  shifted_fft = np.empty( (ants, fftlength), dtype=complex )
  beamformed_fft = np.empty( (fftlength), dtype=complex )
  beamformed_efield =  np.zeros( (blocksize) )
  window = np.array([1.,1.,1.,1.,1.])
  window /= np.sum(window)

  def beamform_function(azel_in):
    if ( azel_in[0] > 360. or azel_in[0] < 0. or azel_in[1] > 90. or azel_in[1] < 0.):
      erg = 0.
    else:
      azel[0] = azel_in[0]
      azel[1] = azel_in[1]
      if (FarField):
        azel[2] = 1.
      else:
        azel[2] = azel_in[2]
      cr.azElRadius2Cartesian(cartesian, azel, True)
      cr.geometricDelays(delays, antenna_positions, cartesian, FarField)
      cr.complexWeights(weights, delays, cr_freqs)        
      shifted_fft = cr_fft*weights        
      beamformed_fft = shifted_fft[ant_indices[0]]
      for n in ant_indices[1:]:
        beamformed_fft += shifted_fft[n]
      cr.backwardFFTW(beamformed_efield, beamformed_fft);         
      erg = - np.max(np.convolve(np.abs(beamformed_efield),window,'same'))
    #print "beamform_function azel:", azel_in, " result:", erg
    return erg

  optErg = fmin(beamform_function, start_position, xtol=1e-2, ftol=1e-4, full_output=1)
  return optErg

#------------------------------------------------------------- triggerMessageFit
def triggerMessageFit(dr, triggerMessageFile, fittype='bruteForce'):
  #Get the trigger-message information
  ddate =dr["date"] + dr["sample_number"][0]/200e6
  (mIDs, mdDates, mTdiffs, mTriggerDates, mSampleNums) = match.matchTriggerfileToTime((ddate+0.00033024),triggerMessageFile)
  #get the position for that
  match_positions = np.reshape(dr["antenna_position"][mIDs],(len(mIDs)*3))
  if (fittype=='linearFit'):
    (radaz, radel) = sfind.directionForHorizontalArray(match_positions, mTdiffs)
  elif (fittype=='bruteForce'):
    (radaz, radel, bftime) = sfind.directionBruteForceSearch(match_positions, mTdiffs)
  else : 
    radaz = 0.
    radel = 0.
  mse = sfind.mse(radaz, radel, match_positions, mTdiffs)
  degaz = radaz*rad2deg
  degel = radel*rad2deg
  if (degaz < 0.):
    degaz += 360.
  #get the average time-offset
  toffset = np.mean((mTriggerDates-dr["date"])+(mSampleNums-dr["sample_number"][0])/200e6)
  return (degaz, degel, mse, toffset, len(mIDs) )

#------------------------------------------------------------------ fullPulseFit
def fullPulseFit(filename, triggerMessageFile, antennaset, FarField=False):
  #Open the file, 
  dr = cr.open(filename,'LOFAR_TBB')
  dr.antennaset = antennaset
  if (dr["samplefrequency"] != 200e6):
    raise ValueError, "Can only process events taken with 200MHz samplingrate."
  #Get the trigger message data
  trigData = triggerMessageFit(dr, triggerMessageFile)
  trigLinfitData = triggerMessageFit(dr, triggerMessageFile, fittype='linearFit')
  #Set the parameters
  samplefreq = dr["samplefrequency"]
  dr.blocksize = 512
  blocksize = dr["blocksize"]
  blockNo = int((trigData[3]*samplefreq)/blocksize)
  print "fullPulseFit: set block-number to:", blockNo
  dr.block = blockNo
  ants = dr["nofantennas"]
  fftlength = dr["fftlength"]
  #Get the data
  cr_time = np.empty(blocksize)
  dr.read("Time",cr_time)
  cr_freqs = np.empty(fftlength)
  dr.read("Frequency",cr_freqs)  
  cr_efield = np.empty((ants, blocksize))
  dr.read("Fx",cr_efield)
  antenna_positions = dr["antenna_position"]
  #Now calculate the foruier-transform of the data
  cr_fft = np.empty( (ants, fftlength), dtype=complex )
  for ant in range(ants):
    status =  cr.forwardFFTW(cr_fft[ant], cr_efield[ant])
  if (FarField):
    start_position = [trigData[0], trigData[1]]
  else:
    start_position = [trigData[0], trigData[1], 30000.]
  ant_indices = range(0,96,2)
  fitDataEven = simplexPositionFit(cr_fft, antenna_positions, start_position, ant_indices, 
                                   cr_freqs, FarField=FarField,blocksize=blocksize)
  ant_indices = range(1,96,2)
  fitDataOdd = simplexPositionFit(cr_fft, antenna_positions, start_position, ant_indices, 
                                  cr_freqs, FarField=FarField,blocksize=blocksize)
  erg = dict()
  erg['triggerFit']=trigData
  erg['linearFit']=trigLinfitData
  erg['fitEven']=fitDataEven
  erg['fitOdd']=fitDataOdd
  return erg

## Executing a module should run doctests.
#  This examines the docstrings of a module and runs the examples
#  as a selftest for the module.
#  See http://docs.python.org/library/doctest.html for more information.
if __name__=='__main__':
    import doctest
    doctest.testmod()
