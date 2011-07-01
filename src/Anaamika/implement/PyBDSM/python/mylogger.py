""" WARNING, ERROR and CRITICAL are always output to screen and to log file.
INFO and USERINFO always go to the log file. DEBUG goes to log file if debug is True.
USERINFO goes to screen only if quiet is False.

Use as follows:

mylog = mylogger.logging.getLogger("name")

mylog.info('info') --> print to logfile, but not to screen
mylog.userinfo(mylog, 'info') --> prints to screen (if quiet==False)
                                  and to logfile
"""
import logging
from socket import gethostname
import commands
import time

def init_logger(logfilename, quiet=False, debug=False):
  logging.USERINFO = logging.INFO + 1
  logging.addLevelName(logging.USERINFO, 'USERINFO')
  logger = logging.getLogger("PyBDSM")
  logger.setLevel(logging.DEBUG)

  # First remove any existing handlers (in case PyBDSM has been run
  # before in this session but the quiet or debug options have changed
  while len(logger.handlers) > 0:
    logger.removeHandler(logger.handlers[0])
  
  # File handler
  fh = logging.FileHandler(logfilename)
  if debug:
    # For log file and debug on, print name and levelname
    fh.setLevel(logging.DEBUG)
    fmt1 = logging.Formatter('%(asctime)s %(name)-20s:: %(levelname)-8s: %(message)s',
                             datefmt='%a %d-%m-%Y %H:%M:%S')
  else:
    # For log file and debug off, don't print name and levelname as
    # they have no meaning to the user.
    fh.setLevel(logging.INFO)
    fmt1 = logging.Formatter('%(asctime)s:: %(message)s',
                             datefmt='%a %d-%m-%Y %H:%M:%S')
  fh.setFormatter(fmt1)
  logger.addHandler(fh)

  # Console handler for warning and critical: format includes levelname
  ch = logging.StreamHandler()
  ch.setLevel(logging.WARNING)
  fmt2 = logging.Formatter('\033[31;1m%(levelname)s\033[0m: %(message)s')
  ch.setFormatter(fmt2)
  logger.addHandler(ch)

  # Console handler for USERINFO only: format does not include levelname
  # (the user does not need to see the levelname, as it has no meaning to them)
  chi = logging.StreamHandler()
  chi.addFilter(InfoFilter())
  if quiet:
    # prints nothing, since filter lets only USERINFO through
    chi.setLevel(logging.WARNING)
  else:
    # prints only USERINFO
    chi.setLevel(logging.USERINFO)
  fmt3 = logging.Formatter('%(message)s')
  chi.setFormatter(fmt3)
  logger.addHandler(chi)  

class InfoFilter(logging.Filter):
  # Lets only USERINFO through
  def filter(self, rec):
    return rec.levelno == logging.USERINFO
 
 
def userinfo(mylog, desc_str, val_str=''):
  """Writes a nicely formatted string to the log file and console

  mylog = logger
  desc_str = description string / message
  val_str = value string, printed in blue

  Message is constructed as:
    'desc_str : val_str'
  """
  if val_str == '':
    sep = ''
    desc_str = '--> ' + desc_str
  else:
    sep = ' : '
    if len(desc_str) < 40:
      desc_str += ' '
    if len(desc_str) < 40:
      while len(desc_str) < 41:
        desc_str += '.'
    else:
      while len(desc_str) < 41:
        desc_str += ' '       
  mylog.log(logging.USERINFO, desc_str+sep+val_str)
