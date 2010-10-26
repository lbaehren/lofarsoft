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
        'indirect_read': ingredient.BoolField(
            '--indirect-read',
            default=False,
            help="Indirect baseline reader: re-write MS for efficiency"
        ),
        'working_dir': ingredient.StringField(
            '--working-dir',
            default='/tmp',
            help="Store temporary products under this root"
        ),
        'nthreads': ingredient.IntField(
            '--nthreads',
            default=8,
            help="Number of threads per rficonsole process"
        ),
        'nproc': ingredient.IntField(
            '--nproc',
            default=1,
            help="Maximum number of simultaneous processes per node"
        ),
        'nmeasurementsets': ingredient.IntField(
            '--nmeasurementsets',
            optional=True,
            help="Maximum number of MeasurementSets processed by a single rficonsole process"
        ),
    }

    def go(self):
        self.logger.info("Starting rficonsole run")
        super(rficonsole, self).go()

        #                           Load file <-> compute node mapping from disk
        # ----------------------------------------------------------------------
        self.logger.debug("Loading map from %s" % self.inputs['args'])
        data = load_data_map(self.inputs['args'][0])

        #        Jobs being dispatched to each host are arranged in a dict. Each
        #            entry in the dict is a list of list of filnames to process.
        # ----------------------------------------------------------------------
        hostlist = defaultdict(lambda: list([[]]))
        for host, filename in data:
            if (
                self.inputs.has_key('nmeasurementsets') and
                len(hostlist[host][-1]) >= self.inputs['nmeasurementsets']
            ):
                hostlist[host].append([filename])
            else:
                hostlist[host][-1].append(filename)

        if self.inputs.has_key('strategy'):
            strategy = self.inputs['strategy']
        else:
            strategy = None

        command = "python %s" % (self.__file__.replace('master', 'nodes'))
        jobs = []
        for host, file_lists in hostlist.iteritems():
            for file_list in file_lists:
                jobs.append(
                    ComputeJob(
                        host, command,
                        arguments=[
                            self.inputs['executable'],
                            self.inputs['nthreads'],
                            strategy,
                            self.inputs['indirect_read'],
                            self.inputs['working_dir']
                        ] + file_list
                    )
                )
        self._schedule_jobs(jobs, max_per_node=self.inputs['nproc'])

        if self.error.isSet():
            self.logger.warn("Failed rficonsole process detected")
            return 1
        else:
            return 0

if __name__ == '__main__':
    sys.exit(rficonsole().main())
