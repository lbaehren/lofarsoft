#!/usr/env/python

"""
This is a model pulsar pipeline definition.

Although it should be runnable as it stands, the user is encouraged to copy it
to a job directory and customise it as appropriate for the particular task at
hand.
"""

from __future__ import with_statement
import sys, os
from lofarpipe.support.control import control
from lofarpipe.support.utilities import log_time
from lofarpipe.support.clusterhandler import ipython_cluster


class pulp(control):
    def __init__(self):
        super(pulp, self).__init__()

    def pipeline_logic(self):
        with log_time(self.logger):
            with ipython_cluster(self.config, self.logger):
                self.run_task("buildPulsArch", obsid="L2010_06160", pulsar="B0329+54", filefactor=4)
                self.run_task("bf2presto",     obsid="L2010_06160", pulsar="B0329+54", filefactor=4)
                self.run_task("prepfold",      obsid="L2010_06160", pulsar="B0329+54", filefactor=4)
        return

if __name__ == '__main__':
    sys.exit(pulp().main())
