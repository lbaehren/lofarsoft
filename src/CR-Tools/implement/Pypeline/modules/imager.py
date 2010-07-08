"""This module implements the imager for LOFAR TBB data.
"""

from ttl import beamforming
import pycrtools
import numpy as np

class Imager():
    """Imager for LOFAR TBB data."""

    def __init__(self, frequencies):
        """Default constructor."""

        # Set frequencies
        self.frequencies = frequencies

    def __str__(self):
        """Return string representation of the parameters."""
        return ''

    def initGrid(self):
        pass
#        azrange=(0.,365.,5)
#        elrange=(0.,95.,5)
#
#        # Calculate number of pixels
#        n_az=int((azrange[1]-azrange[0])/azrange[2])
#        n_el=int((elrange[1]-elrange[0])/elrange[2])
#        n_pixels=n_az*n_el
#
#        for el in np.arange (*elrange):
#            for az in np.arange(*azrange):
#                azel[n,0]=az
#                azel[n,1]=el
#                azel[n,2]= 1
#                n+=1

    def process(self, datablock):
        """Process datablock."""
        return np.zeros(shape=(100, 100), dtype='float64')

## Tests
if __name__=='__main__':
    import doctest
    doctest.testmod()

