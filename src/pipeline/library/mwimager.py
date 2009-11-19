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

from WSRTrecipe import WSRTrecipe

class MWimager(WSRTrecipe):
    """Class wrapper around the mwimager script that is used to start the
    distributed Master-Worker imager."""
    def __init__(self):
        WSRTrecipe.__init__(self)
        self.inputs['parset-file'] = 'mwimager.parset'
        self.inputs['clusterdesc'] = None
        self.inputs['workingdir']  = None
        self.inputs['logfile']     = None
        self.inputs['dryrun']      = None
        self.helptext = """This function runs the mwimager"""

    ## Code to generate results ----------------------------------------
    def go(self):
        """Implementation of the WSRTrecipe.go() interface. This function does
        the actual work by calling the WSRTrecipe.cook_system() method."""
        opts = []
        for k in ['parset-file', 'clusterdesc', 'workingdir', 'logfile']:
            if self.inputs[k] is None: 
                opts.append('')
            else: 
                opts.append(self.inputs[k])
        if self.inputs['dryrun']:
            opts.append('dry')
        sts = self.cook_system('mwimager', opts)
        print "mwimager returned with status:", sts
        return sts


## Stand alone execution code ------------------------------------------
if __name__ == '__main__':
    __standalone__ = MWimager()
    __standalone__.main()
