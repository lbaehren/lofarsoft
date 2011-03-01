""" 
  Module datacheck.py. Part of the LOFAR Cosmic Ray pipeline.
  Opens a CR event data file and performs elementary checks to ensure stability further down the pipeline.
  Provides methods for checking of data quality, flagging of antennas.
  
.. moduleauthor:: Arthur Corstanje <A.Corstanje@astro.ru.nl>
"""

import os
import time
from pycrtools import IO, CRQualityCheck
import matplotlib.pyplot as plt

def safeOpenFile(filename, antennaset): # antennaset only here because it's not set in the file
    """ Opens a file and performs some safety / consistency checks

    Required arguments:
    
    =========    ====================
    Parameter    Description
    =========    ====================
    *filename*   Name of the file to open 
    *antennaset* antenna set descriptor, e.g. LBA_OUTER
    =========    ====================

    Returns:
    
    dictionary with keys:
    *success*                   Boolean
    [if not success] *reason*   String
    [else] *file*               opened file object
    
    """
    result = dict(success=False, name = filename)
    # test system exists / filesize first
    if not os.path.exists(filename.strip()):
        print 'file not found'
        result.update(reason = "File doesn't exist")
        return result
    bytesize = os.path.getsize(filename.strip())
    if bytesize < 15 * 1048576:
        print 'File size too small: %d' % bytesize
        result.update(reason = format("File size too small: %d") % bytesize)
        return result

    crfile = IO.open([filename.strip()])
    crfile.set("blocksize", 2*65536)
    crfile.setAntennaset(antennaset)

    fileDate = crfile["Date"]
    readableDate = time.strftime("%a, %d %b %Y %H:%M:%S", time.gmtime(fileDate))
    result.update(date = readableDate)
    #print result

    times = crfile["TIME"]
  #  print crfile["shift"]
    if times.max() - times.min() > 0:
        print 'Error: Timestamps vary across antennas' # redundant
        result.update(reason=format("Timestamps don't match, spread = %d seconds") % (dates.max() - dates.min()) )
        return result
        
    if crfile["sampleFrequency"] != 200e6: 
        print 'Error: wrong sampling frequency: %d; needs to be 200e6' % crfile["sampleFrequency"]
        result.update(reason=format("wrong sampling frequency %d") % crfile["sampleFrequency"])
        return result
    
    if crfile["Filesize"] < 2 * 65536:
        result.update(reason=format("Data length too small: %d") % crfile["Filesize"])
        return result
    result.update(datalength = crfile["Filesize"])    
    if crfile["nofAntennas"] < 64: # arbitrary choice...
        result.update(reason=format("not enough antennas: %d") % crfile["nofAntennas"])
        return result
    result.update(nofAntennas = crfile["nofAntennas"])
    
    # get all timeseries data
    cr_alldata = crfile["emptyFx"]
    crfile.getTimeseriesData(cr_alldata, 0) 
    
    result.update(success=True, file=crfile, efield = cr_alldata)
    return result
#    return result.update(success=True, file=crfile) !!! This actually returns None (nonetype)...
    
def qualityCheck(crfile, doPlot=False):
    if doPlot:
        cr_efield = crfile["emptyFx"]
        crfile.getTimeseriesData(cr_efield, 0) # crfile["Fx"] crashes on invalid block number ???? While it was set to a valid value...
        efield = cr_efield.toNumpy()
        plt.plot(efield.T)
        raw_input("--- Plotted raw timeseries data - press Enter to continue...")

    qualitycriteria={"mean":(-1.5,1.5),"rms":(3,15),"spikyness":(-5,5)}
    # BUG: works only with one file at a time. 'crfile' has no attribute 'filename' because it can be one or many...
#    datalength = crfile["Filesize"]
#    blocksize = 1024
#    blocklist = hArray(int, [0.5 * datalength / blocksize])
    flaglist=CRQualityCheck(qualitycriteria, crfile.files[0], dataarray=None, maxblocksize=2*65536, nsigma=5, verbose=True)  # NOTE: this changes the crfile's blocksize! Change it back afterwards.
    crfile.set("blocksize", 2 * 65536) # parameter...
    # make warnings for DC offsets and spikyness; flag out for rms too high (i.e. junk data)
    flagged = []
    highDCOffsets = 0
    spikyChannels = 0
    #import pdb; pdb.set_trace()
    for entry in flaglist:
        if entry[3][0] == 'mean': 
            highDCOffsets += 1
        elif entry[3][0] == 'spikyness':
            spikyChannels += 1
        elif entry[3][0] == 'rms':
            flagged.append(entry[0]) # entry[0] = antenna nr.
    
    if doPlot and len(flagged) > 0:
        cr_efield = crfile["emptyFx"]
        crfile.getTimeseriesData(cr_efield, 0) # crfile["Fx"] crashes on invalid block number ???? While it was set to a valid value...
        efield = cr_efield.toNumpy()
        toplot = efield[flagged]
        plt.plot(toplot.T)
        raw_input("--- Plotted antennas flagged as BAD - press Enter to continue...")
                            
    result = dict(success=True, action = 'Data quality check', flagged = flagged, 
                  warnings = format("Too high DC offsets: %d; too high spikyness: %d") % (highDCOffsets, spikyChannels) )
    return result
    
# Execute doctests if module is executed as script
if __name__ == "__main__":
    import doctest
    doctest.testmod()

