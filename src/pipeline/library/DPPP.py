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
import sysconfig

import os
import sys

class DPPP(WSRTrecipe):
    """Class wrapper around the IDPPP executable that is used to flag and
    compress UV-data in frequency and time."""
    def __init__(self):
        WSRTrecipe.__init__(self)
        self.inputs['parset-file']  = 'dppp.parset'
        self.inputs['cluster-name'] = ''
        self.inputs['directory']    = None
        self.inputs['dryrun']       = None
        self.helptext = """This function runs the distributed DPPP"""


    ## Code to generate results ----------------------------------------
    def go(self):
        """Implementation of the WSRTrecipe.go() interface. This function does
        the actual work by calling the WSRTrecipe.cook_system() method."""

        # Get the name of the GDS or VDS file describing the MS-files that
        # comprise one observation.
        dataset = Parset(self.inputs['parset-file'])['dataset']
        observation = os.path.splitext(dataset)[0]
        clusterdesc = \
                    sysconfig.cluster_desc_file(self.inputs['cluster-name'])

        directory = self.inputs['directory'] \
                    if self.inputs['directory'] \
                    else os.path.join('/data/scratch', observation)

##        # create the directory if it doesn't exist already
##        os.makedirs(directory)

        opts = []
        # arguments for 'startdistproc'
        opts += ['-mode', '0']
        opts += ['-nomasterhost']
        opts += ['-dsn', dataset]
        opts += ['-cdn',
                 sysconfig.cluster_desc_file(self.inputs['cluster-name'])]
        opts += ['-logfile', 'dppp.log']
        # program started by 'startdistproc'
        opts += [os.path.join(sysconfig.lofar_root(),
                              'bin/CS1_Offline_pipeline_node.py')]
        # arguments for 'CS1_Offline_pipeline_node'
        opts += [sysconfig.lofar_root()]
        opts += [os.path.realpath(self.inputs['parset-file'])]
        opts += [directory]
        if self.inputs['dryrun']:
            opts += ['dry']

        print opts
        sts = self.cook_system('startdistproc', opts)
        return sts
    

## Stand alone execution code ------------------------------------------
if __name__ == '__main__':
    sys.exit(DPPP().main())
