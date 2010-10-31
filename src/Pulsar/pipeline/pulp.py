#!/usr/env/python

"""
This is a model pulsar pipeline definition.

Although it should be runnable as it stands, the user is encouraged to copy it
to a job directory and customise it as appropriate for the particular task at
hand.
"""

from __future__ import with_statement

from lofarpipe.support.control import control
from lofarpipe.support.utilities import log_time
from lofarpipe.support.clusterhandler import ipython_cluster
import lofarpipe.support.lofaringredient as ingredient

import sys, os

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
		self.run_task("buildPulsArch",obsid=obsid,pulsar=pulsar,filefactor=filefactor,arch=arch)
                self.run_task("bf2presto",    obsid=obsid,pulsar=pulsar,filefactor=filefactor,arch=arch)
                self.run_task("buildRSPAll",  obsid=obsid,pulsar=pulsar,filefactor=filefactor,arch=arch)
                self.run_task("prepareInf",   obsid=obsid,pulsar=pulsar,filefactor=filefactor,arch=arch)
                self.run_task("prepfold",     obsid=obsid,pulsar=pulsar,filefactor=filefactor,arch=arch)
                self.run_task("rfiplot",      obsid=obsid,pulsar=pulsar,filefactor=filefactor,arch=arch)
        return


if __name__ == '__main__':
    sys.exit(pulp().main())
