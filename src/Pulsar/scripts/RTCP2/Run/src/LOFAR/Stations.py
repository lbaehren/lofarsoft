#!/usr/bin/env python

from Partitions import PartitionPsets
import os
import sys
from Locations import Locations

# allow ../util to be found, a bit of a hack
sys.path += [(os.path.dirname(__file__) or ".")+"/.."]

from util.Commands import SyncCommand,backquote

__all__ = ["packetAnalysis","stationInPartition","Stations","overrideRack"]

def overrideRack( stations, rack ):
  """ Set the rack that will be used (0,1,2) for all stations provided. """

  assert rack in [0,1,2]

  for s in stations:
    for t in stations[s]:
      octets = t.ionode.split(".")
      t.ionode = "%s.%s.%s.%s" % (octets[0],octets[1],rack,octets[3])

def packetAnalysis( name, ip, port ):
  # locate packetanalysis binary, since its location differs per usage, mainly because
  # nobody runs these scripts from an installed environment
  locations = map( os.path.abspath, [
    "%s/../packetanalysis"    % os.path.dirname(__file__), # when running straight from a source tree
    "%s/../../packetanalysis" % os.path.dirname(__file__), # when running in an installed environment
    "%s/../../build/gnu/src/packetanalysis" % os.path.dirname(__file__), # when running straight from a source tree

    "/globalhome/mol/projects/LOFAR/RTCP/Run/src/packetanalysis", # fallback: Jan David's version
  ] )
  
  location = None
  for l in locations:
    if os.path.exists( l ):
      location = l
      break

  if location is None:
    return "ERROR: Could not find `packetanalysis' binary"

  mainAnalysis = backquote( "ssh -tq %s %s %s" % (ip,location,port), 5)

  if not mainAnalysis or " 0.00 pps" in mainAnalysis:
    # something went wrong -- don't run tcpdump
    return mainAnalysis

  # do a tcpdump analysis to obtain source mac address
  """ tcpdump: The following information will be received from stations:

08:30:23.175116 10:fa:00:01:01:01 > 00:14:5e:7d:19:71, ethertype IPv4 (0x0800), length 6974: 10.159.1.2.4347 > 10.170.0.1.4347: UDP, length 6928
  """
  tcpdump = backquote("ssh -q %s /opt/lofar/bin/tcpdump -i eth0 -c 10 -e -n udp 2>/dev/null" % (ip,), 2).split("\n")
  macaddress = "UNKNOWN"
  for p in tcpdump:
    if not p: continue

    try:
      f = p.split()
      srcmac = f[1]
      dstip = f[-4]

      dstip,dstport = dstip[:-1].rsplit(".",1)
      if dstport != port:
        continue
    except ValueError:
      continue

    macaddress = srcmac

  if macaddress in [
    "00:12:f2:c3:3a:00", # Effelsberg
  ]:
    macline = " OK Source MAC address:  %s (known router)" % (macaddress,)
  elif macaddress == "UNKNOWN" or not macaddress.startswith("00:22:86:"):
    macline = "NOK Source MAC address:  %s (no LOFAR station)" % (macaddress,)
  else:
    rscs,nr,field = name[0:2],name[2:5],name[5:]
    nr1,nr2,nr3 = nr

    # the station number is the 4th and the 5th field of the MAC address, encoded in hex per two digits (i.e. 302 => 03:02 and 032 => 00:20)
    macnrs = map( lambda x: int(x,16), macaddress.split(":") )
    srcnr = "%01d%02d" % (macnrs[3],macnrs[4])

    try:
      if str(nr) == srcnr:
        macline = " OK Source MAC address:  %s" % (macaddress,)
      else:
        macline = "NOK Source MAC address:  %s (station %d?)" % (macaddress,srcnr)
    except ValueError:    
        macline = "NOK Source MAC address:  %s" % (macaddress,)
    
  return "%s\n%s" % (macline,mainAnalysis) 

def allInputs( station ):
  """ Generates a list of name,ip,port tuples for all inputs for a certain station. """

  for name,input,ionode in sum( ([(s.name,i,s.ionode)] for s in station for i in s.inputs), [] ):
    # skip non-network inputs
    if input == "null:":
      continue

    if input.startswith( "file:" ):
      continue

    # strip tcp:
    if input.startswith( "tcp:" ):
      input = input[4:]

    # only process ip:port combinations
    if ":" in input:
      ip,port = input.split(":")
      if ip in ["0.0.0.0","0"]:
        ip = ionode
      yield (name,ip,port)

