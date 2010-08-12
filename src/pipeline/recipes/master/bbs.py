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

# Local helpers
from lofarpipe.support.lofarrecipe import LOFARrecipe
from lofarpipe.support.lofarnode import run_node
from lofarpipe.support.ipython import LOFARTask
from lofarpipe.support.clusterlogger import clusterlogger
from lofarpipe.support.group_data import gvds_iterator
import lofarpipe.support.utilities as utilities

class bbs(LOFARrecipe):
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

        ms_names = self.inputs['args']

        # Generate source and parameter databases for all input data
        self.run_task("parmdb", ms_names)
        self.run_task("sourcedb", ms_names)

        # Build a VDS file describing all the data to be processed
        self.logger.debug("Building VDS file describing all data for BBS")
        vds_file = os.path.join(
            self.config.get("layout", "vds_directory"), "bbs.gvds"
        )
        self.run_task('vdsmaker', ms_names, gvds=vds_file, unlink="False")
        self.logger.debug("BBS VDS is %s" % (vds_file,))

        # Iterate over groups in the VDS file for suitable for cluster
        # processing
        for to_process in gvds_iterator(vds_file, int(self.inputs["nproc"])):
            # to_process is a list of (host, filename) tuples.
            ms_names  = [filename for host, filename, vds in to_process]
            vds_files = [vds for host, filename, vds in to_process]

            # Clean the database for this run
            self.logger.debug("Cleaning BBS database for key %s" % (self.inputs["key"]))
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

            # Build a VDS file describing the data for this run
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
                raise subprocess.CalledProcessError(combineproc.returncode, command)

            # Construct a parset for BBS control
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
                # When one of our processes fails, we'll set the killswitch.
                # Everything else will then come crashing down, rather than
                # hanging about forever.
                self.killswitch = threading.Event()
                self.killswitch.clear()

                # Run BBS Global Control in a separate thread.
                run_flag = threading.Event()
                run_flag.clear()
                bbs_control = threading.Thread(
                    target=self._run_bbs_control,
                    args=(bbs_parset, run_flag)
                )
                bbs_control.start()
                run_flag.wait() # Wait for control to start before proceeding

                command = "python %s" % (self.__file__.replace('master', 'nodes'))
                with clusterlogger(self.logger) as (loghost, logport):
                    self.logger.debug("Logging to %s:%d" % (loghost, logport))
                    with utilities.log_time(self.logger):
                        bbs_kernels = [
                            threading.Thread(
                                target=self._run_via_ssh,
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

                    # And wait for control
                    self.logger.info("Waiting for GlobalControl thread")
                    bbs_control.join()
            finally:
                os.unlink(bbs_parset)
                shutil.rmtree(vds_dir)
                if self.killswitch.isSet():
                    # If killswitch is set, then one of our processes failed so the
                    # whole run is invalid.
                    return 1

        self.outputs['data'] = self.inputs['args']
        return 0

    def _run_via_ssh(self, host, command, *arguments):
        """
        Run command with arguments on the specified host using ssh. Return its
        return code.

        The resultant process is monitored for failure; see
        _monitor_process() for details.
        """
        engine_ppath = self.config.get('deploy', 'engine_ppath')
        engine_lpath = self.config.get('deploy', 'engine_lpath')
        ssh_cmd = [
            "ssh", "-t", "-x", host, "--",
            "PYTHONPATH=%s" % engine_ppath,
            "LD_LIBRARY_PATH=%s" % engine_lpath,
            command
        ]
        ssh_cmd.extend(arguments)
        self.logger.info("Running %s" % " ".join(ssh_cmd))
        bbs_kernel_process = subprocess.Popen(ssh_cmd)
        return(self._monitor_process(bbs_kernel_process, "BBS Kernel"))

    def _run_bbs_control(self, bbs_parset, run_flag):
        """
        Run BBS Global Control and wait for it to finish. Return its return
        code.
        """
        env = utilities.read_initscript(self.inputs['initscript'])
        self.logger.info("Running BBS GlobalControl")
        with utilities.log_time(self.logger):
            bbs_control_process = subprocess.Popen(
                [
                    self.inputs['control_exec'],
                    bbs_parset,
                    "0"
                ],
                env=env,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE
            )
            run_flag.set()

        returncode = self._monitor_process(bbs_control_process, "BBS Control")
        sout, serr = bbs_control_process.communicate()
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
            if returncode == None:
                # Process still running
                time.sleep(1)
            elif returncode != 0:
                # Process broke!
                self.logger.warn(
                    "%s returned code %d; aborting run" % (name, returncode)
                )
                self.killswitch.set()
                break
            else:
                self.logger.info("%s clean shutdown" % (name))
                break
            if self.killswitch.isSet():
               # Some other process has failed, so we abort
               self.logger.warn("Killing %s" % (name))
               os.kill(process.pid, signal.SIGKILL)
               returncode = process.wait()
               break
        return returncode

if __name__ == '__main__':
    sys.exit(bbs().main())
