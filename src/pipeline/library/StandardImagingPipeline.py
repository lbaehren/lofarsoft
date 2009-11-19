#!/usr/bin/env python
#
#  standard_imaging_pipeline.py: Script to run the distributed Default Pre-Processor Pipeline
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
from ingredient import WSRTingredient
#from dppp import DPPP
#from mwimager import MWImager
from parset import Parset
import sysconfig

import os
import sys

class StandardImagingPipeline(WSRTrecipe):
    """This is the recipe for the LOFAR standard imagaging pipeline."""
    def __init__(self):
        WSRTrecipe.__init__(self)
        self.inputs['dppp-parset']  = 'dppp.parset'
        self.inputs['mwimager-parset'] = 'mwimager.parset'
        self.inputs['cluster-name'] = ''
        self.inputs['directory']    = None
        self.inputs['dryrun']       = None
        self.helptext = """This is the recipe for the LOFAR standard
        imagaging pipeline"""


    ## Code to generate results ----------------------------------------
    def go(self):
        """Implementation of the WSRTrecipe.go() interface."""

        # Run the Default Preprocessor Pipeline (DPPP)
        inputs = WSRTingredient(Parset(self.inputs['dppp-parset']))
        inputs['cluster-name'] = self.inputs['cluster-name']
        outputs = WSRTingredient()
        sts = self.cook_recipe('DPPP', inputs, outputs)
        if sts:
            print "DPPP returned with status", sts
            return sts

        # Run the Master-Worker Imager (mwimager)
        inputs = WSRTingredient(Parset(self.inputs['mwimager-parset']))
        outputs = WSRTingredient()
        sts = self.cook_recipe('MWImager', inputs, outputs)
        if sts:
            print "MWImager returned with status", sts
            return sts
    

## Stand alone execution code ------------------------------------------
if __name__ == '__main__':
    sys.exit(StandardImagingPipeline().main())