def stationInPartition( station, partition ):
  """ Returns a list of stations that are not received within the given partition.
  
      Returns (True,[]) if the station is received correctly.
      Returns (False,missingInputs) if some inputs are missing, where missingInputs is a list of (name,ip:port) pairs.
  """

  notfound = []

  for name,ip,port in allInputs( station ):  
    if ip not in PartitionPsets[partition]:
      notfound.append( (name,"%s:%s" % (ip,port)) )
    
  return (not notfound, notfound)	


class UnknownStationError(StandardError):
    pass

class Station(object):
  """
  Represents a real or virtual station.
  """
  def __init__(self, name, ionode, inputs):
    self.name   = name
    self.ionode	= ionode
    self.inputs	= inputs

  def getPsetIndex(self, partition):
    assert partition in PartitionPsets, "Unknown partition: %s" % (partition,)

    psets = PartitionPsets[partition]

    assert self.ionode in psets, "IONode %s not in partition %s" % (self.ionode,partition)

    return psets.index(self.ionode)

  def getName(self):
    return self.name

class Stations(dict):
  def __init__(self):
     self.stations = {}

  def parse( self, str ):
    """ Parse a string defining a set of stations, such as 'CS001 + CS010'. """

    # use no globals to avoid eval adding "__builtins__" to self, and thus as a station
    try:
      return eval( str, {}, self )  
    except Exception,e:
      raise UnknownStationError("%s (while parsing '%s')" % (e,str))

  def parseAll( self ):
    """ Parse all station definitions that were given as a string and replace them with Station lists. """

    changed = True

    # not everything parses in one go due to interdependencies
    while changed:
      changed = False
      for k,v in self.items():
        if isinstance( v, str ):
          try:
            self[k] = self.parse( v )
          except UnknownStationError:
             continue
          else:
             changed = True

    # reparse all strings left to trigger any exceptions that are left
    for k,v in self.items():
      if isinstance( v, str ):
        self[k] = self.parse( v )

def defineStations( s ):
  # Actual, physical stations. """

  def ports( portnrs ):
    return ['0.0.0.0:%s' % (port,) for port in portnrs]
  def boards( boardnrs ):
    return ['0.0.0.0:%s' % (4346+board,) for board in boardnrs]
    
  # default port numbers
  defaultPorts = {
    "RSP_0": boards( [0, 1, 2, 3] ),
    "RSP_1": boards( [6, 7, 8, 9] ),
  }
  defaultFields = {
    "RSP_0": ["LBA","HBA","HBA0"],
    "RSP_1": ["HBA1"],
  }

  # TODO: reolsve only after Locations.files have been finalised
  # adter processing command-line parameters
  configdir = Locations.resolvePath( Locations.files["configdir"] )

  # parse hostname <-> ip translation, since not all hostnames
  # are ionodes (foreign stations send to a router)
  knownhosts = {}
  for l in file("%s/MAC+IP.dat" % (configdir,)):
    l = l.split("#")[0].strip() # strip comments and whitespace

    info = l.split()
    if len(info) < 3: continue # need HOSTNAME IP MAC

    hostname,ip,mac = info[:3]
    knownhosts[hostname] = ip

  # see MAC/Deployment/data/StaticMetaData/RSPConnections.dat
  # for a mapping of station -> io node
  # and LOFAR/Stations.py -l for a mapping of io node -> ip address 

  # parse RSPConnections.dat.
  for l in file("%s/RSPConnections.dat" % (configdir,)):
    l = l.split("#")[0].strip() # strip comments and whitespace

    info = l.split()
    if len(info) < 3: continue # need at least NAME BOARD IONODE

    station,board,ionode = info[:3]

    ip = knownhosts[ionode]
    portlist = defaultPorts[board]
    fieldlist = defaultFields[board]

    for f in fieldlist:
      s.update( {
        "%s%s" % (station,f): [Station("%s%s" % (station,f), ip, portlist )],
      } )

  # to change the ports of CS302HBA0 for example, use
  # s["CS302HBA0"][0].inputs = boards( [0, 1, 2, 4] )

  # Simulated stations for experimentation.

  # IP suffixes of all psets
  psetsuffixes = [1,2,5,6,9,10,13,14,17,18,21,22,25,26,29,30,33,34,37,38,41,42,45,46,49,50,53,54,57,58,61,62,129,130,133,134,137,138,141,142,145,146,149,150,153,154,157,158,161,162,165,166,169,170,173,174,177,178,181,182,185,186,189,190]

  for suffix in psetsuffixes:
     # Rack R00
     ip = "10.170.0.%s" % (suffix,)

     # sXX_1, sXX: 1 full station (1 RSP board), starting from 10.170.0.XX, input received from station
     inputs = ports( [4346] )
     s["s%s_1" % (suffix,)] = [Station("S%s" % (suffix,), ip, inputs)]
     s["s%s"   % (suffix,)] = [Station("S%s" % (suffix,), ip, inputs)]

     # SXX_1, SXX: 1 full station (4 RSP boards), starting from 10.170.0.XX, input received from station
     inputs = defaultPorts['RSP_0']
     s["S%s_1" % (suffix,)] = [Station("S%s" % (suffix,), ip, inputs)]
     s["S%s"   % (suffix,)] = [Station("S%s" % (suffix,), ip, inputs)]

     # Rack R01
     ip = "10.170.1.%s" % (suffix,)

     # tXX_1: 1 full station (1 RSP board), starting from 10.170.1.XX, input received from station
     inputs = ports( [4346] )
     s["t%s_1" % (suffix,)] = [Station("T%s" % (suffix,), ip, inputs)]

     # TXX_1: 1 full station (4 RSP boards), starting from 10.170.1.XX, input received from station
     inputs = defaultPorts['RSP_0']
     s["T%s_1" % (suffix,)] = [Station("T%s" % (suffix,), ip, inputs)]
     s["T%s"   % (suffix,)] = [Station("T%s" % (suffix,), ip, inputs)]

  # define sets of various sizes 
  for setsize in xrange(2,65):
     for first_index in xrange( 0, len(psetsuffixes)-setsize+1 ):
       suffixes = psetsuffixes[first_index:first_index+setsize]

       for prefix in ["s","S","t","T"]:
         s["%s%s_%s" % (prefix,suffixes[0],setsize)] = sum( (s["%s%s_1" % (prefix,suffix,)] for suffix in suffixes), [] )

  # Special stations, one-time stations, etc.
  s.update( {
   "Pulsar": [Station('Pulsar', '10.170.0.30', ['0.0.0.0:4346'])],
   "twoears":  [Station('CS302HBA0', '10.170.0.133', ['0.0.0.0:4346']),
                Station('CS302HBA1', '10.170.0.134', ['0.0.0.0:4347'])],
  } )

  # Standard configurations
  s.update( {
  } )

