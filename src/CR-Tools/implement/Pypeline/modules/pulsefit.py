#
#  triggered-pulsefitting.py - fit the direction of the triggered pulse
#  
#
#  Created by Andreas Horneffer on Aug. 1, 2010.
#  Copyright (c) 2010, Andreas Horneffer. All rights reserved.
#  Partially ported to pycrtools on Nov. 15, 2010
"""
This module supplies different functions to fit the direction of a triggered
pulse

"""
import os
#import pycr as cr
from pycrtools import *
from pycrtools import IO
import matching as match
import srcfind as sfind
import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import fmin

rad2deg = 180./np.pi
#-------------------------------------------------------------------------------

#------------------------------------------------------------ simplexPositionFit
def simplexPositionFit(crfile, cr_fft, antenna_positions, start_position, ant_indices, 
                       cr_freqs, FarField=True, blocksize=-1):
  if not FarField:
      print 'Warning: only FarField == True is implemented!'
  ants = cr_fft.shape()[0]
  fftlength = cr_fft.shape()[1]
  if (blocksize<=0):
    blocksize = 2*(fftlength-1)
  else:
    if (fftlength != (blocksize/2+1)):
      raise ValueError, '(fftlength != (blocksize/2+1))'
  azel=hArray(float,dimensions=[3])
  cartesian=azel.new()
  delays=hArray(float,dimensions=[crfile["nofAntennas"]])
  weights=hArray(complex,dimensions=[crfile["fftLength"]],name="Complex Weights")
  freqs = hArray(crfile["frequencyValues"]) # a FloatVec comes out, so put it into hArray
  phases=hArray(float,dimensions=[crfile["fftLength"]],name="Phases",xvalues=crfile["frequencyValues"]) 
  shifted_fft=hArray(complex,dimensions=[crfile["fftLength"]])
  beamformed_fft=hArray(complex,dimensions=[crfile["fftLength"]])
  beamformed_efield=hArray(float,dimensions=[crfile["blocksize"]])
  beamformed_efield_smoothed=hArray(float,dimensions=[crfile["blocksize"]])

  #azel = np.zeros( (3) );
  #cartesian = np.zeros( (3) );
  #delays =  np.zeros( (ants) );
  #weights = np.empty( (ants, fftlength), dtype=complex )
  #shifted_fft = np.empty( (ants, fftlength), dtype=complex )
  #beamformed_fft = np.empty( (fftlength), dtype=complex )
  #beamformed_efield =  np.zeros( (blocksize) )
  
  window = hArray(float, dimensions=[5], fill=0.2) # np.array([1.,1.,1.,1.,1.]);  window /= np.sum(window)

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
        
      hCoordinateConvert(azel, CoordinateTypes.AzElRadius, cartesian, CoordinateTypes.Cartesian, True)
      hGeometricDelays(delays, antenna_positions, cartesian, True)   
      hDelayToPhase(phases, freqs, delays) # FIX!
      hPhaseToComplex(weights,phases)
      hMul(shifted_fft, cr_fft, weights)
      #cr.azElRadius2Cartesian(cartesian, azel, True)
#      cr.geometricDelays(delays, antenna_positions, cartesian, FarField)
#      cr.complexWeights(weights, delays, cr_freqs)        
#      shifted_fft = cr_fft*weights        
      beamformed_fft.fill(0.0) # = shifted_fft[ant_indices[0]]  
#      beamformed_fft = shifted_fft[ant_indices[0]]
      #for n in ant_indices[1:]:
#       beamformed_fft += shifted_fft[n]
      shifted_fft[...].addto(beamformed_fft)

      hInvFFTw(beamformed_efield,beamformed_fft)
      beamformed_efield.abs() # make absolute value!
      hRunningAverage(beamformed_efield_smoothed, beamformed_efield, 5, hWEIGHTS.GAUSSIAN)

      #cr.backwardFFTW(beamformed_efield, beamformed_fft);         
      erg = - beamformed_efield_smoothed.max()[0] # just the maximum of beamformed_efield_smoothed
      #erg = - np.max(np.convolve(np.abs(beamformed_efield),window,'same'))
    #print "beamform_function azel:", azel_in, " result:", erg
    return erg

  optErg = fmin(beamform_function, start_position, xtol=1e-2, ftol=1e-4, full_output=1)
  return optErg

