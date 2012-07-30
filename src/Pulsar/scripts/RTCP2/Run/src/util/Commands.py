import os
import fcntl
import socket
from subprocess import Popen,STDOUT,call,PIPE
from Hosts import ropen
from tee import Tee
from Aborter import runFunc


DRYRUN = False

# define our own PIPE as an alias to subprocess.PIPE
PIPE=PIPE

def backquote( cmdline, timeout = 0 ):
  """ Run a command line and return the output. """

  debug("RUN backquote: %s" % (cmdline,) )
  class process:
    def __init__(self):
      self.output = ""

    def run(self):
      self.output = Popen( cmdline.split(), stdout=PIPE, stderr=STDOUT ).communicate()[0]

  p = process()
  if timeout:
    runFunc( p.run, timeout )
  else:
    p.run()

  return p.output

def debug( str ):
  """ Override with custom logging function. """

  pass

def mpikill( pid, signal ):
  SyncCommand( "mpikill -s %s %s" % (signal,pid) )

class AsyncCommand(object):
    """
    Executes an external command in the background
    """

    def __init__(self, cmd, outfiles=[], infile=None, killcmd=os.kill, logger=None ):
        """ Run command `cmd', with I/O optionally redirected.

            cmd:      command to execute.
            outfiles: filenames for output, or [] to use stdout, or PIPE to record it within python.
            infile:   filename for input, or None to use stdin.
            killcmd:  function used to abort, called as killcmd( pid, signal ). """

        if DRYRUN:
          self.cmd = "echo %s" % (cmd,)
        else:
          self.cmd = cmd

        if outfiles not in [[],PIPE]:
          outstr = "> %s" % (", ".join(outfiles),)
        else:
          outstr = ""
        debug("RUN %s: %s %s" % (self.__class__.__name__,cmd,outstr) )

        if DRYRUN:
          self.outputs = []
          stdout = None # no logging
        elif outfiles == PIPE:
          self.outputs = []
          stdout = PIPE # no logging
        else:
          # open all outputs, remember them to prevent the files
          # from being closed at the end of __init__
          self.outputs = [ropen( o, "w", 1 ) for o in outfiles]

          # create a pipe to multiplex everything through
          r,w = os.pipe()

          # feed all file descriptors to Tee
          Tee( r, self.outputs, logger )

          # keep the pipe input
          stdout = w

        if infile:
          # Line buffer stdin to satisfy MPIRUN on the BG/P. It will not provide output without it.
          stdin = ropen( infile, "w", 1 )
        else:
          stdin = None

        self.done = False
        self.reaped = False
        self.success = False
        self.killcmd = killcmd
        self.popen = Popen( self.cmd.split(), stdin=stdin, stdout=stdout, stderr=STDOUT )
        self.run()

    def __str__(self):
        return "(%s) %s" % (self.__class__.__name__,self.cmd)

    def run(self):
        """ Will be called when the command has just been started. """

        pass

    def output(self):
        """ Return the output of the program (when started with outfiles="PIPE"). """

        output = self.popen.communicate()[0] or ""

        # even though process closed stdout, we still need to wait for termination
        self.wait()

        return output

    def isDone(self):
        """ Return whether the command has finished execution, but do not block. """

        self.done = (self.popen.poll() is not None)

        return self.done

    def wait(self):
        """ Block until the command finishes execution. """

        if self.reaped:
          # already wait()ed before
          return

        try:
          self.success = self.popen.wait() == 0
        except OSError:
          # process died prematurely or never started?
          self.success = False

        self.done = True
        self.reaped = True

        for o in self.outputs:
          o.close()

    def abort(self, signal = 2):
        """ Abort the command, if it has not finished yet. """

        if not self.isDone():
          debug( "Sending signal %s to PID %s" % (signal,self.popen.pid) )

          self.killcmd( self.popen.pid, signal )

    def isSuccess(self):
        """ Returns whether the command returned succesfully. """

        self.wait()
        return self.success

class SyncCommand(AsyncCommand):
    """
    Executes an external command immediately.
    """

    def run(self):
      self.wait()

