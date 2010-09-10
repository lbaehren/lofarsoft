#                                                         LOFAR IMAGING PIPELINE
#
#                                                        Baseline flagger recipe
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from __future__ import with_statement
from tempfile import mkstemp
from cPickle import dump
import os
import threading

import lofarpipe.support.utilities as utilities
import lofarpipe.support.lofaringredient as ingredient
from lofarpipe.support.baserecipe import BaseRecipe
from lofarpipe.support.remotecommand import RemoteCommandRecipeMixIn
from lofarpipe.support.clusterlogger import clusterlogger
from lofarpipe.support.group_data import load_data_map
from lofarpipe.support.remotecommand import ProcessLimiter

class flag_baseline(BaseRecipe, RemoteCommandRecipeMixIn):
    """
    Accept a list of baselines (in the format used by NDPPP logging).

    Flag them in all MeasurementSets.
    """
    inputs = {
        'baselines': ingredient.ListField(
            '--baselines',
            help="Baselines (in NDPPP format, eg 1&1)"
        ),
        'nproc': ingredient.IntField(
            '--nproc',
            help="Maximum number of simultaneous processes per compute node",
            default=8
        )
    }

    outputs = {
        'mapfile': ingredient.FileField()
    }

    def go(self):
        self.logger.info("Starting flag_baseline run")
        super(flag_baseline, self).go()

        #       Serialise list of baselines to disk for compute nodes to pick up
        # ----------------------------------------------------------------------
        fd, baseline_filename = mkstemp(
            dir=self.config.get("layout", "parset_directory")
        )
        baseline_file = os.fdopen(fd, "w")
        dump(self.inputs["baselines"], baseline_file)
        baseline_file.close()

        #                 try block ensures baseline_filename is always unlinked
        # ----------------------------------------------------------------------
        try:
            #                       Load file <-> compute node mapping from disk
            # ------------------------------------------------------------------
            self.logger.debug("Loading map from %s" % self.inputs['args'][0])
            data = load_data_map(self.inputs['args'][0])

            #                           Limit number of process per compute node
            # ------------------------------------------------------------------
            self.logger.debug("Limit to %s processes/node" % self.inputs['nproc'])
            compute_nodes_lock = ProcessLimiter(self.inputs['nproc'])

            command = "python %s" % (self.__file__.replace('master', 'nodes'))
            with clusterlogger(self.logger) as (loghost, logport):
                with utilities.log_time(self.logger):
                    self.logger.debug("Logging to %s:%d" % (loghost, logport))
                    flagger_threads = []
                    for host, ms in data:
                        flagger_threads.append(
                            threading.Thread(
                                target=self._dispatch_compute_job,
                                args=(host, command, compute_nodes_lock[host],
                                    loghost, str(logport),
                                    ms,
                                    baseline_filename
                                )
                            )
                        )
                    [thread.start() for thread in flagger_threads]
                    self.logger.info("Waiting for flagger threads")
                    [thread.join() for thread in flagger_threads]

        finally:
            os.unlink(baseline_filename)

        if self.error.isSet():
            return 1
        else:
            self.outputs['mapfile'] = self.inputs['args'][0]
            return 0

if __name__ == '__main__':
    sys.exit(flag_baseline().main())
