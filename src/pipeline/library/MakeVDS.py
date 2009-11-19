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

from lofar.pipeline.WSRTrecipe import WSRTrecipe
from lofar.pipeline.parset import Parset
from lofar.pipeline.Observation import Observation
from lofar.pipeline import sysconfig

import os
import sys
import glob

class MakeVDSError(Exception):
    """Exception class for MakeVDS errors"""
    pass
    

class MakeVDS(WSRTrecipe):
    """
    Generate VDS files for all MS-files that belong to the specified
    observation. The argument `cluster-name' is used to locate a cluster
    description file, which (among other information) contains a list of mount
    points to use when searching for MS-files. Use the argument `input-dir' if
    the MS-files are located in a directory below the mount points. The
    location where the VDS files will be stored can be specified using the
    argument `vds-dir'; `/users/${USER}/data/<observation>' by default. When
    the VDS files have been generated, a GDS file (which is like a
    concatenation of all VDS files) is generated in the same directory.
    """
    
    def __init__(self):
        WSRTrecipe.__init__(self)
        self.inputs['cluster-name']    = 'lioff'
        self.inputs['observation']     = ''
        self.inputs['input-dir']       = None
        self.inputs['vds-dir']         = None
        self.inputs['dryrun']          = False
        self.helptext = """
        This function generates vds files for the MS files that comprise the
        given observation.

        Usage: MakeVDS [OPTION...]
        --cluster-name       name of the cluster where the data resides
                             (default: 'lioff')
        --observation        name of the observation (e.g. L2007_03463)
                             (no default)
        --input-dir          directory, relative to the mount point, where
                             the input data is stored (e.g. /lifs001/pipeline);
                             if None, use directory <mount-point>/<observation>
        --vds-dir            directory for the output VDS-files;
                             if None, use directory
                             '/users/${USER}/data/<observation>'
        --dryrun             do a dry run
                             (default: no)
        """
    
    ## Code to generate results ----------------------------------------
    def go(self):
        """Implementation of the WSRTrecipe.go() interface. This function does
        the actual work by calling the WSRTrecipe.cook_system() method several
        times."""
        clusterdesc = sysconfig.clusterdesc_file(self.inputs['cluster-name'])
        ms_files = Observation(self.inputs['cluster-name'],
                               self.inputs['observation'],
                               self.inputs['input-dir']).ms_files()
        vds_dir = self.inputs['vds-dir'] \
                  if self.inputs['vds-dir'] \
                  else os.path.join('/users', os.environ['USER'], 'data',
                                    self.inputs['observation'])
        vds_files = [vds_dir + '/' + os.path.basename(f) + '.vds'
                     for f in ms_files]

        self.print_debug('ms_files: ' + str(ms_files))
        self.print_debug('vds_files: ' + str(vds_files))

        if len(ms_files) == 0:
            self.print_warning('no files to process!')
            return 0

        if not os.path.exists(vds_dir):
            os.makedirs(vds_dir)
            self.print_debug('Created directory ' + vds_dir)

        # Generate the VDS-files.
        fail = 0
        for (ms, vds) in zip(ms_files, vds_files):
            self.print_message('Processing file ' + ms)
            if not self.inputs['dryrun']:
                fail += self.cook_system('makevds', [clusterdesc, ms, vds])

        if fail:
            self.print_error(str(fail) + ' makevds process(es) failed!')
            return 1

        # Combine the VDS-files into one GDS-file.
        self.print_message('Generating gds file from vds files')
        opts = [os.path.join(vds_dir, self.inputs['observation'] + '.gds')]
        opts.extend(vds_files)
        if not self.inputs['dryrun']:
            if self.cook_system('combinevds', opts):
                self.print_error('combinevds failed!')
                return 1

        return 0

## Stand alone execution code ------------------------------------------
if __name__ == '__main__':
    sys.exit(MakeVDS().main())
