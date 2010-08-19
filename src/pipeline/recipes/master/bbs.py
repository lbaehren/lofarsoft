#                                                         LOFAR IMAGING PIPELINE
#
#                                                BBS (BlackBoard Selfcal) recipe
#                                                         John Swinbank, 2009-10
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from __future__ import with_statement
from contextlib import closing
import psycopg2
import subprocess
import sys
import os
import threading
import tempfile
import shutil
import time
import signal

from lofarpipe.support.lofarrecipe import LOFARrecipe
from lofarpipe.support.lofarnode import run_node
from lofarpipe.support.clusterlogger import clusterlogger
from lofarpipe.support.group_data import gvds_iterator
from lofarpipe.support.pipelinelogging import CatchLog4CPlus
from lofarpipe.support.remotecommand import run_remote_command
import lofarpipe.support.utilities as utilities

class bbs(LOFARrecipe):
    """
    Provides a convenient, pipeline-based mechanism of running the BBS system
    on a dataset.

    Includes setting up source and parameter databases, selecting subbands for
    simultaneous processing, and orchestrating the running of GlobalControl
    and KernelControl executables.
    """
    def __init__(self):
        super(bbs, self).__init__()
        self.optionparser.add_option(
            '--control-exec',
            dest="control_exec",
            help="BBS Control executable"
        )
        self.optionparser.add_option(
            '--kernel-exec',
            dest="kernel_exec",
            help="BBS Kernel executable"
        )
        self.optionparser.add_option(
            '--initscript',
            dest="initscript",
            help="Initscript to source (ie, lofarinit.sh)"
        )
        self.optionparser.add_option(
            '-p', '--parset',
            dest="parset",
            help="BBS configuration parset"
        )
        self.optionparser.add_option(
            '--key',
            dest="key",
            help="Key to identify BBS session"
        )
        self.optionparser.add_option(
            '--db-host',
            dest="db_host",
            help="Database host with optional port"
        )
        self.optionparser.add_option(
            '--db-user',
            dest="db_user",
            help="Database user"
        )
        self.optionparser.add_option(
            '--db-name',
            dest="db_name",
            help="Database name"
        )
        self.optionparser.add_option(
            '--combinevds',
            help="combinevds executable",
            default="/opt/LofIm/daily/lofar/bin/combinevds"
        )
        self.optionparser.add_option(
            '--nproc',
            help="Maximum number of simultaneous processes per compute node",
            default="4"
        )

    def go(self):
        self.logger.info("Starting BBS run")
        super(bbs, self).go()

        #             Generate source and parameter databases for all input data
        # ----------------------------------------------------------------------
        self.run_task("parmdb", self.inputs['args'])
        self.run_task("sourcedb", self.inputs['args'])

        #              Build a GVDS file describing all the data to be processed
        # ----------------------------------------------------------------------
        self.logger.debug("Building VDS file describing all data for BBS")
        vds_file = os.path.join(
            self.config.get("layout", "vds_directory"), "bbs.gvds"
        )
        self.run_task('vdsmaker', self.inputs['args'], gvds=vds_file, unlink="False")
        self.logger.debug("BBS GVDS is %s" % (vds_file,))


        #      Iterate over groups of subbands divided up for convenient cluster
        #          procesing -- ie, no more than nproc subbands per compute node
        # ----------------------------------------------------------------------
        self.logger.debug("Building VDS file describing all data for BBS")
        for to_process in gvds_iterator(vds_file, int(self.inputs["nproc"])):
            #               to_process is a list of (host, filename, vds) tuples
            # ------------------------------------------------------------------
            hosts, ms_names, vds_files = map(list, zip(*to_process))

            #             The BBS session database should be cleared for our key
            # ------------------------------------------------------------------
            self.logger.debug(
                "Cleaning BBS database for key %s" % (self.inputs["key"])
            )
            with closing(
                psycopg2.connect(
                    host=self.inputs["db_host"],
                    user=self.inputs["db_user"],
                    database=self.inputs["db_name"]
                )
            ) as db_connection:
                db_connection.set_isolation_level(
                    psycopg2.extensions.ISOLATION_LEVEL_AUTOCOMMIT
                )
                with closing(db_connection.cursor()) as db_cursor:
                    db_cursor.execute(
                        "DELETE FROM blackboard.session WHERE key=%s",
                        (self.inputs["key"],)
                    )

            #     BBS GlobalControl requires a GVDS file describing all the data
            #          to be processed. We assemble that from the separate parts
            #                                         already available on disk.
            # ------------------------------------------------------------------
            self.logger.debug("Building VDS file describing data for BBS run")
            vds_dir = tempfile.mkdtemp()
            vds_file = os.path.join(vds_dir, "bbs.gvds")
            combineproc = subprocess.Popen(
                [
                    self.inputs['combinevds'],
                    vds_file,
                ] + vds_files,
                stdout = subprocess.PIPE,
                stderr = subprocess.PIPE
            )
            sour, serr = combineproc.communicate()
            if combineproc.returncode != 0:
                raise subprocess.CalledProcessError(
                    combineproc.returncode, command
                )

            #      Construct a parset for BBS GlobalControl by patching the GVDS
            #           file and database information into the supplied template
            # ------------------------------------------------------------------
            self.logger.debug("Building parset for BBS control")
            bbs_parset = utilities.patch_parset(
                self.inputs['parset'],
                {
                    'Observation': vds_file,
                    'BBDB.Key': self.inputs['key'],
                    'BBDB.Name': self.inputs['db_name'],
                    'BBDB.User': self.inputs['db_user'],
                    'BBDB.Host': self.inputs['db_host'],
    #                'BBDB.Port': self.inputs['db_name'],
                }
            )
            self.logger.debug("BBS control parset is %s" % (bbs_parset,))

            try:
                #        When one of our processes fails, we set the killswitch.
                #      Everything else will then come crashing down, rather than
                #                                         hanging about forever.
                # --------------------------------------------------------------
                self.killswitch = threading.Event()
                self.killswitch.clear()

                #                           GlobalControl runs in its own thread
                # --------------------------------------------------------------
                run_flag = threading.Event()
                run_flag.clear()
                bbs_control = threading.Thread(
                    target=self._run_bbs_control,
                    args=(bbs_parset, run_flag)
                )
                bbs_control.start()
                run_flag.wait()    # Wait for control to start before proceeding

                #      We run BBS KernelControl on each compute node by directly
                #                             invoking the node script using SSH
                # --------------------------------------------------------------
                command = "python %s" % (self.__file__.replace('master', 'nodes'))
                with clusterlogger(self.logger) as (loghost, logport):
                    self.logger.debug("Logging to %s:%d" % (loghost, logport))
                    with utilities.log_time(self.logger):
                        #               Each SSH process runs in its own thread
                        # -----------------------------------------------------
                        bbs_kernels = [
                            threading.Thread(
                                target=self._run_bbs_kernel,
                                args=(host, command,
                                    loghost, str(logport),
                                    self.inputs['kernel_exec'],
                                    self.inputs['initscript'],
                                    file,
                                    self.inputs['key'],
                                    self.inputs['db_name'],
                                    self.inputs['db_user'],
                                    self.inputs['db_host']
                                )
                            )
                            for host, file, vds in to_process
                        ]
                        [thread.start() for thread in bbs_kernels]
                        self.logger.debug("Waiting for all kernels to complete")
                        [thread.join() for thread in bbs_kernels]

                    #         When GlobalControl finishes, our work here is done
                    # ----------------------------------------------------------
                    self.logger.info("Waiting for GlobalControl thread")
                    bbs_control.join()
            finally:
                os.unlink(bbs_parset)
                shutil.rmtree(vds_dir)
                if self.killswitch.isSet():
                    #  If killswitch is set, then one of our processes failed so
                    #                                   the whole run is invalid
                    # ----------------------------------------------------------
                    return 1

        self.outputs['data'] = self.inputs['args']
        return 0

    def _run_bbs_kernel(self, host, command, *arguments):
        """
        Run command with arguments on the specified host using ssh. Return its
        return code.

        The resultant process is monitored for failure; see
        _monitor_process() for details.
        """
        bbs_kernel_process = run_remote_command(
            host,
            command,
            {
                "PYTHONPATH": self.config.get('deploy', 'engine_ppath'),
                "LD_LIBRARY_PATH": self.config.get('deploy', 'engine_lpath')
            },
            *arguments
        )
        return(self._monitor_process(bbs_kernel_process, "BBS Kernel"))

    def _run_bbs_control(self, bbs_parset, run_flag):
        """
        Run BBS Global Control and wait for it to finish. Return its return
        code.
        """
        env = utilities.read_initscript(self.inputs['initscript'])
        self.logger.info("Running BBS GlobalControl")
        working_dir = tempfile.mkdtemp()
        with CatchLog4CPlus(
            working_dir,
            self.logger.name + ".GlobalControl",
            os.path.basename(self.inputs['control_exec'])
        ):
            with utilities.log_time(self.logger):
                bbs_control_process = subprocess.Popen(
                    [
                        self.inputs['control_exec'],
                        bbs_parset,
                        "0"
                    ],
                    env=env,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                    cwd=working_dir
                )
                run_flag.set()

            returncode = self._monitor_process(
                bbs_control_process, "BBS Control"
            )
            sout, serr = bbs_control_process.communicate()
        shutil.rmtree(working_dir)
        self.logger.info("Global Control stdout: %s" % (sout,))
        self.logger.info("Global Control stderr: %s" % (serr,))
        return returncode

    def _monitor_process(self, process, name="Monitored process"):
        """
        Monitor a process for successful exit. If it fails, set the kill
        switch, so everything else gets killed too. If the kill switch is set,
        then kill this process off.

        Name is an optional parameter used only for identification in logs.
        """
        while True:
            returncode = process.poll()
            if returncode == None:                       # Process still running
                time.sleep(1)
            elif returncode != 0:                               # Process broke!
                self.logger.warn(
                    "%s returned code %d; aborting run" % (name, returncode)
                )
                self.killswitch.set()
                break
            else:                                       # Process exited cleanly
                self.logger.info("%s clean shutdown" % (name))
                break
            if self.killswitch.isSet():            # Other process failed; abort
               self.logger.warn("Killing %s" % (name))
               os.kill(process.pid, signal.SIGKILL)
               returncode = process.wait()
               break
        return returncode

if __name__ == '__main__':
    sys.exit(bbs().main())
