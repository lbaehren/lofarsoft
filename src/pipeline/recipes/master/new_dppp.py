#                                                         LOFAR IMAGING PIPELINE
#
#                                         New DPPP recipe: fixed node allocation
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from __future__ import with_statement

from itertools import cycle

import subprocess
import collections
import threading
import sys
import os

import lofarpipe.support.utilities as utilities
from lofarpipe.support.lofarrecipe import LOFARrecipe
from lofarpipe.support.clusterlogger import clusterlogger
from lofarpipe.support.clusterdesc import ClusterDesc, get_compute_nodes
from lofarpipe.cuisine.parset import Parset

class new_dppp(LOFARrecipe):
    def __init__(self):
        super(new_dppp, self).__init__()
        self.optionparser.add_option(
            '--executable',
            dest="executable",
            help="DPPP executable"
        )
        self.optionparser.add_option(
            '--initscript',
            dest="initscript",
            help="DPPP initscript"
        )
        self.optionparser.add_option(
            '-p', '--parset',
            dest="parset",
            help="Parset containing configuration for DPPP"
        )
        self.optionparser.add_option(
            '--suffix',
            dest="suffix",
            default=".dppp",
            help="Suffix to add to trimmed data (default: overwrite existing)"
        )
        self.optionparser.add_option(
            '-w', '--working-directory',
            dest="working_directory",
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
            default="4"
        )
        self.optionparser.add_option(
            '--nthreads',
            help="Number of threads per (N)DPPP process",
            default="2"
        )


    def go(self):
        self.logger.info("Starting DPPP run")
        super(new_dppp, self).go()

        #                           Load file <-> compute node mapping from disk
        # ----------------------------------------------------------------------
        self.logger.debug("Loading map from %s" % self.inputs['args'])
        datamap = Parset(self.inputs['args'])

        data = []
        for host in datamap.iterkeys():
            for filename in datamap.getStringVector(host):
                data.append((host, filename))

        #   BoundedSempaphores will manage the number of simulataneous jobs/node
        # ----------------------------------------------------------------------
        compute_nodes_lock = collections.defaultdict(
            lambda: threading.BoundedSemaphore(int(self.inputs['nproc']))
        )

        #       If an imager process fails, set the error Event & bail out later
        # ----------------------------------------------------------------------
        self.error = threading.Event()
        self.error.clear()

        #       We can use the same node script as the "old" IPython dppp recipe
        # ----------------------------------------------------------------------
        command = "python %s" % (
            self.__file__.replace('master', 'nodes').replace('new_dppp', 'dppp')
        )
        with clusterlogger(self.logger) as (loghost, logport):
            self.logger.debug("Logging to %s:%d" % (loghost, logport))
            with utilities.log_time(self.logger):
                dppp_threads = []
                outnames = []
                for host, ms in data:
                    outnames.append(
                        os.path.join(
                            self.inputs['working_directory'],
                            self.inputs['job_name'],
                            os.path.basename(ms) + self.inputs['suffix']
                        )
                    )
                    dppp_threads.append(
                        threading.Thread(
                            target=self._run_dppp_node,
                            args=(host, compute_nodes_lock[host], command,
                                loghost, str(logport),
                                ms,
                                outnames[-1],
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
            self.logger.warn("Failed imager process detected")
            return 1
        else:
            self.outputs['data'] = outnames
            return 0

    def _run_dppp_node(
        self, host, semaphore, command, loghost, logport, infile, outfile,
        parset, executable, initscript, start_time, end_time, nthreads
    ):
        semaphore.acquire()
        try:
            #                                   Run DPPP process on compute node
            # ------------------------------------------------------------------
            engine_ppath = self.config.get('deploy', 'engine_ppath')
            engine_lpath = self.config.get('deploy', 'engine_lpath')
            ssh_cmd = [
                "ssh", "-n", "-t", "-x", host, "--",
                "PYTHONPATH=%s" % engine_ppath,
                "LD_LIBRARY_PATH=%s" % engine_lpath,
                command
            ]
            ssh_cmd.extend(
                [
                    loghost,
                    logport,
                    infile,
                    outfile,
                    parset,
                    executable,
                    initscript,
                    start_time,
                    end_time,
                    nthreads
                ]
            )
            self.logger.info("Running %s" % " ".join(ssh_cmd))
            dppp_process = subprocess.Popen(ssh_cmd)
            sout, serr = dppp_process.communicate()
        finally:
            semaphore.release()

        if dppp_process.returncode != 0:
            self.error.set()
        return dppp_process.returncode

if __name__ == '__main__':
    sys.exit(new_dppp().main())
