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

import os
import sys
import glob

class MakeVDSError(Exception):
    """Exception class for MakeVDS errors"""
    pass
    

def locate(filename, path_list):
    """Locate the file `filename' in the directories specified in `path_list'.
    Return filename including absolute path, as soon as a match is found. If
    no match is found None is returned (implicitly)."""
    from os.path import abspath, isfile, join
    for path in path_list:
        name = join(path, filename)
        if isfile(name):
            return abspath(name)


class MakeVDS(WSRTrecipe):
    """Generate a VDS files for all MS-files that belong to the specified
    observation. The argument `cluster-name' is used to locate a cluster
    description file, which (among other information) contains a list of mount
    points to use when searching for MS-files. Use the argument `directory' if
    the MS-files are located in a directory below the mount points."""
    
    def __init__(self):
        WSRTrecipe.__init__(self)
        self.inputs['observation'] = ''
        self.inputs['directory'] = None
        self.inputs['cluster-name'] = ''
        self.helptext = """
        This function generates vds files for the MS files that comprise the
        given observation."""
        self.sysconfpath = [os.path.join(x, y)
                            for x in ['',
                                      os.environ['LOFARROOT'],
                                      os.environ['HOME']]
                            for y in ['etc', '']]
        
    
    ## Code to generate results ----------------------------------------
    def go(self):
        """Implementation of the WSRTrecipe.go() interface. This function does
        the actual work by calling the WSRTrecipe.cook_system() method several
        times."""
        
        _obs = self.inputs['observation']
        _dir = self.inputs['directory']

        observation = os.path.join(_dir, _obs) if _dir else _obs
        clusterdesc = locate(self.inputs['cluster-name'] + '.clusterdesc',
                             self.sysconfpath)
        if not clusterdesc:
            raise MakeVDSError('Cluster description file not found')

        self.print_debug('Cluster description file: ' + clusterdesc)
        self.print_debug('Observation file pattern: ' + observation)
        
        mount_points = Parset(clusterdesc).getStringVector('MountPoints')
        self.print_debug('mount_points = ' + str(mount_points))

        pats = [observation + '_SB[0-9]*.MS', observation + '_SB[0-9]*.ms']
        dirs = [os.path.join(mp, pat) for mp in mount_points for pat in pats]
        fail = 0
        for _dir in dirs:
            self.print_debug('Searching for ' + str(pats) + ' in ' + _dir)
            for _ms in glob.glob(_dir):
                fail += self.cook_system('makevds', [clusterdesc, _ms])
        if fail:
            self.print_error(fail + ' makevds process(es) failed!')
            return 1
        else:
            self.print_notification('all makevds process(es) ran successfully.')
            return 0

## Stand alone execution code ------------------------------------------
if __name__ == '__main__':
    sys.exit(MakeVDS().main())
