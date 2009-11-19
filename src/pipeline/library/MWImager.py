#!/usr/bin/env python
#
#  mwimager.py: Script to run the mwimager
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

"""Script to run the mwimager"""

from WSRTrecipe import WSRTrecipe
import sysconfig

import os.path, sys

class MWImager(WSRTrecipe):
    """Class wrapper around the mwimager script that is used to start the
    distributed Master-Worker imager."""
    def __init__(self):
        WSRTrecipe.__init__(self)
        self.inputs['parset-file']  = 'mwimager.parset'
        self.inputs['cluster-name'] = 'lioff'
        self.inputs['observation']  = ''
        self.inputs['output-dir']   = None
        self.inputs['logfile']      = None
        self.inputs['dryrun']       = False
        self.helptext = """
        This is the recipe for the LOFAR Master-Worker Imager.

        Usage: MWImager [OPTION...]
        --parset-file        parameter set filename for MWImager
                             (default: 'mwimager.parset')
        --cluster-name       name of the cluster to be used for processing
                             (default: 'lioff')
        --observation        name of the observation (e.g. L2007_03463)
                             (no default)
        --output-dir         directory where images will be stored
                             (default: same directory as MS)
        --logfile            root name of logfile of each subprocess
                             (default 'mwimager.log')
        --dryrun             do a dry run
                             (default: no)
        """

    ## Code to generate results ----------------------------------------
    def go(self):
        """Implementation of the WSRTrecipe.go() interface. This function does
        the actual work by calling the WSRTrecipe.cook_system() method."""
        opts = []
        opts += [os.path.abspath(self.inputs['parset-file']) \
                 if self.inputs['parset-file'] is not None else '']
        opts += [sysconfig.clusterdesc_file(self.inputs['cluster-name']) \
                 if self.inputs['cluster-name'] is not None else '']
        opts += [self.inputs['output-dir'] \
                 if self.inputs['output-dir'] is not None else '']
        opts += [self.inputs['logfile'] \
                 if self.inputs['logfile'] is not None else '']
        opts += ['dry' if self.inputs['dryrun'] else '']

        self.print_message('mwimager ' + ' '.join(opts))
        sts = self.cook_system('mwimager', opts)
        return sts


## Stand alone execution code ------------------------------------------
if __name__ == '__main__':
    sys.exit(MWImager().main())
