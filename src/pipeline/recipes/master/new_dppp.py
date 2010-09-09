#                                                         LOFAR IMAGING PIPELINE
#
#                                         New DPPP recipe: fixed node allocation
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from __future__ import with_statement

from itertools import cycle
from contextlib import nested
from collections import defaultdict

import subprocess
import collections
import threading
import sys
import os

import lofarpipe.support.utilities as utilities
import lofarpipe.support.lofaringredient as ingredient
from lofarpipe.support.baserecipe import BaseRecipe
from lofarpipe.support.remotecommand import RemoteCommandRecipeMixIn
from lofarpipe.support.clusterlogger import clusterlogger
from lofarpipe.support.remotecommand import ProcessLimiter
from lofarpipe.support.group_data import load_data_map
from lofarpipe.support.parset import Parset

class new_dppp(BaseRecipe, RemoteCommandRecipeMixIn):
    inputs = {
        'executable': ingredient.ExecField(
            '--executable',
            help="DPPP executable"
        ),
        'initscript': ingredient.FileField(
            '--initscript',
            help="DPPP initscript"
        ),
        'parset': ingredient.FileField(
            '-p', '--parset',
            help="Parset containing configuration for DPPP"
        ),
        'suffix': ingredient.StringField(
            '--suffix',
            default=".dppp",
            help="Suffix to add to trimmed data (default: .dppp)"
        ),
        'working_directory': ingredient.StringField(
            '-w', '--working-directory',
            help="Working directory used on compute nodes"
        ),
        'data_start_time': ingredient.StringField(
            '--data-start-time',
            default="None",
            help="Start time to be passed to DPPP (optional)"
        ),
        'data_end_time': ingredient.StringField(
            '--data-end-time',
            default="None",
            help="End time to be passed to DPPP (optional)"
        ),
        'nproc': ingredient.IntField(
            '--nproc',
            default=8,
            help="Maximum number of simultaneous processes per compute node"
        ),
        'nthreads': ingredient.IntField(
            '--nthreads',
            default=2,
            help="Number of threads per (N)DPPP process"
        ),
        'mapfile': ingredient.StringField(
            '--mapfile',
            help="Output mapfile name"
        )
    }

    outputs = {
        'mapfile': ingredient.FileField(),
        'fullyflagged': ingredient.ListField()
    }


    def go(self):
        self.logger.info("Starting DPPP run")
        super(new_dppp, self).go()

        #                Keep track of "Total flagged" messages in the DPPP logs
        # ----------------------------------------------------------------------
        self.logger.searchpatterns["fullyflagged"] = "Fully flagged baselines"

        #                           Load file <-> compute node mapping from disk
        # ----------------------------------------------------------------------
        self.logger.debug("Loading map from %s" % self.inputs['args'])
        data = load_data_map(self.inputs['args'][0])

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

        #                                  Log number of fully flagged baselines
        # ----------------------------------------------------------------------
        matches = self.logger.searchpatterns["fullyflagged"].results
        self.logger.searchpatterns.clear() # finished searching
        stripchars = "".join(set("Fully flagged baselines: "))
        baselinecounter = defaultdict(lambda: 0)
        for match in matches:
            for pair in (
                pair.strip(stripchars) for pair in match.getMessage().split(";")
            ):
                baselinecounter[pair] += 1
        self.outputs['fullyflagged'] = baselinecounter.keys()

        if self.error.isSet():
            self.logger.warn("Failed DPPP process detected")
            return 1
        else:
            parset = Parset()
            for host, filenames in outnames.iteritems():
                parset.addStringVector(host, filenames)
            parset.writeFile(self.inputs['mapfile'])
            self.outputs['mapfile'] = self.inputs['mapfile']
            return 0

if __name__ == '__main__':
    sys.exit(new_dppp().main())
