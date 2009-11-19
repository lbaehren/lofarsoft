#!/usr/bin/env python
#
#  observation.py: Class containing meta-data of an observation
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


    def __init__(self, observation, cluster_name, directory = None,
                 glob_pattern = ['*SB[0-9]*.MS']):
        """
        Constructor.
          - observation:  name of the observation (e.g. L2007_03463).
          - cluster_name: name of the cluster where the data resides
                          (e.g. lifs).
          - directory:    directory, relative to the mount point, where
                          the data is stored (e.g. /lifs001/pipeline);
                          if None, use directory <mount-point>/<observation>
          - glob_pattern: pattern used when matching MS-files;
                          if None, use '*SB[0-9]*.MS'
        """
        WSRTingredient.__init__(self)
        clusterdesc = sysconfig.cluster_desc_file(cluster_name)
        self.observation = directory if directory else observation
        self.ms_pattern = [os.path.join(self.observation, p) \
                           for p in glob_pattern]
        self.mount_points = Parset(clusterdesc).getStringVector('MountPoints')

    def ms_files(self):
        """
        Return a list of all the MS-files that comprise the current
        observation. MS-files are searched in self.directory on all mounted
        disks in the cluster.
        """
        dirs = [os.path.join(mp, pat) 
                for mp in self.mount_points for pat in self.ms_pattern]
        files = []
        for d in dirs:
            files.extend(glob.glob(d))
        return files


## Stand alone execution code ------------------------------------------
if __name__ == '__main__':
    import sys
    print Observation(*sys.argv[1:]).ms_files()
