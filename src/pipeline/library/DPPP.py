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
        self.inputs['cluster-name'] = 'lioff'
        self.inputs['observation']  = ''
        self.inputs['output-dir']   = '/data/' + os.environ['USER']
        self.inputs['dryrun']       = False
        self.helptext = """
        This function runs the distributed DPPP
        Usage: DPPP [OPTION...]
        --parset-file        parameter set filename for DPPP
                             (default: 'dppp.parset')
        --cluster-name       name of the cluster to be used for processing
                             (default: 'lioff')
        --observation        name of the observation (e.g. L2007_03463)
                             (no default)
        --output-dir         directory for the output MS-files;
                             only needed when VDS-files are missing
                             (optional; default: '/data/${USER}')
        --dryrun             do a dry run
                             (default: no)
        """


    ## Code to generate results ----------------------------------------
    def go(self):
        """Implementation of the WSRTrecipe.go() interface. This function does
        the actual work by calling the WSRTrecipe.cook_system() method."""

        dataset = self.inputs['observation'] + '.gds'
        clusterdesc = sysconfig.clusterdesc_file(self.inputs['cluster-name'])

        self.print_debug('dataset = ' + dataset)
        self.print_debug('clusterdesc = ' + clusterdesc)
        
        opts = []
        # arguments for 'startdistproc'
        opts += ['-mode', '0']
        opts += ['-nomasterhost']
        opts += ['-dsn', dataset]
        opts += ['-cdn',
                 sysconfig.clusterdesc_file(self.inputs['cluster-name'])]
        opts += ['-logfile', 'dppp.log']
        # program started by 'startdistproc'
        opts += [os.path.join(sysconfig.lofar_root(),
                              'bin/dppp_node.py')]
        # arguments for 'dppp_node'
        opts += [sysconfig.lofar_root()]
        opts += [os.path.abspath(self.inputs['parset-file'])]
        opts += [self.inputs['output-dir']]
        if self.inputs['dryrun']:
            opts += ['dry']

        self.print_message('startdistproc ' + ' '.join(opts))
        sts = self.cook_system('startdistproc', opts)
        return sts
    

## Stand alone execution code ------------------------------------------
if __name__ == '__main__':
    sys.exit(DPPP().main())
