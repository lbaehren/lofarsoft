#!/usr/bin/env python
#
#  StandardImagingPipeline.py: Script to run the Standard Imaging Pipeline.
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

"""Script to run the Standard Imaging Pipeline"""

from lofar.pipeline.WSRTrecipe import WSRTrecipe
from lofar.pipeline.ingredient import WSRTingredient
from lofar.pipeline.parset import Parset
from lofar.pipeline import sysconfig

import os
import sys

class StandardImagingPipeline(WSRTrecipe):
    """This is the recipe for the LOFAR Standard Imagaging Pipeline."""
    def __init__(self):
        WSRTrecipe.__init__(self)
        self.inputs['dppp-parset']     = 'dppp.parset'
        self.inputs['mwimager-parset'] = 'mwimager.parset'
        self.inputs['cluster-name']    = 'lioff'
        self.inputs['observation']     = ''
        self.inputs['make-vds-files']  = True
        self.inputs['input-dir']       = None
        self.inputs['output-dir']      = None
        self.inputs['vds-dir']         = None
        self.inputs['dryrun']          = False
        self.helptext = """
        This is the recipe for the LOFAR standard imagaging pipeline.
        
        Usage: StandardImagingPipeline [OPTION...]
        --dppp-parset        parameter set filename for DPPP
                             (default: 'dppp.parset')
        --mwimager-parset    parameter set filename for MWImager
                             (default: 'mwimager.parset')
        --cluster-name       name of the cluster to be used for processing
                             (default: 'lioff')
        --observation        name of the observation to be processed
                             (no default)
        --make-vds-files     create VDS files
                             (default: yes)
        --input-dir          directory for the input MS-files;
                             only needed when VDS files are missing
                             (optional; no default)
        --output-dir         directory for the output MS-files;
                             only needed when VDS files are missing
                             (optional; default: '/data/${USER}/<obs>')
        --vds-dir            directory for the output VDS-files;
                             only needed until IDPPP creates these files
                             (optional; default: '/users/${USER}/data/<obs>)
        --dryrun             do a dry run
                             (default: no)
        """


    ## Code to generate results ----------------------------------------
    def go(self):
        """Implementation of the WSRTrecipe.go() interface."""

        obs = self.inputs['observation']
        
        if self.inputs['output-dir'] is None:
            self.inputs['output-dir']  = '/data/' + os.environ['USER'] + \
                                         '/' + obs

        if self.inputs['vds-dir'] is None:
            self.inputs['vds-dir'] = '/users/' + os.environ['USER'] + \
                                     '/data/' + obs
            
        # Create VDS files for the MS-files in the observation, if requested.
        if self.inputs['make-vds-files']:
            inputs = WSRTingredient()
            outputs = WSRTingredient()
            inputs['cluster-name'] = self.inputs['cluster-name']
            inputs['observation'] = self.inputs['observation']
            inputs['input-dir'] = self.inputs['input-dir']
            inputs['dryrun'] = self.inputs['dryrun']
            sts = self.cook_recipe('MakeVDS', inputs, outputs)
            if sts:
                print "MakeVDS returned with status", sts
                return sts
        
        # Run the Default Preprocessor Pipeline (DPPP)
        inputs = WSRTingredient(Parset(self.inputs['dppp-parset']))
        inputs['cluster-name'] = self.inputs['cluster-name']
        inputs['observation'] = self.inputs['observation']
        inputs['output-dir'] = self.inputs['output-dir']
        inputs['vds-dir'] = self.inputs['vds-dir']
        inputs['dryrun'] = self.inputs['dryrun']
        outputs = WSRTingredient()
        sts = self.cook_recipe('DPPP', inputs, outputs)
        if sts:
            print "DPPP returned with status", sts
            return sts

        # Combine the VDS files that were generated by DPPP
        vdsdir = inputs['output-dir']
        
        # Run the Master-Worker Imager (mwimager)
        inputs = WSRTingredient(Parset(self.inputs['mwimager-parset']))
        inputs['cluster-name'] = self.inputs['cluster-name']
        inputs['observation'] = self.inputs['observation']
        inputs['output-dir'] = self.inputs['output-dir']
        inputs['vds-dir'] = self.inputs['vds-dir']
        inputs['dryrun'] = self.inputs['dryrun']
        
        outputs = WSRTingredient()
        sts = self.cook_recipe('MWImager', inputs, outputs)
        if sts:
            print "MWImager returned with status", sts
            return sts
    

## Stand alone execution code ------------------------------------------
if __name__ == '__main__':
    sys.exit(StandardImagingPipeline().main())
