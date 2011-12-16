#!/usr/env/python

#            $Id: pulp.py 6866 2011-01-28 09:59:43Z kenneth $

#                                                          LOFAR PULSAR PIPELINE
#
#                                                        Pulsar.pipeline.pulp.py
#                                                          Ken Anderson, 2010-10
#                                                            k.r.anderson@uva.nl
# ------------------------------------------------------------------------------
"""
This is a model pulsar pipeline definition, aka pulp.py

This definition is tighlty bound to pulsar processing.  Alteration of this 
definition should be undertaken only with proper expertise.

N.B. Commonality of the run_node interfaces to all recipes, i.e. a common,
shared argument set, is a design  decision only.  Future processing
requirements may demand modification to these interfaces, and/or that new
run_node recipe interfaces and definitions may deviate from this design.

                                                          -- k.r.anderson,
                                                               25.01.2011
"""

from __future__ import with_statement

from lofarpipe.support.control import control
from lofarpipe.support.utilities import log_time
from lofarpipe.support.clusterhandler import ipython_cluster
import lofarpipe.support.lofaringredient as ingredient

import sys, os

# This is version ...

import pulpVersion

__version__      = pulpVersion.version
__version_date__ = pulpVersion.version_date
__svn_revision__ = ('$Rev: 6880 $').split()[1]
__svn_revdate__  = ('$Date: 2011-01-28 14:51:09 +0100 (Fri, 28 Jan 2011) $')[7:26]
__svn_author__   = ('$Author: kenneth $').split()[1]


class pulp(control):

    """
    Provides a convenient, pipeline-based mechanism of running pulp
    on a dataset.
    """

    inputs = {
        'obsid' : ingredient.StringField(
            '--obsid',
            dest="obsid",
            help="Observation identifier"
        ),
        'pulsar' : ingredient.StringField(
            '--pulsar',
            dest="pulsar",
            help="Pulsar name"
        ),
        'filefactor' : ingredient.IntField(
            '--filefactor',
             dest="filefactor",
             help="factor by which obsid subbands will be RSP split."
        ),
        'arch' : ingredient.StringField(
            '--arch',
            dest="arch",
            help="Destination archive for output, pick one of arch134, etc."
        )
    }

    outputs = {
        'data': ingredient.ListField()
    }

    def pipeline_logic(self):

        obsid      = self.inputs['obsid']
        pulsar     = self.inputs['pulsar']
        arch       = self.inputs['arch']
        filefactor = self.inputs['filefactor']

        with log_time(self.logger):
            with ipython_cluster(self.config, self.logger):
                self.logger.info("Pulp package, version "+__version__+":")
                self.logger.info("pulp.py revision id: "+__svn_revision__)
                if filefactor == 1:
                    self.run_task("buildPulsArch",obsid=obsid,pulsar=pulsar,filefactor=filefactor,arch=arch)
                    self.run_task("bf2presto",    obsid=obsid,pulsar=pulsar,filefactor=filefactor,arch=arch)
                    self.run_task("prepareInf",   obsid=obsid,pulsar=pulsar,filefactor=filefactor,arch=arch)
                    self.run_task("prepfold",     obsid=obsid,pulsar=pulsar,filefactor=filefactor,arch=arch)
                    self.run_task("rfiplot",      obsid=obsid,pulsar=pulsar,filefactor=filefactor,arch=arch)
                    self.run_task("bundleFiles",  obsid=obsid,pulsar=pulsar,filefactor=filefactor,arch=arch)
                else:
                    self.run_task("buildPulsArch",obsid=obsid,pulsar=pulsar,filefactor=filefactor,arch=arch)
                    self.run_task("bf2presto",    obsid=obsid,pulsar=pulsar,filefactor=filefactor,arch=arch)
                    self.run_task("buildRSPAll",  obsid=obsid,pulsar=pulsar,filefactor=filefactor,arch=arch)
                    self.run_task("prepareInf",   obsid=obsid,pulsar=pulsar,filefactor=filefactor,arch=arch)
                    self.run_task("prepfold",     obsid=obsid,pulsar=pulsar,filefactor=filefactor,arch=arch)
                    self.run_task("rfiplot",      obsid=obsid,pulsar=pulsar,filefactor=filefactor,arch=arch)                    
                    self.run_task("bundleFiles",  obsid=obsid,pulsar=pulsar,filefactor=filefactor,arch=arch)
        return


if __name__ == '__main__':
    sys.exit(pulp().main())
