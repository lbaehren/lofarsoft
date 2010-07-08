"""This module implements the imager for LOFAR TBB data.
"""

import matplotlib.pyplot as plt
from ttl import beamforming
from ttl import coordinates
#import pycrtools
import numpy as np

class Imager():
    """Imager for LOFAR TBB data."""

    def __init__(self, frequencies, antpos):
        """Default constructor."""

        # Set parameters
        self.frequencies = frequencies
        self.antpos = antpos
        self.n_antennas = antpos.shape[0]

        # Initialize grid
        self.initGrid()

        # Calculate delays
        self.calculateDelays()

    def __str__(self):
        """Return string representation of the parameters."""
        return ''

    def initGrid(self):
        """Initialize a Az/El coordinate grid."""

        self.n_az = 50
        self.n_el = 50

        azrange = np.linspace(183.831-30,183.831+30,self.n_az)
        elrange = np.linspace(59.0699-30,59.0699+30,self.n_el)

        # Calculate number of pixels
        self.n_pixels = self.n_az*self.n_el

        self.azel = np.zeros(shape=(self.n_pixels, 3), dtype='float64')

        n=0
        for el in elrange:
            for az in azrange:
                self.azel[n,0] = az*np.pi/180.
                self.azel[n,1] = el*np.pi/180.
                self.azel[n,2] = 1
                n+=1

        # Convert to Cartesian
        self.cartesian = np.zeros_like(self.azel)
        coordinates.azElRadius2Cartesian(self.cartesian, self.azel, False) 

    def calculateDelays(self):
        """Calculate delays."""

        self.delays = np.zeros(shape=(self.n_pixels,self.n_antennas))
        beamforming.geometricDelays(self.delays, self.antpos, self.cartesian, True)

        # Check delays in meters
        #plt.hist(self.delays.ravel()*2.997e8,100)
        #plt.show()

    def process(self, data):
        """Process data."""

        # Setup arrays
        image = np.zeros(self.n_pixels)
        weights = np.zeros(shape=(self.frequencies.shape[0], self.delays.shape[1]), dtype='complex')

        # Loop over all pixels
        for i in range(self.n_pixels):
            print 'pixel:', i+1, 'of', self.n_pixels

            # Calculate complex weights
            beamforming.complexWeights(weights, self.delays[i], self.frequencies)

            # Apply weights
            image[i]=np.abs(np.sum(weights*data))

        return image.reshape(self.n_az, self.n_el)

## Tests
if __name__=='__main__':
    import doctest
    doctest.testmod()

