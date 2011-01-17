""" 
  Module datacheck.py. Part of the LOFAR Cosmic Ray pipeline.
  Opens a CR event data file and performs elementary checks to ensure stability further down the pipeline.
  TODO: Provides methods for checking of data quality, flagging of antennas.
  
.. moduleauthor:: Arthur Corstanje <A.Corstanje@astro.ru.nl>
"""

from pycrtools import IO

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
    # TODO test system filesize first
    crfile = IO.open([filename])
    crfile.setAntennaset(antennaset)

    dates = crfile["TIME"]
  #  print crfile["shift"]
    if dates.max() - dates.min() > 0:
        print 'Error: Timestamps vary across antennas' # redundant
        return dict(success=False, reason=format("Timestamps don't match, spread = %d seconds") % (dates.max() - dates.min()) )
        
    if crfile["sampleFrequency"] != 200e6: 
    # MOVE to integrity check
        print 'Error: wrong sampling frequency: %d; needs to be 200e6' % crfile["sampleFrequency"]
        return dict(success=False, reason=format("wrong sampling frequency %d") % crfile["sampleFrequency"])
    
    if crfile["Filesize"] < 2 * 65536:
        return dict(success=False, reason=format("file size too small: %d") % crfile["Filesize"])
        
    if crfile["nofAntennas"] < 64: # arbitrary choice...
        return dict(success=False, reason=format("not enough antennas: %d") % crfile["nofAntennas"])

    return dict(success=True, file=crfile)
    


# Execute doctests if module is executed as script
if __name__ == "__main__":
    import doctest
    doctest.testmod()

