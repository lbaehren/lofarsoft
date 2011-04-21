#
#  footprint.py
#
#
#  Created by Arthur Corstanje on 10/20/10.
#  Copyright (c) 2010 __MyCompanyName__. All rights reserved.
#

from pycrtools import *
import numpy as np
import matplotlib.pyplot as plt

def plotFootprint(datafile, footprintValues = [], dotsize = 200.0):
    """
    Plot an antenna layout based on 'datafile', with optional footprint values on top (e.g. pulse heights, signal power, etc.)
    In: datafile: the output of an IO.open(filename) call. This is needed for the call to ["RelativeAntennaPositions"].
        footprintValues: list or array of values per antenna. Normalization is done below.

    Can be modified to not do the plotting but return relevant arrays instead.
    """
    datafile["ANTENNA_SET"] = "LBA_OUTER" # HACK !!
    antennaPositions = datafile["RELATIVEANTENNA_POSITIONS"]
    nofAntennas = datafile["NOF_DIPOLE_DATASETS"]

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

def footprintForCRdata(datafile, cr_efield, doPlot = False):
    result = dict(success=False, action = 'Footprint per antenna')

    nofAntennas = cr_efield.shape()[0]
    abs_efield = hArray(copy = cr_efield)
    abs_efield.abs()
    maxPerAntenna = hArray(abs_efield[...].max())
    
    summedPulseHeight = maxPerAntenna.sum()[0] # incoherently summed pulse height
    result.update(summedPulseHeight = summedPulseHeight)
    
    even = hArray(int, dimensions=[nofAntennas/2])
    even.fillrange(0, 2)
    odd = hArray(copy=even)
    odd.fillrange(1, 2)
    oddMax = maxPerAntenna[odd].max()[0]
    oddMin = maxPerAntenna[odd].min()[0]
    evenMax = maxPerAntenna[even].max()[0]
    evenMin = maxPerAntenna[even].min()[0]
    result.update(oddMax=oddMax, oddMin = oddMin, evenMax=evenMax, evenMin=evenMin)
    
    XY = np.zeros(nofAntennas/2)
    for i in range(0, nofAntennas, 2):
        if maxPerAntenna[i] > maxPerAntenna[i+1]:
            XY[i/2] = 1
#            print '%d: even > odd' % i
        else:
            XY[i/2] = 0
#            print '%d: odd > even' % i
    if max(XY) - min(XY) > 0.01:
        result.update(XYvariable=True)
    else:
        result.update(XYvariable=False)
    
#    plt.clf()
    if doPlot:
        plt.clf()
        plotFootprint(datafile, maxPerAntenna)
        raw_input("--- Plotted footprint of maximum abs value per antenna. Press enter to continue...")
        plt.clf()
    result.update(success=True)
    return result
#    for i in range(0, nofAntennas, 2):
#        if maxPerAntenna[i] > maxPerAntenna[i+1]:
#            print '%d: even > odd' % i
#        else:
#            print '%d: odd > even' % i
#    print 'even max = %f' % maxPerAntenna[even].max()[0]
#    print 'even min = %f' % maxPerAntenna[even].min()[0]
#    print 'odd  max = %f' % maxPerAntenna[odd].max()[0]
#    print 'odd  min = %f' % maxPerAntenna[odd].min()[0]
#    print ' Hele array even: '
#    print maxPerAntenna[even]
#    print ' odd: '
#    print maxPerAntenna[odd]
#    print ' '
