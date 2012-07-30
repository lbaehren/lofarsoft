#!/usr/bin/env python

from util.Hosts import ropen
from Locations import Locations

# do not modify any files if DRYRUN is True
DRYRUN = False

"""
  The following files exist to aid the creation of observation IDs:

  nextMSnumber	contains the next free observation ID (integer)
  MSList	contains a list of existing measurements and their start time [deprecated: not updated anymore]

  Their locations are stored in the Locations dictionary.
"""

class ObservationID:
  def __init__( self ):
    self.obsid = 0

  def generateID( self ):
    """ Returns an unique observation ID to use and reserve it. """

    if self.obsid:
      # already reserved an observation ID
      return self.obsid

    # read the next ms number
    f  = ropen( Locations.files["nextmsnumber"], "r" )
    obsid = int(f.readline())
    f.close()

    if not DRYRUN:
      # increment it and save
      f = ropen( Locations.files["nextmsnumber"], "w" )
      print >>f, "%s" % (obsid+1)
      f.close()

    self.obsid = obsid

    return self.obsid
