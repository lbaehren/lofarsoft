#! /usr/bin/env python

import pycrtools as cr

## =============================================================================
##  Initialisation
## =============================================================================

datafile = cr.LOFARSOFT+"/data/aera/ad001037.f0001"


## =============================================================================
##  Opening the data file
## =============================================================================

dr = cr.hAERAFileOpen(datafile)


## =============================================================================
##  Summaries of the data file
## =============================================================================

# File summary
dr.fileSummary()

# Event summary
dr.eventSummary()

# Local station summary
dr.localStationSummary()


## =============================================================================
##  Processing the data file
## =============================================================================


eventStatus = dr.firstEvent()
while (True == eventStatus): # Loop over events
    # Operations per event
    eventID = dr.getAttribute("eventid")

    print "Event ID : %d" %(eventID)

    localstationStatus = dr.firstLocalStation()
    while (True == localstationStatus): # Loop over local stations
        # Operations per local station
        localstationID = dr.getAttribute("localstationid")

        print "Local station ID : %d" %(localstationID)
        localstationStatus = dr.nextLocalStation()

    eventStatus = dr.nextEvent()


## =============================================================================
##  Obtaining ADC data for the first local station of the first event
## =============================================================================

dr.firstEvent()
dr.firstLocalStation()

tracelength = dr.getAttribute("tracelength")
a = cr.hArray(int, [4,tracelength])
cr.hAERAGetADCData(dr, a)


## =============================================================================
##  Closing the data file
## =============================================================================

dr.close()
