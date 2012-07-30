#!/usr/bin/env python

print "WARNING: BGcontrol.py has been depricated. Use Stations.py or Partitions.py instead."

import os
from Partitions import PartitionPsets
from Stations import Stations
import sys

# allow ../util to be found, a bit of a hack
sys.path += [os.path.dirname(__file__)+"/.."]

from util.Commands import SyncCommand,backquote

__all__ = ["owner","runningJob","packetAnalysis","stationInPartition","killJobs","freePartition","allocatePartition"]

"""
  Reports about the status of the BlueGene. An interface to the bg* scripts, and to check
  what data is being received on the BlueGene from the stations.
"""

# Locations of the bg* scripts
BGBUSY = "/usr/local/bin/bgbusy"
BGJOBS = "/usr/local/bin/bgjobs"

def owner( partition ):
  """ Returns the name of the owner of the partition, or None if the partition is not allocated. """

  cmd = "%s" % (BGBUSY,)

  for l in os.popen( cmd, "r" ).readlines():
    try:
      part,nodes,cores,state,owner,net = l.split()
    except ValueError:
      continue

    if part == partition:  
      # partition found     
      return owner
  
  # partition is not allocated
  return None

def runningJob( partition ):
  """ Returns a (jobId,name) tuple of the job running on the partition, or None if no job is running. """

  cmd = "%s" % (BGJOBS,)

  for l in os.popen( cmd, "r" ).readlines():
    try:
      job,part,mode,executable,user,state,queue,limit,wall = l.split()
    except ValueError:
      continue

    if part == partition: 
      # job found 
      return (job,executable)
  
  # partition is not allocated or has no job running
  return None

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

  mainAnalysis = backquote( "ssh -tq %s %s %s" % (ip,location,port) )

  # do a tcpdump analysis to obtain source mac address
  """ tcpdump: The following information will be received from stations:

08:30:23.175116 10:fa:00:01:01:01 > 00:14:5e:7d:19:71, ethertype IPv4 (0x0800), length 6974: 10.159.1.2.4347 > 10.170.0.1.4347: UDP, length 6928
  """
  tcpdump = backquote("ssh -q %s /opt/lofar/bin/tcpdump -i eth0 -c 10 -e -n udp 2>/dev/null" % (ip,)).split("\n")
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

    macnrs = macaddress.split(":")
    srcnr = "%s%s" % (macnrs[3],macnrs[4])

    if int(nr) == int(srcnr):
      macline = " OK Source MAC address:  %s" % (macaddress,)
    else:
      macline = "NOK Source MAC address:  %s (station %d?)" % (macaddress,int(srcnr))
    
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

def killJobs( partition ):
  """ Kill anything running on the partition. """
  return SyncCommand( "%s | /usr/bin/grep %s | /usr/bin/awk '{ print $1; }' | /usr/bin/xargs -r bgkilljob" % (BGJOBS,partition,) ).isSuccess()


def freePartition( partition ):
  """ Free the given partition. """
  return SyncCommand( "mpirun -partition %s -free wait" % (partition,) ).isSuccess()

def allocatePartition( partition ):
  """ Allocate the given partition by running Hello World. """
  return SyncCommand( "mpirun -partition %s -nofree -exe /bgsys/tools/hello" % (partition,), ["/dev/null"] ).isSuccess()

if __name__ == "__main__":
  from optparse import OptionParser,OptionGroup
  import sys

  def okstr( q ):
    return ["NOK","OK"][int(bool(q))]

  # parse the command line
  parser = OptionParser( """usage: %prog [options]

  Typical invocations:
    %prog -c   -S CS302LBA               Checks input from station CS302LBA
    %prog -kfa -P R00                    Kills jobs, frees and allocates partition R00
  """)
  parser.add_option( "-q", "--quiet",
  			dest = "quiet",
			action = "store_true",
			default = False,
  			help = "output less" )
  parser.add_option( "-c", "--check",
  			dest = "check",
			action = "store_true",
			default = False,
  			help = "check whether a certain station or partition is ok" )

  pargroup = OptionGroup(parser, "Partition" )
  pargroup.add_option( "-k", "--kill",
  			dest = "kill",
			action = "store_true",
			default = False,
  			help = "kill all jobs running on the partition" )
  pargroup.add_option( "-a", "--allocate",
  			dest = "allocate",
			action = "store_true",
			default = False,
  			help = "allocate the partition" )
  pargroup.add_option( "-f", "--free",
  			dest = "free",
			action = "store_true",
			default = False,
  			help = "free the partition" )
  parser.add_option_group( pargroup )

  hwgroup = OptionGroup(parser, "Hardware" )
  hwgroup.add_option( "-S", "--stations",
  			dest = "stations",
			action = "append",
			type = "string",
  			help = "the station(s) to use [%default]" )
  hwgroup.add_option( "-P", "--partition",
  			dest = "partition",
			type = "string",
  			help = "name of the BlueGene partition [%default]" )
  parser.add_option_group( hwgroup )

  # parse arguments
  (options, args) = parser.parse_args()
  errorOccurred = False

  if options.partition:
    assert options.partition in PartitionPsets

    if options.kill and not errorOccurred:
      if not options.quiet: print "Killing jobs on %s..." % ( options.partition, )
      errorOccured = killJobs( options.partition )

    if options.free and not errorOccurred:
      if not options.quiet: print "Freeing %s..." % ( options.partition, )
      errorOccured = freePartition( options.partition )

    if options.allocate and not errorOccurred:
      if not options.quiet: print "Allocating %s..." % ( options.partition, )
      errorOccured = allocatePartition( options.partition )

    # check partition if requested so
    if options.check:
      expected_owner = os.environ["USER"]
      real_owner = owner( options.partition )

      print "Partition Owner : %-40s %s" % (real_owner,okstr(real_owner == expected_owner))

      expected_job = None
      real_job = runningJob( options.partition )

      print "Running Job     : %-40s %s" % (real_job,okstr(real_job == expected_job))

    sys.exit(int(errorOccurred))

  # check stations if requested so
  if options.stations:
    if options.check:
      for stationName in options.stations:
        if stationName not in Stations:
          # unknown station
          errorOccurred = True
          print "NOK Station name unknown: %s" % (stationName,)
          continue

        for name,ip,port in allInputs( Stations[stationName] ):
          print "---- Packet analysis for %s %s:%s" % (name,ip,port)
          print packetAnalysis( name, ip, port )

    sys.exit(int(errorOccurred))
   
  parser.print_help()
  sys.exit(0)


