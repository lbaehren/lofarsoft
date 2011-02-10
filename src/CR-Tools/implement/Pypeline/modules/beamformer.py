""" 
  Module beamformer.py. Part of the LOFAR Cosmic Ray pipeline.
  Beamformer class for the LOFAR pipeline. May be superseded by dedicated (wrapped) C++ functions.
  hBeamformImage is already there for tied-array (adding) beam.
        
.. moduleauthor:: Arthur Corstanje <A.Corstanje@astro.ru.nl>
"""

from pycrtools import *

class Beamformer(object):
    """A beamformer class. As the name says, it does beamforming.
       Work in progress...
    """

    def __init__(self, crfile, cr_fft):
        nofAntennas = crfile["nofAntennas"]
        
        blocksize = crfile["blocksize"]
        
        self.delays = hArray(float,dimensions=[nofAntennas])
        self.weights = hArray(complex,dimensions = cr_fft,name="Complex Weights")
        self.freqs = hArray(crfile["frequencyValues"]) # a FloatVec comes out, so put it into hArray
        self.phases = hArray(float,dimensions=cr_fft,name="Phases",xvalues=crfile["frequencyValues"]) 
        self.shiftedFFT = hArray(complex,dimensions=cr_fft)
        self.shiftedTimeSeries = hArray(float, dimensions=[nofAntennas, blocksize])
        
        self.beamformedFFT=hArray(complex,dimensions=[crfile["fftLength"]])
        
        self.tiedArrayBeam = hArray(float, dimensions=[blocksize])
        self.incoherentBeam = hArray(float, dimensions=[blocksize])
        self.ccBeam = hArray(float, dimensions=[blocksize])
        
        self.smoothedBeam = hArray(float,dimensions=[blocksize])

        self.azel=hArray(float,dimensions=[3])
        self.cartesian=self.azel.new()
        self.blocksize = blocksize
        
  #antennaPositionsForIndices = 
  # FIX: cut out the right antennaIndices here! In antenna_positions, dimensions for new arrays, then beamforming...

    def getTiedArrayBeam(self, azel_in, fftData, antennaPositions, antennaIndices, FarField):
        if FarField:
            print 'Evaluating for az = %f, el = %f, R = inf' % (azel_in[0], azel_in[1]),
        else:
            print 'Evaluating for az = %f, el = %f, R = %f' % (azel_in[0], azel_in[1], 2000.0 / (azel_in[2])),
        if ( azel_in[0] > 360. or azel_in[0] < 0. or azel_in[1] > 90. or azel_in[1] < 0.):
            erg = 0.
        else:
            self.azel[0] = azel_in[0]
            self.azel[1] = azel_in[1]
            if (FarField):
                self.azel[2] = 1.
            else:
                self.azel[2] = 2000.0 / (azel_in[2])
#        import pdb; pdb.set_trace()
        hCoordinateConvert(self.azel, CoordinateTypes.AzElRadius, self.cartesian, CoordinateTypes.Cartesian, True)
        hGeometricDelays(self.delays, antennaPositions, self.cartesian, FarField)   
        hDelayToPhase(self.phases, self.freqs, self.delays) 
        hPhaseToComplex(self.weights, self.phases)

        hMul(self.shiftedFFT, fftData, self.weights) # Dimensions don't match: need [...] ???
        
        self.beamformedFFT.fill(0.0)   
            
        self.shiftedFFT[antennaIndices, ...].addto(self.beamformedFFT)

        hInvFFTw(self.tiedArrayBeam, self.beamformedFFT)              
        
        self.tiedArrayBeam *= (1.0 / self.blocksize)
        # that's in fact faster than dividing by the blocksize. Not that it matters here, of course

        return self.tiedArrayBeam
    
    def getAllBeams(self, azel_in, fftData, antennaPositions, antennaIndices, FarField):
    # TODO: merge half of this stuff with previous function...
        if FarField:
            print 'Evaluating for az = %f, el = %f, R = inf' % (azel_in[0], azel_in[1]),
        else:
            print 'Evaluating for az = %f, el = %f, R = %f' % (azel_in[0], azel_in[1], 2000.0 / (azel_in[2])),
        if ( azel_in[0] > 360. or azel_in[0] < 0. or azel_in[1] > 90. or azel_in[1] < 0.):
            erg = 0.
        else:
            self.azel[0] = azel_in[0]
            self.azel[1] = azel_in[1]
            if (FarField):
                self.azel[2] = 1.
            else:
                self.azel[2] = 2000.0 / (azel_in[2])
        hCoordinateConvert(self.azel, CoordinateTypes.AzElRadius, self.cartesian, CoordinateTypes.Cartesian, True)
        hGeometricDelays(self.delays, antennaPositions, self.cartesian, FarField)   
        hDelayToPhase(self.phases, self.freqs, self.delays) 
        hPhaseToComplex(self.weights, self.phases)

        hMul(self.shiftedFFT, fftData, self.weights) # Dimensions don't match: need [...] ???

        self.beamformedFFT.fill(0.0)   
        self.shiftedFFT[antennaIndices, ...].addto(self.beamformedFFT)
        hInvFFTw(self.tiedArrayBeam, self.beamformedFFT) # to test the following
        self.tiedArrayBeam *= (1.0 / self.blocksize)

