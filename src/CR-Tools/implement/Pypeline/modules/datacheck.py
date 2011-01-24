""" 
  Module datacheck.py. Part of the LOFAR Cosmic Ray pipeline.
  Opens a CR event data file and performs elementary checks to ensure stability further down the pipeline.
  TODO: Provides methods for checking of data quality, flagging of antennas.
  
.. moduleauthor:: Arthur Corstanje <A.Corstanje@astro.ru.nl>
"""

import os
import time
from pycrtools import IO, CRQualityCheck

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
        return result.update(reason = "File doesn't exist")
    bytesize = os.path.getsize(filename.strip())
    if bytesize < 15 * 1048576:
        print 'File size too small: %d' % bytesize
        return result.update(reason = format("File size too small: %d") % bytesize)

    crfile = IO.open([filename.strip()])
    crfile.setAntennaset(antennaset)

    fileDate = crfile["Date"]
    readableDate = time.strftime("%a, %d %b %Y %H:%M:%S", time.gmtime(fileDate))
    result.update(date = readableDate)
    print result

    times = crfile["TIME"]
  #  print crfile["shift"]
    if times.max() - times.min() > 0:
        print 'Error: Timestamps vary across antennas' # redundant
        return result.update(reason=format("Timestamps don't match, spread = %d seconds") % (dates.max() - dates.min()) )
        
    if crfile["sampleFrequency"] != 200e6: 
    # MOVE to integrity check
        print 'Error: wrong sampling frequency: %d; needs to be 200e6' % crfile["sampleFrequency"]
        return result.update(reason=format("wrong sampling frequency %d") % crfile["sampleFrequency"])
    
    if crfile["Filesize"] < 2 * 65536:
        return result.update(reason=format("file size too small: %d") % crfile["Filesize"])
    result.update(datalength = crfile["Filesize"])    
    if crfile["nofAntennas"] < 64: # arbitrary choice...
        return result.update(reason=format("not enough antennas: %d") % crfile["nofAntennas"])
    result.update(nofAntennas = crfile["nofAntennas"])

    result.update(success=True, file=crfile)
    return result
#    return result.update(success=True, file=crfile) !!! This actually returns None (nonetype)...
    
def qualityCheck(crfile):
    qualitycriteria={"mean":(-1.5,1.5),"rms":(4,15),"spikyness":(-5,5)}
    # BUG: works only with one file at a time. 'crfile' has no attribute 'filename' because it can be one or many...
#    datalength = crfile["Filesize"]
#    blocksize = 1024
#    blocklist = hArray(int, [0.5 * datalength / blocksize])
    flaglist=CRQualityCheck(qualitycriteria, crfile.files[0], dataarray=None, maxblocksize=65536, nsigma=5, verbose=True) 
    # make warnings for DC offsets and spikyness; flag out for rms too high (i.e. junk data)
    flagged = []
    highDCOffsets = 0
    spikies = 0
    #import pdb; pdb.set_trace()
    for entry in flaglist:
        if entry[3][0] == 'mean': 
            highDCOffsets += 1
        elif entry[3][0] == 'spikyness':
            spikies += 1
        elif entry[3][0] == 'rms':
            flagged.append(entry[0]) # entry[0] = antenna nr.
            
    result = dict(success=True, action = 'Data quality check', flagged = flagged, 
                  warnings = format("Too high DC offsets: %d; too high spikyness: %d") % (highDCOffsets, spikies) )
    return result
    
# Execute doctests if module is executed as script
if __name__ == "__main__":
    import doctest
    doctest.testmod()

