from __future__ import with_statement
import os
import threading

# Local helpers
from lofarpipe.support.lofarrecipe import LOFARrecipe
from lofarpipe.support.ipython import LOFARTask
from lofarpipe.support.clusterlogger import clusterlogger
from lofarpipe.support.remotecommand import run_remote_command
from lofarpipe.cuisine.parset import Parset
import lofarpipe.support.utilities as utilities

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

    def go(self):
        self.logger.info("Starting sourcedb run")
        super(sourcedb, self).go()

        #                           Load file <-> compute node mapping from disk
        # ----------------------------------------------------------------------
        self.logger.debug("Loading map from %s" % self.inputs['args'])
        datamap = Parset(self.inputs['args'])

        data = []
        for host in datamap.iterkeys():
            for filename in datamap.getStringVector(host):
                data.append((host, filename))

        #       If an imager process fails, set the error Event & bail out later
        # ----------------------------------------------------------------------
        self.error = threading.Event()
        self.error.clear()

        command = "python %s" % (self.__file__.replace('master', 'nodes'))
        with clusterlogger(self.logger) as (loghost, logport):
            with utilities.log_time(self.logger):
                self.logger.debug("Logging to %s:%d" % (loghost, logport))
                sourcedb_threads = []
                for host, ms in data:
                    sourcedb_threads.append(
                        threading.Thread(
                            target=self._run_sourcedb_node,
                            args=(host, command, loghost, str(logport),
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

    def _run_sourcedb_node(
        self, host, command, loghost, logport, executable, ms, skymodel
    ):
        sourcedb_process = run_remote_command(
            host,
            command,
            {
                "PYTHONPATH": self.config.get('deploy', 'engine_ppath'),
                "LD_LIBRARY_PATH": self.config.get('deploy', 'engine_lpath')
            },
            loghost,
            logport,
            executable,
            ms,
            skymodel
        )
        sout, serr = sourcedb_process.communicate()
        if sourcedb_process.returncode != 0:
            self.error.set()
        return sourcedb_process.returncode


if __name__ == '__main__':
    sys.exit(sourcedb().main())