#        cr_fft[...].fftw(cr_efield[...])
        self.shiftedTimeSeries[...].invfftw(self.shiftedFFT[...])
        self.shiftedTimeSeries *= (1.0 / self.blocksize) # optimize
        
        testSeries = self.tiedArrayBeam.new()
        self.shiftedTimeSeries[antennaIndices, ...].addto(testSeries)
        
#        import pdb; pdb.set_trace()
        
        testInc = self.shiftedTimeSeries.new()
        testInc[...].square(self.shiftedTimeSeries[...])
        
        self.incoherentBeam.fill(0.0) 
        
        testInc[...].addto(self.incoherentBeam)
#        pdb.set_trace()
        
        self.ccBeam.fill(0.0)
        self.ccBeam.square(self.tiedArrayBeam)
        self.ccBeam -= self.incoherentBeam
        self.ccBeam *= 0.5
#        pdb.set_trace()
        # signed square root needed! TODO: Add to hftools...       
        ccBeam = self.ccBeam.toNumpy() # hack around
        for i in range(len(ccBeam)):
            if ccBeam[i] < 0.0:
                ccBeam[i] = - np.sqrt( - ccBeam[i])
            else:
                ccBeam[i] = np.sqrt(ccBeam[i])
        self.ccBeam = hArray(ccBeam)

        self.incoherentBeam.sqrt()

#        pdb.set_trace()
        return (self.tiedArrayBeam, self.incoherentBeam, self.ccBeam)
        
    def pulseMaximizer(self, azel_in, fftData, antennaPositions, antennaIndices, FarField):
        # Develop this to calculate the pulse height in a proper way!
        tiedArrayBeam = self.getTiedArrayBeam(azel_in, fftData, antennaPositions, antennaIndices, FarField)
        tiedArrayBeam.abs() # make absolute value!
#        hRunningAverage(smoothedBeam, tiedArrayBeam, 5, hWEIGHTS.GAUSSIAN)

        value = - tiedArrayBeam.max()[0] # just the maximum. 
        print ' value = %f ' % value
        return value
    
    
    def fancyPulseMaximizer(self, azel_in, fftData, antennaPositions, antennaIndices, FarField, beamType = 'tiedArrayBeam'):
        # Develop this to calculate the pulse height in a proper way!
        (tiedArrayBeam, incoherentBeam, ccBeam) = self.getAllBeams(azel_in, fftData, antennaPositions, antennaIndices, FarField)
        
        if beamType == 'tiedArrayBeam':        
            tiedArrayBeam.abs() # make absolute value!
    #        hRunningAverage(smoothedBeam, tiedArrayBeam, 5, hWEIGHTS.GAUSSIAN)
            value = - tiedArrayBeam.max()[0] # just the maximum. 
        elif beamType == 'incoherentBeam':
            value = - incoherentBeam.max()[0]
        elif beamType == 'ccBeam':
            value = - ccBeam.max()[0]
        else:
            return 1.0e9
        print ' value = %f ' % value
        return value
        
            
                    
  #      efield = beamformed_efield.toNumpy()
  #      plt.clf()
  #      plt.plot(efield.T / blocksize)
  #      plt.show()
#        beamformed_efield.abs() # make absolute value!
#        hRunningAverage(beamformed_efield_smoothed, beamformed_efield, 5, hWEIGHTS.GAUSSIAN)
        # make smoother by convoluting with a 5-tap gaussian window

  #       smoothedstuff = beamformed_efield_smoothed.toNumpy()
  #      plt.clf()
  #      plt.plot(smoothedstuff.T / blocksize)
  #      plt.show()

#        erg = - beamformed_efield.max()[0] / blocksize # just the maximum of beamformed_efield_smoothed !!!!
#        print ' value = %f ' % erg
        
        
         
        """Initializes the object.

        Required arguments:
        
        ========= =================
        Parameter Description
        ========= =================
        *a*       does nothing
        *b*       also does nothing
        ========= =================

        Optional arguments:
        
        ========= =======================
        Parameter Description
        ========= =======================
        *c*       [True (default)| False]
        ========= =======================

        """



# Execute doctests if module is executed as script
if __name__ == "__main__":
    import doctest
    doctest.testmod()

  #     test code for indexing arrays
  #      shifted_fft[range(0, nofAntennas, 2), ...].addto(beamformed_fft_even)
  #      shifted_fft[range(1, nofAntennas, 2), ...].addto(beamformed_fft_odd)
        
  #      beamformed_fft_odd.addto(beamformed_fft_test)
  #      beamformed_fft_even.addto(beamformed_fft_test)
        
  #      beamformed_fft_test2.subadd(beamformed_fft_test, beamformed_fft)
  #      beamformed_fft_test2.pprint()
