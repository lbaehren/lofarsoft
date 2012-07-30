#!/usr/bin/python

import Logger
from logging import debug,info,warning,error,critical
import Sections
from util import Commands
from Locations import Locations
from CommandClient import sendCommand
from ObservationID import ObservationID
from Parset import Parset
from Stations import Stations,overrideRack
from util.dateutil import format
import sys
import signal
from threading import Lock
import thread
import socket

DRYRUN = False

aborted = False
lock = Lock()
lock.acquire() # lock can be released by anyone to signal the end of the run

def installSigHandlers():
  """ Translate signals to KeyboardInterrupts to catch them in a try block. """

  def sigHandler( sig, frame ):
    global aborted,lock

    critical( "Caught signal %s -- aborting" % (sig,) )
    aborted = True

    try:
      lock.release()
    except thread.error:
      pass

  signal.signal( signal.SIGTERM, sigHandler )
  signal.signal( signal.SIGQUIT, sigHandler )
  signal.signal( signal.SIGINT, sigHandler )

def runCorrelator( partition, start_cnproc = True, start_ionproc = True ):
  """ Run an observation using the provided parsets. """

  # ----- Select the sections to start
  sections = Sections.SectionSet()

  if start_ionproc:
    sections += [Sections.IONProcSection( partition )]
  if start_cnproc:
    sections += [Sections.CNProcSection( partition )]

  # sanity check on sections
  if not DRYRUN:
    sections.check()

  installSigHandlers()  

  # ----- Run all sections
  try:
    # start all sections
    sections.run()
    sections.wait( lock )

    if aborted:
      raise Exception("aborted")

  except Exception,e:
    error( "%s", e )

    try:
      # soft abort -- wait for all observations to stop
      sendCommand( partition, "cancel all" )
      sendCommand( partition, "quit" )
    except: 
      # hard abort -- kill all sections
      sections.abort()

  # let the sections clean up 
  sections.postProcess()

def buildParset( parset = None, args = "", olapparset = "OLAP.parset", partition = None ):
  """
    Adjust and augment the keys of a parset for use in OLAP.

    parset:         if defined, a parset object which already contains the keys read from the parset files
    args:           a string of arguments (parset=RTCP.parset,start=+60, etc, see the __main__ function below)
    olapparset:     the filename of OLAP.parset (contains station positions and delays)
    partition:      the BG/P partition that will be used (if none, the partition has to be specified in the parset)
  """

  # valid observation parameters
  validObsParams = [
    "parset", "output", "stations", "tcp", "null",
    "start", "stop", "run",
    "clock", "integration",
    "nostorage"
  ]

  def isValidObsParam( key ):
    return key in validObsParams or "." in key

  # default observation parameters

  defaultObsParams = {
    "parset": "RTCP.parset",
    "start": "+15",
    "output": Locations.files["parset"],
  }
  defaultRunTime = "00:01:00"

  # define no default run or stop time in defaultObsParams,
  # otherwise it will always overrule the parset
  assert "stop" not in defaultObsParams
  assert "run" not in defaultObsParams

  # always assume the stations will be sending to the rack we will be using
  if partition:
    overrideRack( Stations, int(partition[2]) )

  # ========== Observations
  obs = args
  info( "===== Parsing observation %s =====" % (obs,) )

  # parse and check the observation parameters
  def splitparam( s ):
    """ Convert a parameter which is either 'key=value' or 'key' into a
        key,value tuple. """

    if "=" in s:
      return s.split("=",1)

    return (s,None)

  obsparams = defaultObsParams.copy()
  obsparams.update( dict( map( splitparam, obs.split(",") ) ) )

  for p in obsparams:
    if p and not isValidObsParam( p ):
      critical("Unknown observation parameter '%s'" % (p,))

  if parset is None:
    parset = Parset()

    if "parset" not in obsparams:
      critical("Observation '%s' does not define a parset file." % (obs,))

    # read the parset file
    def addParset( f ):
      info( "Reading parset %s..." % (f,) )
      parset.readFile( Locations.resolvePath( f ) )

    try:
      addParset( olapparset )
      addParset( obsparams["parset"] )  
    except IOError,e:
      critical("Cannot read parset file: %s" % (e,))

  # parse specific parset values from the command line (all options containing ".")
  # apply them so that we will parse them instead of the parset values
  for k,v in obsparams.iteritems():
    if "." not in k:
      continue

    parset.parse( "%s=%s" % (k,v) )

  # reserve an observation id
  parset.postRead()

  if parset.getObsID():
    info( "Distilled observation ID %s from parset." % (parset.getObsID(),) )
  else:  
    try:
      obsid = ObservationID().generateID()
    except IOError,e:
      critical("Could not generate observation ID: %s" % (e,))

    parset.setObsID( obsid )

    info( "Generated observation ID %s" % (obsid,) )

  # override parset with command-line values
  if partition:
    parset.setPartition( partition )
  else:
    parset.setPartition( parset.distillPartition() )

  # set stations
  if "stations" in obsparams:
    stationList = Stations.parse( obsparams["stations"] )

    parset.forceStations( stationList )
  else:
    stationList = parset.stations

  if "tcp" in obsparams:
    # turn all inputs into tcp:*
    def tcpify( input ):
      if input.startswith("tcp:") or input.startswith("file:"):
        return input
      elif input.startswith("udp:"):
        return "tcp:"+input[4:]
      else:
        return "tcp:"+input

    for s in stationList:
      s.inputs = map( tcpify, s.inputs )

  if "null" in obsparams:
    # turn all inputs into null:
    for s in stationList:
      s.inputs = ["null:"] * len(s.inputs)

  parset.setStations( stationList )

  if "nostorage" in obsparams:
    parset.disableStorage()

  # set runtime
  if "start" in obsparams and "stop" in obsparams:
    parset.setStartStopTime( obsparams["start"], obsparams["stop"] )
  elif "start" in obsparams and "run" in obsparams:
    parset.setStartRunTime( obsparams["start"], obsparams["run"] )
  elif "Observation.startTime" in parset and "Observation.stopTime" in parset:
    parset.setStartStopTime( parset["Observation.startTime"], parset["Observation.stopTime"] )
  else:  
    parset.setStartRunTime( obsparams["start"], defaultRunTime )

  info( "Running from %s to %s." % (parset["Observation.startTime"], parset["Observation.stopTime"] ) )
  info( "Partition     = %s" % (parset.partition,) )
  info( "Storage Nodes = %s" % (", ".join(parset.storagenodes),) )
  info( "Stations      = %s" % (", ".join([s.name for s in parset.stations]),) )

  # set a few other options
  configmap = {
    "clock": parset.setClock,
    "integration": parset.setIntegrationTime,
  }

  for k,v in configmap.iteritems():
    if k in obsparams:
      v( obsparams[k] )

  # reapply them in case they got overwritten
  for k,v in obsparams.iteritems():
    if "." not in k:
      continue

    parset.parse( "%s=%s" % (k,v) )

  parset.setFilename( Locations.resolvePath( obsparams["output"], parset ) )

  return parset
