## This is a template Python module.
#  Use this template for each module in the Pypeline.
#  Note that a script is not the same as a module, the difference can
#  already be seen by the #! on the first line.
#  For scripts (e.g. files that you want to execute from the commandline)
#  you should use scripts/template.py instead.

"""This module provides function to do (incoherent) dedispersion.

"""

## Imports
#  Only import modules that are actually needed and avoid
#  "from module import *" as much as possible to prevent name clashes.

import numpy as np
#import matplotlib.pyplot as plt
#import pycrtools as hf

## Examples
class ExampleClass():
    """Class documentation"""

    def __init__(self):
        """Constructor"""

        weights_filename=LOFARSOFT+'/src/CR-tools/data/ppfWeights16384.dat'
# Initialize arrays and values
        weights=hf.hArray(float,[16,1024])
        weights.readdump(weights_filename)

        if weights[0,0].val()==0.0:
            print "Obtaining Kaiser coefficient from file"
            # Reading of weights failed
            f=open(LOFARSOFT+'/src/CR-Tools/data/Coeffs16384Kaiser-quant.dat')
            weights.setDim([16*1024])
            f.seek(0)
            for i in range(0,16*1024):
                weights[i]=float(f.readline())
            weights.setDim([16,1024])
        
        buffer=hf.hArray(float,[16,1024])
        startrow=15
        total_rows_added=0

    def add(input):
        self.buffer[self.startrow].copy(input)
        input.fill(0)
        for row in range(0,16):
            input.muladd(weights[row],self.buffer[(row+self.startrow)%16])
        
        total_rows_added+=1
        self.startrow-=1
        if total_row_added < 16:
            input.fill(0)
            return false
        
        return true

        



def freq_to_delay(DM, freq, timeresolution=1):
    """This is a docstring, each function should have one.

    *DM* Dispersion measure in cm / pc^3
    *freq* Numpy vector of frequencies in Hz
    *timeresolution* Time resolution in seconds (default 1 sec)
    Examples (also for doctests):
    Returns list of delays in units of the provided timeresolution

    >>> example_function(1,2)
    (1, 2)
    """
    
    ref_freq_sqr=np.square(min(freq)/1e9)
    freq_sqr=np.square(freq/1e9)
    offsets=1/freq_sqr-1/ref_freq_sqr
    offsets*=-1*4.15e-3*DM/timeresolution
    
    return offsets

def dedisperse_array(dynspec, DM, frequencies, timeresolution):
    """Dedipserse an array. Returns an array with dedispersed values
    of the same length as the timeaxis of the array. Data will be dedispersed
    to the original time axis of the lowest frequency.
     
    *dynspec*        Two dimensional numpy array. First axis frequency, 
                     second axis time
    *DM*             Dispersion for which to dedisperse the data (in cm/pc^3)
    *frequencies*    numpy array of frequency values of the frequency axis 
                     of the dynspec
    *timeresolution* Resolution of the time axis of the array

    returns array with dedispersed data values

    """
    

    offsets=freq_to_delay(DM,frequencies,timeresolution)

    (nrfreq,nrtime)=dynspec.shape

    nrtime-=max(offsets)
    
    #Check if data can be dispersed at all
    assert nrtime>0

    dedisp_data=np.zeros(nrtime,dtype=float)

    for freq in range(nrfreq):
        dedisp_data[0:nrtime]+=dynspec[freq,int(offsets[freq]):int(offsets[freq])+nrtime]

    return dedisp_data


## Executing a module should run doctests.
#  This examines the docstrings of a module and runs the examples
#  as a selftest for the module.
#  See http://docs.python.org/library/doctest.html for more information.
if __name__=='__main__':
    import doctest
    doctest.testmod()

