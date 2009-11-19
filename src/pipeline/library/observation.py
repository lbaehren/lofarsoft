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

import os
import glob

class Observation(WSRTingredient):
    """Class containing meta-data of an observation"""
    
    def __init__(self, observation, cluster_name, directory = None,
                 glob_pattern = ['_SB[0-9]*.MS']):
        """
        Constructor.
          - observation: name of the observation (e.g. L2007_03463).
          - cluster_name: name of the cluster where the data resides
            (e.g. lifs).
          - directory [optional]: directory where the data is stored;
            None means directly below the mount-point
            (e.g. /lifs001/L2007_03463_SB0.MS).
          - glob_pattern: pattern used when matching MS-files.
        """
        WSRTingredient.__init__(self)
        clusterdesc = sysconfig.locate(cluster_name + '.clusterdesc')
        self.observation = os.path.join(directory, observation) \
                           if directory else observation
        self.ms_pattern = [self.observation + p for p in glob_pattern]
        self.mount_points = Parset(clusterdesc).getStringVector('MountPoints')
        print "observation:", self.observation
        print "ms_pattern:", self.ms_pattern
        print "mount_points:", self.mount_points

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
#        return [glob.glob(d) for d in dirs]  ## Hmm, returns list of list