#------------------------------------------------------------- triggerMessageFit
def triggerMessageFit(crfile, triggerMessageFile, fittype='bruteForce'):  # crfile has to be imported using IO module!
  #Get the trigger-message information
  fileDate = crfile["Date"]
  fileSamplenum = crfile["SAMPLE_NUMBER"][0]                  # assuming start sample nr. is the same for all antennas
  ddate = fileDate + fileSamplenum / 200.0e6             
  (mIDs, mdDates, mTdiffs, mTriggerDates, mSampleNums) = match.matchTriggerfileToTime((ddate+0.00033024),triggerMessageFile)
  if len(mIDs) == 0:
      print 'NO TRIGGERS FOUND'
  #get the position for that
  #match_positions = np.reshape(dr["antenna_position"][mIDs],(len(mIDs)*3))
  # !!! GET antenna positions for the correct station. How do you know the station? 
  # -> use IO, metadata (sub)modules
  
  nofAntennas = crfile["nofAntennas"]
  # now trim the matched trigger IDs to include only antennas which are actually in the data!
  validChannels = np.argwhere(mIDs < nofAntennas) # get indices in array
  print mTdiffs
  print len(mTdiffs)
  mTdiffs = mTdiffs[validChannels].ravel()
  print mTdiffs
  print len(mTdiffs)
  print ' '
  mIDs = mIDs[validChannels].ravel()
  print len(mIDs)
  match_positions = crfile["RelativeAntennaPositions"].toNumpy()[mIDs].reshape(3 * len(mIDs))
  #print mTdiffs
  #print len(mTdiffs)
#  print match_positions
#  print match_positions
#  print len(match_positions)

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
  toffset = np.mean((mTriggerDates - fileDate) + (mSampleNums - fileSamplenum) / 200.0e6) # matching.py returns np array, so keep it (doing mean anyway)
  return (degaz, degel, mse, toffset, len(mIDs) )

#------------------------------------------------------------------ fullPulseFit
def fullPulseFit(filename, triggerMessageFile, antennaset, FarField=False):
  #Open the file, 
  #dr = cr.open(filename,'LOFAR_TBB')
  #import pdb; pdb.set_trace()
  crfile = IO.open([filename])
  crfile.setAntennaset(antennaset)
  # validity check - has to move !!!
  dates = crfile["TIME"]
  print crfile["shift"]
  print crfile["shift"].val()
  if dates.max() - dates.min() > 0:
      print 'Invalid times in file!!!'
      
  if (crfile["sampleFrequency"] != 200e6):
    print crfile["sampleFrequency"]
    raise ValueError, "Can only process events taken with 200MHz samplingrate."
  #Get the trigger message data
  trigData = triggerMessageFit(crfile, triggerMessageFile)
  trigLinfitData = triggerMessageFit(crfile, triggerMessageFile, fittype='linearFit')
  #Set the parameters
  samplefreq = 200.0e6 # must be
  blocksize = 512
  # crfile["blocksize"] = blocksize # doesn't work, bug?
  crfile.set("blocksize", blocksize)
  print 'File size is %d' % crfile["Filesize"]
  print 'Block size is now: %d' % crfile["blocksize"]
  print 'So there are: %d blocks' % int((crfile["Filesize"]) / int(crfile["blocksize"]))
  blockNo = int((trigData[3] * samplefreq) / blocksize)
  print "fullPulseFit: set block-number to:", blockNo
#  crfile.set("block", blockNo)
  ants = crfile["nofAntennas"]
  fftlength = crfile["fftLength"]
  #Get the data
  cr_time = hArray(float, dimensions=[blocksize])
  cr_freqs = hArray(float, dimensions=[blocksize/2 + 1])
  #cr_time = np.empty(blocksize)
  
#  cr_time = crfile["emptyFx"]  
#  cr_freqs = crfile["emptyFFT"] 
  
 # ??? dr.read("Time",cr_time)
  #cr_freqs = np.empty(fftlength)
  
# ???  dr.read("Frequency",cr_freqs)  
  
  # cr_efield = np.empty((ants, blocksize))
  
  cr_efield = crfile["emptyFx"]
  crfile.readdata(cr_efield, blockNo) # crfile["Fx"] crashes on invalid block number ???? While it was set to a valid value...
  cr_fft = crfile["emptyFFT"]
  print 'Number of antennas = %d' % cr_fft.shape()[0]
  print 'Block length fft = %d' % cr_fft.shape()[1]
#  dr.read("Fx",cr_efield)

  antenna_positions = crfile["RelativeAntennaPositions"]
  #Now calculate the foruier-transform of the data
  
  #    fftdata[...].fftcasa(fxdata[...], nyquistZone)
  cr_fft[...].fftw(cr_efield[...]) 
  #cr_fft = np.empty( (ants, fftlength), dtype=complex )
  
#  for ant in range(ants):
#    status =  cr.forwardFFTW(cr_fft[ant], cr_efield[ant])
  if (FarField):
    start_position = [trigData[0], trigData[1]]
  else:
    start_position = [trigData[0], trigData[1], 30000.]
  ant_indices = range(0,96,2)
  fitDataEven = simplexPositionFit(crfile, cr_fft, antenna_positions, start_position, ant_indices, 
                                   cr_freqs, FarField=FarField,blocksize=blocksize)
  ant_indices = range(1,96,2)
  fitDataOdd = simplexPositionFit(crfile, cr_fft, antenna_positions, start_position, ant_indices, 
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
