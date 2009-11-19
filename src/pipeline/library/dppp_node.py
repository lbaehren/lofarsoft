#!/usr/bin/env python
from parset import Parset
import sys, os, time, logging, socket

# Set the logging level of the root logger.
logging.getLogger().setLevel(logging.DEBUG)

logging.info('Running on node %s', socket.gethostname())

# Add six empty arguments to handle missing arguments. 
sys.argv.extend(6*[''])
try:
    # The first four arguments are dummy.
    (seqnr, msn, lroot, psn, wd, dry) = sys.argv[5:11]
except ValueError:
    print 'usage:', os.path.basename(sys.argv[0]), 'dummy dummy dummy dummy', \
          'rank ms-part lofarroot parset-file wd dry'
    sys.exit(1)

# Show input arguments
logging.debug('seqnr = %s', seqnr)
logging.debug('msn   = %s', msn)
logging.debug('lroot = %s', lroot)
logging.debug('psn   = %s', psn)
logging.debug('wd    = %s', wd)
logging.debug('dry   = %s', dry)

# Input MS; strip off '.vds' until IDPPP can handle VDS files.
msin = os.path.splitext(msn)[0]
logging.info('Input MS: %s', msin)

if msin == "":
    logging.warning("No Measurement set given")
    sys.exit(0)

if not os.path.exists(msin):
    logging.error("Input MS %s does not exist", msin)
    sys.exit(1)

# Output MS; will be stored in working directory using same name as input MS.
msout = os.path.join(wd,os.path.split(msin)[1])
logging.info('Output MS: %s', msout)

if msin == msout:
    logging.error("Input MS and output MS cannot be the same")
    sys.exit(1)

# Create working dir if it doesn't exist and change to it.
if wd:
    if not os.path.exists(wd):
        os.makedirs(wd)
    os.chdir(wd)
wd = os.getcwd()
logging.info('Current working directory: %s', wd)

# Parset filename for the current run.
parsetfile = 'CS1_IDPPP.%02d.parset' % int(seqnr)

# Clean up working directory
os.system('rm -rf %s %s' % (msout, parsetfile))
logging.info('Removed %s, and %s' % (msout, parsetfile))

# Create a parset file for the current run
logging.info("Creating parset-file")
try:
    parset  = Parset(psn)
    parset['msin'] = msin
    parset['msout'] = msout
    parset.writeToFile(parsetfile)
except IOError, e:
    logging.error('Failed to open parset-file: %s' % e)
    sys.exit(1)

# Start IDPPP
logging.info('Start processing: %s', msn)
os.system(os.path.join(lroot, 'bin/CS1_IDPPP') + ' ' + parsetfile + ' 0')
logging.info('CS1_IDPPP finished')
