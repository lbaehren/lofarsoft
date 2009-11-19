#!/usr/bin/env python
#
#  makevds.py: Class for generating VDS-files from MS-files.
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

"""Class for generating VDS-files from MS-files"""

from WSRTrecipe import WSRTrecipe
from parset import Parset
import sysconfig

import os
import sys
import glob

class MakeVDSError(Exception):
    """Exception class for MakeVDS errors"""
    pass
    

class MakeVDS(WSRTrecipe):
    """Generate a VDS files for all MS-files that belong to the specified
    observation.
    The argument `cluster-name' is used to locate a cluster description file,
    which (among other information) contains a list of mount points to use
    when searching for MS-files. Use the argument `directory' if the MS-files
    are located in a directory below the mount points.
    After the VDS files have been generated a GDS file (which is like a
    concatenation of all VDS files) is generated and stored in the current
    directory."""
    
    def __init__(self):
        WSRTrecipe.__init__(self)
        self.inputs['observation'] = ''
        self.inputs['directory'] = None
        self.inputs['cluster-name'] = ''
        self.helptext = """
        This function generates vds files for the MS files that comprise the
        given observation."""
        
    
    ## Code to generate results ----------------------------------------
    def go(self):
        """Implementation of the WSRTrecipe.go() interface. This function does
        the actual work by calling the WSRTrecipe.cook_system() method several
        times."""
        
        obs_ = self.inputs['observation']
        dir_ = self.inputs['directory']

        observation = os.path.join(dir_, obs_) if dir_ else obs_
        clusterdesc = sysconfig.locate(self.inputs['cluster-name'] + 
                                       '.clusterdesc',
                                       sysconfig.sysconfpath())
        if not clusterdesc:
            raise MakeVDSError('Cluster description file not found')

        self.print_debug('Cluster description file: ' + clusterdesc)
        self.print_debug('Observation file pattern: ' + observation)
        
        mount_points = Parset(clusterdesc).getStringVector('MountPoints')
        self.print_debug('mount_points = ' + str(mount_points))

        pats = [observation + '_SB[0-9]*.MS', observation + '_SB[0-9]*.ms']
        dirs = [os.path.join(mp, pat) for mp in mount_points for pat in pats]
        fail = 0
        vds_files = []
        for _dir in dirs:
            self.print_debug('Searching for ' + str(pats) + ' in ' + _dir)
            for _ms in glob.glob(_dir):
                vds = _ms + '.vds'
                vds_files.append(vds)
                self.print_message('Processing file ' + _ms)
                fail += self.cook_system('makevds', [clusterdesc, _ms, vds])

        if fail:
            self.print_error(str(fail) + ' makevds process(es) failed!')
            return 1

        if len(vds_files) == 0:
            self.print_warning('no files to process!')
            return 0

        self.print_message('Generating gds file from vds files')
        opts = [obs_ + '.gds']
        opts.extend(vds_files)
        if self.cook_system('combinevds', opts):
            self.print_error('combinevds failed!')
            return 1

        return 0

## Stand alone execution code ------------------------------------------
if __name__ == '__main__':
    sys.exit(MakeVDS().main())
