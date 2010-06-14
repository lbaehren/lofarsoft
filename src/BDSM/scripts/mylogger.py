
""" Define the logger here. 
WARNING, ERROR and CRITICAL are output to screen. These and DEBUG and INFO go to 
the log file. INFO has lots of information. INFO also goes to screen if log_notes is True in opts."""

#Define a level NOTES (25) in between INFO and WARNING which goes to screen if log_notes is True in opts."""

import logging
from socket import gethostname
import commands
import time

log_notes = True  # &^$%!&#$&^#$&&^

hostname = gethostname()
dum, user = commands.getstatusoutput('whoami') 
tdum = time.strftime("%d-%m-%Y_%H.%M.%S")
logfilename = "log_PyBDSM_"+user+"_"+hostname+"_"+tdum


logger = logging.getLogger("PyBDSM")
logger.setLevel(logging.DEBUG)
#logging.addLevelName(25, "NOTES")

fh = logging.FileHandler(logfilename)
fh.setLevel(logging.DEBUG)
fmt1 = logging.Formatter('PyBDSM>  %(asctime)s %(name)s:: %(levelname)s: %(message)s', datefmt='%a %d-%m-%Y %H:%M:%S')
fh.setFormatter(fmt1)
logger.addHandler(fh)

ch = logging.StreamHandler()
#logging.addLevelName(25, "NOTES")
if log_notes:
  ch.setLevel(logging.INFO)
else:
  ch.setLevel(logging.WARNING)
fmt2 = logging.Formatter('PyBDSM log>  %(name)s:: %(levelname)s: %(message)s')
ch.setFormatter(fmt2)
logger.addHandler(ch)

lnotes = 25

