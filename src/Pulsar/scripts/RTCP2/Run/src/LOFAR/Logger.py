import sys
import os
from math import modf
from time import time,strftime,localtime
import logging
from logging.handlers import TimedRotatingFileHandler
from traceback import format_exception
from itertools import count

DEBUG=False

def my_excepthook( etype, value, tb ):
  """ Replacement for default exception handler, which uses the logger instead of stderr. """

  lines = format_exception( etype, value, tb )

  for l in lines:
    for m in l.split("\n")[:-1]:
      logging.critical( m )

def initLogger():
  if DEBUG:
    minloglevel = logging.DEBUG
  else:
    minloglevel = logging.INFO

  logging.basicConfig( level = minloglevel,
                       format = "OLAP %(asctime)s.%(msecs)03d %(levelname)-5s %(message)s",
                       datefmt = "%d-%m-%y %H:%M:%S",
                   )

  logging.raiseExceptions = False                 

  loglevels = {
   "DEBUG":  logging.DEBUG,
   "INFO":   logging.INFO,
   "WARN":   logging.WARNING,
   "ERROR":  logging.ERROR,
   "FATAL":  logging.CRITICAL
  }

  for name,level in loglevels.iteritems():
    logging.addLevelName( level, name )

  sys.excepthook = my_excepthook  

class TimedSizeRotatingFileHandler(TimedRotatingFileHandler):
  """
    Rolls over both at midnight and after the log has reached a certain size.
  """
  def __init__(self, filename):
    TimedRotatingFileHandler.__init__( self, filename, when = "midnight", interval = 1, backupCount = 0 )

    self.maxBytes = 1024*1024*1024
    self.fileCount = 1

  def shouldRollover(self, record):
    if TimedRotatingFileHandler.shouldRollover(self, record):
      return 1

    msg = "%s\n" % self.format(record)

    if self.stream.tell() + len(msg) >= self.maxBytes:
      return 1

    return 0  

  def doRollover(self):
    def rename(f,t):
      if not os.path.exists(f):
        return

      if os.path.exists(t):
        os.remove(t)
      os.rename(f,t)  

    t = self.rolloverAt - self.interval
    timeTuple = localtime(t)

    base = self.baseFilename + "." + strftime(self.suffix, timeTuple)

    if os.path.exists( base ):
      # increment the suffix number of older logfiles, since TimedRotatingFileHandler
      # will overwrite "base"
      for b in xrange( self.fileCount, -1, -1 ):
        if b == 0:
          sfn = base
        else:  
          sfn = base + "." + str(b)

        dfn = base + "." + str(b+1)

        rename( sfn, dfn )

      self.fileCount += 1  
    else:
      self.fileCount = 1

    TimedRotatingFileHandler.doRollover(self)

def rotatingLogger( appname, filename ):
  logger = logging.getLogger( appname )

  handler = TimedSizeRotatingFileHandler( filename )

  logger.propagate = False
  logger.addHandler( handler )

  return logger