# Construct everything

Stations = Stations()
defineStations( Stations )
Stations.parseAll()

if __name__ == "__main__":
  from optparse import OptionParser,OptionGroup
  import sys

  # parse the command line
  parser = OptionParser( "usage: %prog [options] station" )
  parser.add_option( "-P", "--partition",
  			dest = "partition",
                        type = "string",
			default = "R00",
  			help = "the partition to use [%default]" )
  parser.add_option( "-i", "--inputs",
  			dest = "inputs",
			action = "store_true",
			default = False,
  			help = "list the inputs for the station" )
  parser.add_option( "-c", "--check",
  			dest = "check",
			action = "store_true",
			default = False,
  			help = "check whether the station provide correct data" )
  parser.add_option( "-a", "--analyze",
  			dest = "analyze",
			action = "store_true",
			default = False,
  			help = "run John's analyzer (assumes 200 MHz clock, 61 subbands, 16 beamlets)" )
  parser.add_option( "-l", "--list",
  			dest = "list",
			action = "store_true",
			default = False,
  			help = "list all known stations" )

  # parse arguments
  (options, args) = parser.parse_args()
  errorOccurred = False
  
  if options.list:
    print "\n".join(sorted(Stations.keys()))
    sys.exit(0)

  if not args:
    parser.print_help()
    sys.exit(0)

  overrideRack( Stations, int(options.partition[2]) )

  for stationName in args:
    # print the inputs of a single station
    if options.inputs:
      for s in Stations[stationName]:
        print "Station:     %s" % (s.name,)
        print "IONode:      %s" % (s.ionode,)
        print "Inputs:      %s" % (" ".join(s.inputs),)

    # check stations if requested so
    if options.check:
      if stationName not in Stations:
        # unknown station
        errorOccurred = True
        print "NOK Station name unknown: %s" % (stationName,)
        continue

      for name,ip,port in allInputs( Stations[stationName] ):
        print "---- Packet analysis for %s %s:%s" % (name,ip,port)
        print packetAnalysis( name, ip, port )

    if options.analyze:
      # ssh 10.170.0.182 "echo 0.0.0.0:4346 0.0.0.0:4347 0.0.0.0:4348 0.0.0.0:4349 | xargs -n 1 -P 4 /globalhome/romein/bin.ppc/analyzer" 2>&1 | awk '{ print "DE602LBA ",$0; }'
      s = Stations[stationName][0]
      os.system('ssh %s "echo %s | xargs -n 1 -P 4 /globalhome/romein/bin.ppc/analyzer" 2>&1 | awk \'{ print "%s",$0; }\'' % (s.ionode, " ".join(s.inputs), s.name))

  sys.exit(int(errorOccurred))
