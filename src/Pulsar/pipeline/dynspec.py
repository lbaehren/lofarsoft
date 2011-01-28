#!/usr/env/python

# $Id$

#                                                          LOFAR PULSAR PIPELINE
#
#                                                     Pulsar.pipeline.dynspec.py
#                                                          Ken Anderson, 2011-10
#                                                            k.r.anderson@uva.nl
# ------------------------------------------------------------------------------
"""
This is a model pipeline definition, aka dynspec.py.  The interface is precisely
that of pulp.py.  As a minor variation on pulp.py definition, dyspec.py should
mirror all changes to pulp.py, while the pulpVersion should apply to both
pipeline definitions.

This definition is tighlty bound to beam-formed data processing.  Alteration of
this definition should be undertaken only with proper expertise.

N.B. Commonality of the run_node interfaces to all recipes, i.e. a common,
shared argument set, is a design  decision only.  Future processing
requirements may demand modification to these interfaces, and/or that new
run_node recipe interfaces and definitions may deviate from this design.

                                                          -- k.r.anderson,
                                                               28.01.2011
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
__svn_revision__ = ('$Rev$').split()[1]
__svn_revdate__  = ('$Date$')[7:26]
__svn_author__   = ('$Author$').split()[1]


class dynspec(control):

    """
    Provides a convenient, pipeline-based mechanism of running only
    bf2presto and rfiplot on beam-formed data.

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
                self.run_task("buildPulsArch",obsid=obsid,pulsar=pulsar,filefactor=filefactor,arch=arch)
                self.run_task("bf2presto",    obsid=obsid,pulsar=pulsar,filefactor=filefactor,arch=arch)
                self.run_task("rfiplot",      obsid=obsid,pulsar=pulsar,filefactor=filefactor,arch=arch)
        return


if __name__ == '__main__':
    sys.exit(pulp().main())
