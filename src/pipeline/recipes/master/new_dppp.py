#                                                         LOFAR IMAGING PIPELINE
#
#                                         New DPPP recipe: fixed node allocation
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from __future__ import with_statement

from itertools import cycle
from contextlib import nested

import subprocess
import collections
import threading
import sys
import os

import lofarpipe.support.utilities as utilities
from lofarpipe.support.lofarrecipe import BaseRecipe
from lofarpipe.support.remotecommand import RemoteCommandRecipeMixIn
from lofarpipe.support.clusterlogger import clusterlogger
from lofarpipe.support.remotecommand import ProcessLimiter
from lofarpipe.support.group_data import load_data_map
from lofarpipe.support.parset import Parset

class new_dppp(BaseRecipe, RemoteCommandRecipeMixIn):
    def __init__(self):
        super(new_dppp, self).__init__()
        self.optionparser.add_option(
            '--executable',
            help="DPPP executable"
        )
        self.optionparser.add_option(
            '--initscript',
            help="DPPP initscript"
        )
        self.optionparser.add_option(
            '-p', '--parset',
            help="Parset containing configuration for DPPP"
        )
        self.optionparser.add_option(
            '--suffix',
            default=".dppp",
            help="Suffix to add to trimmed data (default: .dppp)"
        )
        self.optionparser.add_option(
            '-w', '--working-directory',
            help="Working directory used on compute nodes"
        )
        self.optionparser.add_option(
            '--data-start-time',
            help="Start time to be passed to DPPP (optional)",
        )
        self.optionparser.add_option(
            '--data-end-time',
            help="End time to be passed to DPPP (optional)",
        )
        self.optionparser.add_option(
            '--nproc',
            help="Maximum number of simultaneous processes per compute node",
            default="8"
        )
        self.optionparser.add_option(
            '--nthreads',
            help="Number of threads per (N)DPPP process",
            default="2"
        )
        self.optionparser.add_option(
            '--mapfile',
            help="Output mapfile"
        )

    def go(self):
        self.logger.info("Starting DPPP run")
        super(new_dppp, self).go()

        #                           Load file <-> compute node mapping from disk
        # ----------------------------------------------------------------------
        self.logger.debug("Loading map from %s" % self.inputs['args'])
        data = load_data_map(self.inputs['args'])

        #                               Limit number of process per compute node
        # ----------------------------------------------------------------------
        self.logger.debug("Limit to %s processes/node" % self.inputs['nproc'])
        compute_nodes_lock = ProcessLimiter(self.inputs['nproc'])

        #       We can use the same node script as the "old" IPython dppp recipe
        # ----------------------------------------------------------------------
        command = "python %s" % (
            self.__file__.replace('master', 'nodes').replace('new_dppp', 'dppp')
        )
        with nested(
            clusterlogger(self.logger), utilities.log_time(self.logger)
        ) as ((loghost, logport), unused):
            self.logger.debug("Logging to %s:%d" % (loghost, logport))
            dppp_threads = []
            outnames = collections.defaultdict(list)
            for host, ms in data:
                outnames[host].append(
                    os.path.join(
                        self.inputs['working_directory'],
                        self.inputs['job_name'],
                        os.path.basename(ms) + self.inputs['suffix']
                    )
                )
                dppp_threads.append(
                    threading.Thread(
                        target=self._dispatch_compute_job,
                        args=(host, command, compute_nodes_lock[host],
                            loghost, logport,
                            ms,
                            outnames[host][-1],
                            self.inputs['parset'],
                            self.inputs['executable'],
                            self.inputs['initscript'],
                            self.inputs['data_start_time'],
                            self.inputs['data_end_time'],
                            self.inputs['nthreads'],
                        )
                    )
                )
            [thread.start() for thread in dppp_threads]
            self.logger.info("Waiting for DPPP threads")
            [thread.join() for thread in dppp_threads]

        if self.error.isSet():
            self.logger.warn("Failed DPPP process detected")
            return 1
        else:
            parset = Parset()
            for host, filenames in outnames.iteritems():
                parset.addStringVector(host, filenames)
            self.outputs['mapfile'] = self.inputs['mapfile']
            parset.writeFile(self.outputs['mapfile'])
            return 0

if __name__ == '__main__':
    sys.exit(new_dppp().main())
