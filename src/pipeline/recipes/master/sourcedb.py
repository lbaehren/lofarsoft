from __future__ import with_statement
import os
import threading

# Local helpers
from lofarpipe.support.lofarrecipe import LOFARrecipe
from lofarpipe.support.ipython import LOFARTask
from lofarpipe.support.clusterlogger import clusterlogger
from lofarpipe.support.group_data import load_data_map
import lofarpipe.support.utilities as utilities
from lofarpipe.support.remotecommand import ProcessLimiter

class sourcedb(LOFARrecipe):
    def __init__(self):
        super(sourcedb, self).__init__()
        self.optionparser.add_option(
            '--executable',
            help="Executable for makesourcedb",
            default="/opt/LofIm/daily/lofar/bin/makesourcedb"
        )
        self.optionparser.add_option(
            '-s', '--skymodel',
            dest="skymodel",
            help="Input sky catalogue"
        )
        self.optionparser.add_option(
            '--nproc',
            help="Maximum number of simultaneous processes per compute node",
            default="8"
        )

    def go(self):
        self.logger.info("Starting sourcedb run")
        super(sourcedb, self).go()

        #                           Load file <-> compute node mapping from disk
        # ----------------------------------------------------------------------
        self.logger.debug("Loading map from %s" % self.inputs['args'])
        data = load_data_map(self.inputs['args'])

        #                               Limit number of process per compute node
        # ----------------------------------------------------------------------
        self.logger.debug("Limit to %s processes/node" % self.inputs['nproc'])
        compute_nodes_lock = ProcessLimiter(self.inputs['nproc'])

        command = "python %s" % (self.__file__.replace('master', 'nodes'))
        with clusterlogger(self.logger) as (loghost, logport):
            with utilities.log_time(self.logger):
                self.logger.debug("Logging to %s:%d" % (loghost, logport))
                sourcedb_threads = []
                for host, ms in data:
                    sourcedb_threads.append(
                        threading.Thread(
                            target=self._dispatch_compute_job,
                            args=(host, command, compute_nodes_lock[host],
                                loghost, str(logport),
                                self.inputs['executable'],
                                ms,
                                self.inputs['skymodel']
                            )
                        )
                    )
                [thread.start() for thread in sourcedb_threads]
                self.logger.info("Waiting for parmdb threads")
                [thread.join() for thread in sourcedb_threads]

        if self.error.isSet():
            return 1
        else:
            self.outputs['mapfile'] = self.inputs['args']
            return 0

if __name__ == '__main__':
    sys.exit(sourcedb().main())
