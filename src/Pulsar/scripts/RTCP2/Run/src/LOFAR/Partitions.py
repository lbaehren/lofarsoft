#!/usr/bin/env python

__all__ = [ "PartitionPsets","owner","runningJob","killJobs","freePartition","allocatePartition"]

import os
import sys

# allow ../util to be found, a bit of a hack
sys.path += [os.path.abspath(os.path.dirname(__file__)+"/..")]

from util.Commands import SyncCommand

# PartitionPsets:	A dict which maps partitions to I/O node IP addresses.
# the pset hierarchy is is analogue to:
# R00-M0-N00-J00 = R00-M0-N00-J00-16 consists of a single pset
# R00-M0-N00-32  = R00-M0-N00-J00 + R00-M0-N00-J01
# R00-M0-N00-64  = R00-M0-N00-32  + R00-M0-N01-32
# R00-M0-N00-128 = R00-M0-N00-64  + R00-M0-N02-64
# R00-M0-N00-256 = R00-M0-N00-128 + R00-M0-N04-128
# R00-M0         = R00-M0-N00-256 + R00-M0-N08-256
# R00            = R00-M0 + R00-M1

PartitionPsets = {}
for R in xrange(3):
  rack = "R%02d" % R
  for M in xrange(2):
    midplane = "%s-M%01d" % (rack,M)

    # individual psets
    for N in xrange(16):
       nodecard = "%s-N%02d" % (midplane,N)
       for J in xrange(2):
         # ip address for this pset
         ip = "10.170.%d.%d" % (R,(1+M*128+N*4+J))

         pset = "%s-J%02d" % (nodecard,J)
         if R == 0: PartitionPsets[pset] = [ip] # single psets without -16 suffix only work on R00
         PartitionPsets["%s-16" % (pset,)] = [ip]

    # groups smaller than a midplane
    for groupsize in (1,2,4,8):
      for N in xrange(0,16,groupsize):
        nodecard = "%s-N%02d" % (midplane,N)

        PartitionPsets["%s-%d" % (nodecard,32*groupsize)] = sum( [
          PartitionPsets["%s-N%02d-J00-16" % (midplane,x)] + PartitionPsets["%s-N%02d-J01-16" % (midplane,x)]
         for x in xrange( N, N+groupsize) ], [] )

    # a midplane
    PartitionPsets[midplane] = PartitionPsets["%s-N00-256" % midplane] + PartitionPsets["%s-N08-256" % midplane]

  # a rack
  PartitionPsets[rack] = PartitionPsets["%s-M0" % rack] + PartitionPsets["%s-M1" % rack]

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

def killJobs( partition ):
  """ Kill anything running on the partition. """
  return SyncCommand( "%s | /usr/bin/grep %s | /usr/bin/awk '{ print $1; }' | /usr/bin/xargs -r bgkilljob" % (BGJOBS,partition,) ).isSuccess()

def freePartition( partition ):
  """ Free the given partition. """
  return SyncCommand( "mpirun -partition %s -free wait" % (partition,) ).isSuccess()

def resetPartition( partition ):
  """ Reset /dev/flatmem on all I/O nodes and kill all processes that we started. """
  success = True

  for node in PartitionPsets[partition]:
    success = success and SyncCommand( "ssh -tq %s pkill IONProc ; pkill orted ; echo 1 > /proc/flatmem_reset" % (node,) ).isSuccess()

  return success  

def allocatePartition( partition ):
  """ Allocate the given partition by running Hello World. """
  return SyncCommand( "mpirun -partition %s -nofree -exe /bgsys/tools/hello" % (partition,), ["/dev/null"] ).isSuccess()

def stealPartition( partition ):
  """ Take over a partition from another user. UNDER CONSTRUCTION. """
  old_owner = owner( partition )
  new_owner = os.environ["USER"]

  if old_owner is None:
    # we already own it
    return allocatePartition( partition )

  jobinfo = runningJob( partition )

  if jobinfo is not None:
    # someone is still running a job
    return False

  # reallocate partition
  commands = [
    "set_username %s" % (old_owner,),
    "free %s" % (partition,),
    "set_username %s" % (new_owner,),
    "allocate %s" % (partition),
  ]
  return SyncCommand("ssh bgsn echo '%s' | mmcs_db_console" % "\\n".join(commands) ).isSuccess()

if __name__ == "__main__":
  from optparse import OptionParser,OptionGroup
  import sys

  # parse the command line
  parser = OptionParser( "usage: %prog [options] partition" )
  parser.add_option( "-l", "--list",
  			dest = "list",
			action = "store_true",
			default = False,
  			help = "list the psets in the partition" )
  parser.add_option( "-c", "--check",
  			dest = "check",
			action = "store_true",
			default = False,
  			help = "check the partition status" )
  parser.add_option( "-k", "--kill",
  			dest = "kill",
			action = "store_true",
			default = False,
  			help = "kill all jobs running on the partition" )
  parser.add_option( "-a", "--allocate",
  			dest = "allocate",
			action = "store_true",
			default = False,
  			help = "allocate the partition" )
  parser.add_option( "-f", "--free",
  			dest = "free",
			action = "store_true",
			default = False,
  			help = "free the partition" )
  parser.add_option( "-r", "--reset",
  			dest = "reset",
			action = "store_true",
			default = False,
  			help = "reset the partition without freeing it" )
  parser.add_option( "-s", "--steal",
  			dest = "steal",
			action = "store_true",
			default = False,
  			help = "take over a partition from another user (needs access to bgsn)" )

  # parse arguments
  (options, args) = parser.parse_args()
  errorOccurred = False

  if not args:
    parser.print_help()
    sys.exit(0)

  for partition in args:
    assert partition in PartitionPsets,"Partition unknown: %s" % (partition,)

    if options.list:
      # print the psets of a single partition
      for ip in PartitionPsets[partition]:
        print ip

    if options.kill and not errorOccurred:
      print "Killing jobs on %s..." % ( partition, )
      errorOccured = killJobs( partition )

    if options.free and not errorOccurred:
      print "Freeing %s..." % ( partition, )
      errorOccured = freePartition( partition )

    if options.allocate and not errorOccurred:
      print "Allocating %s..." % ( partition, )
      errorOccured = allocatePartition( partition )

    if options.reset and not errorOccurred:
      print "Resetting %s..." % ( partition, )
      errorOccured = resetPartition( partition )

    if options.steal and not errorOccurred:
      print "Taking over partition %s..." % ( partition, )
      errorOccured = stealPartition( partition )

    # check partition if requested so
    if options.check:
      expected_owner = os.environ["USER"]
      real_owner = owner( partition )

      print "Partition Owner : %-40s" % (real_owner,)

      expected_job = None
      real_job = runningJob( partition )

      print "Running Job     : %-40s" % (real_job,)

    sys.exit(int(errorOccurred))

