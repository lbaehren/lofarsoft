#!/usr/bin/env python
#
#  dppp.py: Script to run the distributed Default Pre-Processor Pipeline
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

"""Script to run the distributed Default Pre-Processor Pipeline"""

from WSRTrecipe import WSRTrecipe
from parset import Parset

import sys

class DPPP(WSRTrecipe):
    """Class wrapper around the IDPPP executable that is used to flag and
    compress UV-data in frequency and time."""
    def __init__(self):
        WSRTrecipe.__init__(self)
        self.inputs['parset-file']  = 'dppp.parset'
        self.inputs['cluster-name'] = ''
        self.inputs['hostfile']    = None
        self.inputs['logfile']     = None
        self.inputs['mode']        = 0
        self.inputs['masterhost']  = None
        self.inputs['extra-hosts'] = None
        self.inputs['workingdir']  = None
        self.inputs['dryrun']      = None
        self.helptext = """This function runs the distributed DPPP"""
        self.parset = None

    def main_init(self):
        """This method is called by the framework before run() is called.
        It can be used to initialize variables, open files, etc."""

        if WSRTrecipe.main_init(self):
            return 1

        self.parset = Parset(self.inputs['parset-file'])
##        # Add the key/value pairs in the parset-file to our inputs dictionary.
##        # This is a no-op, if no parset-file was specified.
##        self.inputs.update(Parset(self.inputs['parset-file']))
##        self.print_debug(self.inputs)
        
        return 0
        
        
    ## Code to generate results ----------------------------------------
    def go(self):
        """Implementation of the WSRTrecipe.go() interface. This function does
        the actual work by calling the WSRTrecipe.cook_system() method."""

        # Get the name of the GDS or VDS file describing the MS-files that
        # comprise one observation.
        dataset = self.parset['dataset']
        hostfile = None
        clusterdesc = \
                    sysconfig.cluster_desc_file(self.inputs(['cluster-name']))
        logfile = self.inputs['logfile']
        dry = self.inputs['dryrun']

        opts = []
        for k in ['parset-file', 'clusterdesc', 'hostfile', 'logfile'
        # startdistproc -mode 0 -nomasterhost -dsn "$msvds" -hfn "$hfn" \
        #               -cdn "$cdn" -logfile "$logfile" \
        #               $pgmpath/mwimager-part "$LOFARROOT" "$psn" "$wd" "$dry"
        pass

## Stand alone execution code ------------------------------------------
if __name__ == '__main__':
    sys.exit(DPPP().main())
