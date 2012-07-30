#!/usr/bin/env python

__all__ = ["reconnecting_socket","ropen","rmkdir","rexists","runlink","rsymlink"]

import Queue
import os
import sys
import subprocess
import socket
from threading import Thread
from time import sleep

HOSTNAME = os.environ.get("HOSTNAME")

def correct_hostname( host ):
  """ Translates the hostname such that bgfen can resolve it. """

  if host.startswith("locus") and "." not in host:
    return "%s.cep2.lofar" % (host,)

  return host  

class reconnecting_socket:
  """ A socket that keeps reconnecting if the connection is lost. Data is sent
      asynchronously, with a buffer which drops messages if full. """

  def __init__( self, host, port, retry_timeout=10, socket_timeout=5, bufsize=256 ):
    self.host = host
    self.port = port
    self.socket_timeout = socket_timeout
    self.retry_timeout = retry_timeout
    self.socket = None
    self.done = False

    self.writebuf = Queue.Queue( bufsize )

    self.iothread = Thread( target=self.iothread_main, name="I/O thread for %s:%s" % (host,port) )
    self.iothread.start()

  def iothread_main( self ):
    def close():
      self.socket.close()
      self.socket = None

    def reconnect():
      self.socket = socket.socket()
      self.socket.settimeout( self.socket_timeout )

      while not self.done:
        try:
          self.socket.connect( (self.host,self.port) )
        except socket.error:
          pass
        except socket.timeout:
          pass
        else:
          # connected!
          break

        # sleep, but do stop when told
        for i in xrange( self.retry_timeout ):
          if self.done:
            return
          sleep( 1 )

    def write( data ):
      if self.socket is None:
        reconnect()

      if self.done:
        return

      written = 0

      try:
        while written < len(data):
          written += self.socket.send( data[written:] )
      except socket.error:
        # do not attempt to send remaining data
        close()
        return
      except socket.timeout:
        # do not attempt to send remaining data
        close()
        return

    # start with a connection
    if self.socket is None:
      reconnect()

    while True:
      try:
        data = self.writebuf.get( timeout=1 )
      except Queue.Empty:
        # TODO: we can't keep a close check on our socket, delaying
        # closing and reconnecting and keeping the line open
        continue

      if data is None:
        # close request
        break

      write( data ) 

  def write( self, data ):
    if self.done:
      return

    try:
      self.writebuf.put_nowait( data )
    except Queue.Full:
      # queue full -- drop data
      pass

  def close( self ):
    self.done = True # abort any reconnection attempts
    self.writebuf.put( None ) # prod the deque, wait if necessary

    self.iothread.join()


def split( filename ):
  """ Internally used: split a filename into host,file. Host == '' if pointing to localhost. """

  if ":" not in filename:
    return "",filename
  else:
    host,file = filename.split(":",1)

    if host in ["localhost",HOSTNAME]:
      host = ""

    return host,file

def ropen( filename, mode = "r", buffering = -1 ):
  """ Open a local or a remote file for reading or writing. A remote file
      has the syntax host:filename or tcp:<ip>:<port>. """

  assert mode in "rwa", "Invalid mode: %s" % (mode,)

  host,file = split( filename )

  if host == "":
    # a local file
    if file == "-":
      if mode == "r":
        return sys.stdin
      else:
        return sys.stdout

    return open(file, mode, buffering)

  if host == "tcp":
    # create a TCP socket
    assert mode in "wa", "ropen: only modes w and a are supported when using tcp"

    ip,port = file.split(":")
    return reconnecting_socket( ip, int(port) )

  modelist = {
    "r": "cat %s" % (file,),
    "w": "cat - > %s" % (file,),
    "a": "cat - >> %s" % (file,),
  }

  primitive = {
    "r": "r",
    "w": "w",
    "a": "w",
  }

  assert mode in modelist, "Invalid mode: %s" % (mode,)

  if mode in "wa":
    # writing
    return subprocess.Popen( ["ssh",correct_hostname(host),modelist[mode]], bufsize=buffering, stdin=subprocess.PIPE ).stdin
  else:
    # reading
    return subprocess.Popen( ["ssh",correct_hostname(host),modelist[mode]], bufsize=buffering, stdout=subprocess.PIPE ).stdout

def rmkdir( dirname ):
  """ Make a local or a remote directory. A remote directory name
      has the syntax host:filename. """

  host,dir = split( dirname )

  if host == "":
    # a local file
    return os.path.exists( dir ) or os.mkdir( dir )

  # only create directory if it does not exist
  subprocess.call( ["ssh",host,"[ ! -e %s ] && mkdir %s" % (dir,dir)] )

def rexists( filename ):
  """ Checks for the availability of a local or a remote file. A remote
      file has the syntax host:filename. """

  host,file = split( filename )

  if host == "":
    # a local file
    return os.path.exists( file )

  return int(subprocess.Popen( ["ssh",correct_hostname(host),"[ ! -e %s ]; echo $?" % (file,)], stdout=subprocess.PIPE ).stdout.read()) == 1

def runlink( filename, recursive = False ):
  """ Deletes a local or a remote file. A remote
      file has the syntax host:filename. """

  host,file = split( filename )

  if host == "":
    # a local file
    return os.unlink( file )

  flags = "-f"

  if recursive:
    flags += " -r"

  return int(subprocess.Popen( ["ssh",correct_hostname(host),"rm %s -- '%s'" % (flags,file,)], stdout=subprocess.PIPE ).stdout.read()) == 1

def rsymlink( src, dest ):
  """ Create a symlink at src, pointing to dest.
      src/dest have the syntax host:filename, but dest should not point at a different host. """

  srchost,srcfile = split( src )    
  desthost,destfile = split( dest )    

  assert srchost == desthost, "rsymlink( %s, %s ) requires a link across machines" % (src,dest)

  if srchost == "":
    # a local file
    return os.symlink( dest, src )

  return int(subprocess.Popen( ["ssh",correct_hostname(srchost),"ln -s '%s' '%s'" % (destfile,srcfile,)], stdout=subprocess.PIPE ).stdout.read()) == 1

