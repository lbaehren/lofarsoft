#                                                         LOFAR IMAGING PIPELINE
#
#                                                  cimager (ASKAP imager) recipe
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from __future__ import with_statement

import os
import sys
import threading
import collections
import subprocess

from lofar.parameterset import parameterset

from lofarpipe.support.lofarrecipe import LOFARrecipe
from lofarpipe.support.pipelinelogging import log_time
from lofarpipe.support.clusterlogger import clusterlogger


class cimager(LOFARrecipe):
    """
    Provides a convenient, pipeline-based mechanism of running the cimager on
    a dataset.

    Ingests an MWimager-style parset, converting it to cimager format as
    required.
    """
    def __init__(self):
        super(cimager, self).__init__()
        self.optionparser.add_option(
            '--imager-exec',
            help="cimager executable"
        )
        self.optionparser.add_option(
            '--convert-exec',
            help="convertimagerparset executable"
        )
        self.optionparser.add_option(
            '--parset',
            help="Imager configuration parset (mwimager format)"
        )
        self.optionparser.add_option(
            '--nproc',
            help="Maximum number of simultaneous processes per compute node",
            default="8"
        )


    def go(self):
        self.logger.info("Starting cimager run")
        super(cimager, self).go()
        ms_names = self.inputs['args']

        #              Build a GVDS file describing all the data to be processed
        # ----------------------------------------------------------------------
        self.logger.debug("Building VDS file describing all data for cimager")
        gvds_file = os.path.join(
            self.config.get("layout", "vds_directory"), "cimager.gvds"
        )
        self.run_task('vdsmaker', ms_names, gvds=gvds_file, unlink="False")
        self.logger.debug("cimager GVDS is %s" % (gvds_file,))

        #                            Read data for processing from the GVDS file
        # ----------------------------------------------------------------------
        parset = parameterset(gvds_file)

        data = []
        for part in range(parset.getInt('NParts')):
            host = parset.getString("Part%d.FileSys" % part).split(":")[0]
            vds  = parset.getString("Part%d.Name" % part)
            data.append((host, vds))

        #   BoundedSempaphores will manage the number of simulataneous jobs/node
        # ----------------------------------------------------------------------
        compute_nodes_lock = collections.defaultdict(
            lambda: threading.BoundedSemaphore(int(self.inputs['nproc']))
        )

        #       If an imager process fails, set the error Event & bail out later
        # ----------------------------------------------------------------------
        self.error = threading.Event()
        self.error.clear()

        #                          Run each cimager process in a separate thread
        # ----------------------------------------------------------------------
        command = "python %s" % (self.__file__.replace('master', 'nodes'))
        with clusterlogger(self.logger) as (loghost, logport):
            self.logger.debug("Logging to %s:%d" % (loghost, logport))
            with log_time(self.logger):
                imager_threads =[
                    threading.Thread(
                        target=self._run_via_ssh,
                        args=(host, compute_nodes_lock[host], command,
                            loghost, str(logport),
                            self.inputs['imager_exec'],
                            self.inputs['convert_exec'],
                            vds,
                            self.inputs['parset'],
                            self.config.get('layout', 'results_directory')
                        )
                    )
                    for host, vds in data
                ]
                [thread.start() for thread in imager_threads]
                self.logger.info("Waiting for imager threads")
                [thread.join() for thread in imager_threads]

        #                Check if we recorded a failing process before returning
        # ----------------------------------------------------------------------
        if self.error.isSet():
            self.logger.warn("Failed imager process detected")
            return 1
        else:
            return 0

    def _run_via_ssh(self, host, semaphore, command, *arguments):
        """
        Run command with arguments on the specified host using ssh. Return its
        return code.

        We use a semaphore to limit the number of simultaneous processes per
        node.
        """
        engine_ppath = self.config.get('deploy', 'engine_ppath')
        engine_lpath = self.config.get('deploy', 'engine_lpath')
        ssh_cmd = [
            "ssh", "-n", "-t", "-x", host, "--",
            "PYTHONPATH=%s" % engine_ppath,
            "LD_LIBRARY_PATH=%s" % engine_lpath,
            command
        ]
        ssh_cmd.extend(arguments)
        semaphore.acquire()
        self.logger.info("Running %s" % " ".join(ssh_cmd))
        try:
            cimager_process = subprocess.Popen(ssh_cmd)
            sout, serr = cimager_process.communicate()
        finally:
            semaphore.release()
        if cimager_process.returncode != 0:
            self.error.set()
        return(cimager_process.returncode)


if __name__ == '__main__':
    sys.exit(cimager().main())
