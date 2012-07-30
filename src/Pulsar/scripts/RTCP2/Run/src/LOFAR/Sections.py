#!/usr/bin/env python

from util.Commands import SyncCommand,AsyncCommand,mpikill,backquote,PIPE
from util.Aborter import runUntilSuccess,runFunc
from Locations import Locations,Hosts,isProduction
import Logger
import os
import Partitions
import ObservationID
import time
from logging import debug,info,warning
from threading import Thread,Lock
import thread

DEBUG=False
DRYRUN=False
VALGRIND_ION=False
VALGRIND_STORAGE=False

SSH="ssh -o StrictHostKeyChecking=no -q "

class Section:
  """ A 'section' is a set of commands which together perform a certain function. """

  def __init__(self, partition):
    self.commands = []

    self.logoutputs = []
    if Locations.nodes["logserver"]:
      self.logoutputs.append( "%s" % (Locations.nodes["logserver"],) )

    self.partition = partition
    self.psets     = Partitions.PartitionPsets[self.partition]

    self.preProcess()

  def __str__(self):
    return self.__class__.__name__

  def preProcess(self):
    pass

  def run(self):
    pass

  def postProcess(self):
    pass

  def killSequence(self,name,killfunc,timeout):
    killfuncs = [ lambda: killfunc(2), lambda: killfunc(9) ]

    success = runUntilSuccess( killfuncs, timeout )

    if not success:
      warning( "%s: Could not kill %s" % (self,name) )

    return success

  def killCommand(self,cmd,timeout):
    def kill( signal ):
      cmd.abort( signal )
      cmd.wait()

    return self.killSequence(str(cmd),kill,timeout)

  def abort(self,timeout):
    for c in self.commands:
      self.killCommand(c,timeout)

  def wait(self):
    for s in self.commands:
      s.wait()

  def check(self):
    pass

class SectionSet(list):
  def run(self):
    for s in self:
      info( "Starting %s." % (s,) )
      s.run()

  def postProcess(self):
    for s in self:
      info( "Post processing %s." % (s,) )
      s.postProcess()


  def abort(self, timeout=5):
    for s in self:
      info( "Killing %s." % (s,) )
      s.abort(timeout)

  def wait(self, lock = Lock()):
    """ Wait until the sections finish, or until the lock is released. Releases lock when section finishes. """

    sections = self

    # wait in a separate thread to allow python to capture KeyboardInterrupts in the main thread
    class WaitThread(Thread):
      def run(self):
        try:
          for s in sections:
            info( "Waiting for %s." % (s,) )
            s.wait()
        finally:
          try:
            lock.release()  
          except thread.error:
            # lock already released
            pass

    WaitThread().start() 

    # wait for lock to be released by waiting thread or from an external source
    # !! DO NOT use lock.acquire() since that will delay signals (SIGQUIT/SIGTERM/etc) from arriving until
    # the lock is acquired.
    while lock.locked():
      time.sleep(1)

  def check(self):
    for s in self:
      info( "Checking %s for validity." % (s,) )
      s.check()

class CNProcSection(Section):
  def run(self):
    logger = Logger.rotatingLogger( "CNProc", "%s/run.CNProc.log" % (Locations.files["logdir"]) )
    logfiles = self.logoutputs

    # CNProc is started on the Blue Gene, which has BG/P mpirun 1.65
    # NOTE: This mpirun needs either stdin or stdout to be line buffered,
    # otherwise it will not provide output from CN_Processing (why?).
    mpiparams = [
      # where
      "-mode VN",
      "-partition %s" % (self.partition,),

      # environment
      "-env DCMF_COLLECTIVES=0",
      "-env BG_MAPPING=XYZT",
      "-env LD_LIBRARY_PATH=/bgsys/drivers/ppcfloor/comm/lib:/bgsys/drivers/ppcfloor/runtime/SPI:/globalhome/romein/lib.bgp",

      # working directory
      "-cwd %s" % (Locations.files["rundir"],),

      # executable
      "-exe %s" % (Locations.files["cnproc"],),

      # arguments
    ]

    self.commands.append( AsyncCommand( "mpirun %s" % (" ".join(mpiparams),), logfiles, killcmd=mpikill, logger=logger ) )

class IONProcSection(Section):
  def run(self):
    logger = Logger.rotatingLogger( "IONProc", "%s/run.IONProc.log" % (Locations.files["logdir"]) )
    logfiles = self.logoutputs

    if VALGRIND_ION:
      valgrind = "/globalhome/mol/root-ppc/bin/valgrind --suppressions=%s --leak-check=full --show-reachable=no" % (Locations.files["ionsuppfile"],)
    else:
      valgrind = ""

    mpiparams = [
      # where
      "-host %s" % (",".join(self.psets),),
      "--pernode",

      # "--tag-output",

      # environment
      # "-x FOO=BAR",

      # working directory
      "-wd %s" % (Locations.files["rundir"],),

      # valgrind
      valgrind,

      # executable
      "%s" % (Locations.files["ionproc"],),

      # arguments
      "%d" % (int(isProduction()),),
    ]


    if DEBUG:
      mpiparams = [
        "-v",
      ] + mpiparams

    self.commands.append( AsyncCommand( "/bgsys/LOFAR/openmpi-ion/bin/mpirun %s" % (" ".join(mpiparams),), logfiles, logger=logger ) )

  def check(self):
    # I/O nodes need to be reachable -- check in parallel

    # ping

    pingcmds = [
      (node,AsyncCommand( "ping %s -c 1 -w 2 -q" % (node,), ["/dev/null"] ))
      for node in self.psets
    ]

    for (node,c) in pingcmds:
      assert c.isSuccess(), "Cannot reach I/O node %s [ping]" % (node,)

    # ssh & flatmem access

    # cat /dev/flatmem returns "Invalid argument" if the memory is available,
    # and "Cannot allocate memory" if not.
    successStr = "cat: /dev/flatmem: Invalid argument"
    sshcmds = [
      (node,AsyncCommand( SSH+"%s cat /dev/flatmem 2>&1 | grep -F '%s'" % (node,successStr),PIPE) )
      for node in self.psets
    ]

    def waitForSuccess():
      for (node,c) in sshcmds:
        c.wait()

        assert successStr in c.output(), "Cannot allocate flat memory on I/O node %s" % (node,)

    assert runFunc( waitForSuccess, 20 ), "Failed to reach one or more I/O nodes [ssh]"
