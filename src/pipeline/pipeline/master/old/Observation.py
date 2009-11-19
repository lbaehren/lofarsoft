#!/usr/bin/env python
#
#  Observation.py: Class containing meta-data of an observation
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

"""Class containing meta-data of an observation"""

from ingredient import WSRTingredient
from parset import Parset
import sysconfig

import os.path
import glob

class Observation(WSRTingredient):
    """Class containing meta-data of an observation"""

    def __init__(self, cluster_name, observation, directory = None,
                 glob_pattern = None):
        """
        Constructor.
          - cluster_name:   name of the cluster where the data resides
                            (e.g. lifs).
          - observation:    name of the observation (e.g. L2007_03463).
          - directory:      directory, relative to the mount point, where
                            the data is stored (e.g. /lifs001/pipeline);
                            if None, use directory <mount-point>/<observation>
          - glob_pattern:   pattern used when matching MS-files;
                            if None, use <observation>/SB[0-9]*.MS and
                            <observation>_SB[0-9]*.MS.
        """
        WSRTingredient.__init__(self)
        self.mount_points = Parset(sysconfig.clusterdesc_file(cluster_name)).\
                            getStringVector('MountPoints')
        if glob_pattern is None:
            glob_pattern = [os.path.join(observation, 'SB[0-9]*.MS'),
                            observation + '_SB[0-9]*.MS']
        if directory:
            self.ms_pat = [os.path.join(directory, p) for p in glob_pattern]
        else:
            self.ms_pat = glob_pattern

    def ms_files(self):
        """
        Return a list of all the MS-files that comprise the current
        observation. MS-files are searched on all mounted disks using
        self.ms_pat as glob pattern.
        """
        dirs = [os.path.join(mp, pat) 
                for mp in self.mount_points for pat in self.ms_pat]
        files = []
        for d in dirs:
            files.extend(glob.glob(d))
        return files


## Stand alone execution code ------------------------------------------
if __name__ == '__main__':
    import sys
    if len(sys.argv) < 3:
        print 'Usage:', sys.argv[0], '<cluster-name> <observation> [directory]'
    else:
        print Observation(*sys.argv[1:]).ms_files()
