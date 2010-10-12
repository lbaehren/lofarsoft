#                                                         LOFAR IMAGING PIPELINE
#
#                                                  rficonsole (AOflagger) recipe
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from __future__ import with_statement

from contextlib import nested
from collections import defaultdict

import sys

import lofarpipe.support.utilities as utilities
import lofarpipe.support.lofaringredient as ingredient
from lofarpipe.support.baserecipe import BaseRecipe
from lofarpipe.support.remotecommand import RemoteCommandRecipeMixIn
from lofarpipe.support.remotecommand import ComputeJob
from lofarpipe.support.group_data import load_data_map

class rficonsole(BaseRecipe, RemoteCommandRecipeMixIn):
    inputs = {
        'executable': ingredient.ExecField(
            '--executable',
            default="/opt/LofIm/daily/lofar/bin/rficonsole",
            help="rficonsole executable"
        ),
        'strategy': ingredient.FileField(
            '--strategy',
            help="RFI strategy",
            optional=True
        ),
        'nthreads': ingredient.IntField(
            '--nthreads',
            default=8,
            help="Number of threads per rficonsole process"
        ),
    }

    def go(self):
        self.logger.info("Starting rficonsole run")
        super(rficonsole, self).go()

        #                           Load file <-> compute node mapping from disk
        # ----------------------------------------------------------------------
        self.logger.debug("Loading map from %s" % self.inputs['args'])
        data = load_data_map(self.inputs['args'][0])

        #      We'll dispatch a single rficonsole job to each node, asking it to
        #                                    process all the data for that node.
        # ----------------------------------------------------------------------
        hostlist = defaultdict(list)
        for host, file in data:
            hostlist[host].append(file)

        if self.inputs.has_key("strategy"):
            strategy = self.inputs["strategy"]
        else:
            strategy = None
        command = "python %s" % (self.__file__.replace('master', 'nodes'))
        jobs = []
        for host, files in hostlist.iteritems():
            jobs.append(
                ComputeJob(
                    host, command,
                    arguments=[
                        self.inputs['executable'],
                        self.inputs['nthreads'],
                        strategy
                    ] + hostlist[host]
                )
            )
        self._schedule_jobs(jobs)

        if self.error.isSet():
            self.logger.warn("Failed rficonsole process detected")
            return 1
        else:
            return 0

if __name__ == '__main__':
    sys.exit(rficonsole().main())
