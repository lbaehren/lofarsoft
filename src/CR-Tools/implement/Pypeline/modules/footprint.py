#
#  footprint.py
#  
#
#  Created by Arthur Corstanje on 10/20/10.
#  Copyright (c) 2010 __MyCompanyName__. All rights reserved.
#

from pycrtools import *
import IO
import numpy as np
import matplotlib.pyplot as plt

def plotFootprint(datafile, footprintValues = [], dotsize = 200.0):
    """
    Plot an antenna layout based on 'datafile', with optional footprint values on top (e.g. pulse heights, signal power, etc.)
    In: datafile: the output of an IO.open(filename) call. This is needed for the call to ["RelativeAntennaPositions"].
        footprintValues: list or array of values per antenna. Normalization is done below.

    Can be modified to not do the plotting but return relevant arrays instead.    
    """
    antennaPositions = datafile["RelativeAntennaPositions"]
    nofAntennas = datafile["nofAntennas"]

    antennaPositions = antennaPositions.toNumpy()   
    antennaPositions = np.reshape(antennaPositions, (nofAntennas, 3)).T

    x = antennaPositions[0]
    y = antennaPositions[1]
    # assume z == 0, valid for LOFAR stations in their own reference frame. Besides, the plot is 2-D anyway.
    
    plt.scatter(x, y, 100, marker='+')

    if len(footprintValues) > 0:
        oddevensep = np.ones(len(x)) * 0.5 # want to see difference between odd/even channels
        oddevensep[0:-1:2] = -0.5 # even numbers get negative offset
        x += oddevensep
  
        s = dotsize * np.array(footprintValues) / max(footprintValues)
        
        plt.scatter(x, y, s, marker='o', c='r')
        #plt.show()

