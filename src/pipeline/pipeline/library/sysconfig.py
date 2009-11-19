#!/usr/bin/env python
#
#  sysconfig.py: System configuration utilities
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

"""System configuration utilities"""

class SysConfigError(Exception):
    """Exception class for SysConfig errors"""
    pass

def lofar_root():
    """Return the LOFAR root directory, which is stored in the environment
    variable LOFARROOT, as an absolute path. If LOFARROOT is not defined, a
    SysConfigError will be raised."""
    import os
    try:
        return os.path.abspath(os.environ['LOFARROOT'])
    except KeyError:
        raise SysConfigError, "environment variable LOFARROOT is not defined"
    
def sysconfpath():
    """Return the search path for system configuration files."""
    import os
    return [os.path.join(x, y)
            for x in ['.', lofar_root(), os.environ['HOME']]
            for y in ['etc', '']]

def locate(filename, path_list = sysconfpath()):
    """Locate the file `filename' in the directories specified in `path_list'.
    Return the canonical path of `filename' (eliminating any symbolic links),
    as soon as a match is found. If no match is found None is returned
    (implicitly)."""
    from os.path import abspath, isfile, join, pathsep
    for path in path_list:
        name = join(path, filename)
        if isfile(name):
            return abspath(name)
    raise IOError, filename + " not found in " + pathsep.join(path_list)


def clusterdesc_file(cluster_name):
    """Return the name of the cluster description file. Its name is the
    concatenation of the cluster name and the file extension '.clusterdesc'.
    It is searched for in the directories returned by sysconfpath()."""
    return locate(cluster_name + '.clusterdesc')
