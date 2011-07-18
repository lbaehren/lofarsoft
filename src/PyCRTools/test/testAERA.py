import pycrtools as cr
import os

datafile=os.environ["HOME"]+"/Development/Work/AERA/Datareader/data/ad001037.f0001"
dr = cr.hAERAFileOpen(datafile)

## =============================================================================
##
##  Test functions
##
## =============================================================================

def testFileSummary():
    """
    Testing of the file summary
    """
    print "testing File Summary..."
    dr.fileSummary()

def testEventSummary():
    """
    Testing of the event summary.
    """
    print "testing Event Summary..."
    dr.eventSummary()

def testLocalStationSummary():
    """
    Testing of the local station summary.
    """
    print "testing Local Station Summary..."
    dr.localStationSummary()

def testAERA():
    """
    Process list of tests.
    """
    testFileSummary()
    testEventSummary()
    testLocalStationSummary()


## =============================================================================
##
##  Main routine
##
## =============================================================================

if __name__ == '__main__':
    import doctest
    doctest.testmod()
    doctest
    testAERA()

