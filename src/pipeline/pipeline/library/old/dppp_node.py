#!/usr/bin/env python
#
#  dppp_node.py: script to run DPPP on a single node
#
#  Copyright (C) 2002-2008
#  ASTRON (Netherlands Foundation for Research in Astronomy)
#  P.O.Box 2, 7990 AA Dwingeloo, The Netherlands, seg@astron.nl
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#  $Id$

"""Script to run DPPP on a single node"""

import os, sys, logging, socket

# Set the logging level of the root logger.
logging.getLogger().setLevel(logging.DEBUG)

hostname = socket.gethostname()
logging.info('Running on node %s', hostname)

# Add empty arguments to handle missing arguments. 
sys.argv.extend(9*[''])
try:
    # The first four arguments are dummy.
    (seqnr, msn, fsys, msnvds, lroot, psn, wd, vdsdir, dry) = sys.argv[5:14]
except ValueError:
    print 'usage:', os.path.basename(sys.argv[0]), 'dummy dummy dummy dummy', \
          'rank ms-part filesys ms-part-vds lofarroot parset-file wd vds-dir [no]dry'
    sys.exit(1)

# Show input arguments
logging.debug('seqnr = %s', seqnr)
logging.debug('msn   = %s', msn)
logging.debug('lroot = %s', lroot)
logging.debug('psn   = %s', psn)
logging.debug('wd    = %s', wd)
logging.debug('vdsdir= %s', vdsdir)
logging.debug('dry   = %s', dry)

# Before we can import LOFAR stuff, we need to adjust sys.path. 
# Source lofarinit.sh in a shell to get the LOFAR-specific PYTHONPATH,
# and prepend it to sys.path
cmd = '. ' + os.path.join(lroot, 'lofarinit.sh') + '; echo $PYTHONPATH';
pypath = os.popen(cmd).read().strip().split(':')
logging.debug("Adding '%s' to sys.path", ":".join(pypath))
for p in reversed(pypath):
    sys.path.insert(1, p)

# Now we can import LOFAR stuff
from lofar.pipeline.parset import Parset


# Get name of input MS from VDS file, until IDPPP can handle VDS files.
#msin = Parset(msn).getString('Name')
msin = msn
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

# Output VDS file; will be stored in directory 'vdsdir', using output MS name.
vds = vdsdir + '/' + os.path.basename(msout) + '.vds'
logging.info('Output VDS: %s', vds)

# Create working dir if it doesn't exist and change to it. Working directory
# `wd' may already have been created by another instance of dppp_node.py. So
# wrap os.makedirs() in a try-except block.
if wd:
    try:
        os.makedirs(wd)
    except OSError, e:
        logging.info(e)
    os.chdir(wd)
wd = os.getcwd()
logging.info('Current working directory: %s', wd)

# Create the output directory for the VDS file if it doesn't exist yet.
if not os.path.exists(vdsdir):
    os.makedirs(vdsdir)

# Parset filename for the current run.
parsetfile = 'CS1_IDPPP.%02d.parset' % int(seqnr)

# Clean up working directory
os.system('rm -rf %s %s' % (msout, parsetfile))
logging.info('Removed %s, and %s' % (msout, parsetfile))

# Create a parset file for the current run
logging.info("Creating parset-file")
parset  = Parset(psn)
parset['msin'] = msin
parset['msout'] = msout
parset.writeToFile(parsetfile)

# Start IDPPP
cmd = os.path.join(lroot, 'bin/CS1_IDPPP') + ' ' + parsetfile + ' 1'
if dry == 'dry':
    logging.info('Dry run: %s', cmd)
else:
    logging.info('Start processing: %s', msn)
    sts = os.WEXITSTATUS(os.system(cmd))
    if sts:
        logging.error('CS1_IDPPP returned with error status %d', sts)
        sys.exit(sts)
    else:
        logging.info('CS1_IDPPP finished')

# Now create a VDS file (until IDPPP can do this)
cmd = os.path.join(lroot, 'bin/makevds') + ' "" ' + msout + ' ' + vds
if dry == 'dry':
    logging.info('Dry run: %s', cmd)
else:
    logging.info('Creating VDS file %s', vds)
    sts = os.WEXITSTATUS(os.system(cmd))
    if sts:
        logging.error('makevds returned with an error status %d', sts)
        sys.exit(sts)
    else:
        logging.info('makevds finished')

# Argh!! And now for the really ugly part. We must patch the VDS-file,
# to correct the key/value pair FileSys.
logging.info('Patching VDS file')
# Try to figure out the mount point using 'df'.
mp = os.popen("df . ").read().split()[-1]
filesys = hostname + ':' + mp
logging.info('FileSys = %s', filesys)
if dry != 'dry':
    ps = Parset(vds)
    ps['FileSys'] = filesys
    ps.writeToFile(vds)


# Note: Maybe I should replace each os.system() call with a call to popen(),
# followed by waitpid() -- ref. Python 2.5 manual sec. 17.1.3.3
