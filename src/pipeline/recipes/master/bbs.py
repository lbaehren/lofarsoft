from __future__ import with_statement
from contextlib import closing
import psycopg2
import subprocess
import sys
import os
import threading
import tempfile
import shutil

# Local helpers
from lofarpipe.support.lofarrecipe import LOFARrecipe
from lofarpipe.support.lofarnode import run_node
from lofarpipe.support.ipython import LOFARTask
from lofarpipe.support.clusterlogger import clusterlogger
import lofarpipe.support.utilities as utilities

from lofar.parameterset import parameterset
from collections import defaultdict

def gvds_iterator(gvds_file, nproc=4):
    """
    Reads a GVDS file.

    Provides a generator, which successively returns the contents of the GVDS
    file in the form (host, filename), in chunks suitable for processing
    across the cluster. Ie, no more than nproc files per host at a time.
    """
    parset = parameterset(gvds_file)

    data = defaultdict(list)
    for part in range(parset.getInt('NParts')):
        host = parset.getString("Part%d.FileSys" % part).split(":")[0]
        file = parset.getString("Part%d.FileName" % part)
        vds  = parset.getString("Part%d.Name" % part)
        data[host].append((file, vds))

    for host, values in data.iteritems():
        data[host] = utilities.group_iterable(values, nproc)

    while True:
        yieldable = []
        for host, values in data.iteritems():
            try:
                for filename, vds in values.next():
                    yieldable.append((host, filename, vds))
            except StopIteration:
                pass
        if len(yieldable) == 0:
            raise StopIteration
        else:
            yield yieldable


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

    def go(self):
        self.logger.info("Starting BBS run")
        super(bbs, self).go()

        ms_names = self.inputs['args']

        # Build a VDS file describing all the data to be processed
        self.logger.debug("Building VDS file describing all data for BBS")
        vds_file = os.path.join(
            self.config.get("layout", "vds_directory"), "bbs.gvds"
        )
        self.run_task('vdsmaker', ms_names, gvds=vds_file, unlink="False")
        self.logger.debug("BBS VDS is %s" % (vds_file,))

        # Iterate over groups in the VDS file for suitable for cluster
        # processing
        for to_process in gvds_iterator(vds_file):
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
                    "/opt/LofIm/daily/lofar/bin/combinevds",
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

            # Run BBS control in a separate thread.
            run_flag = threading.Event()
            run_flag.clear()
            bbs_control = threading.Thread(
                target=self._run_bbs_control,
                args=(bbs_parset, run_flag)
            )
            bbs_control.start()
            run_flag.wait() # Wait for control to start before proceeding

            command = "python /opt/pipeline/recipes/nodes/bbs.py"
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
            try:
                bbs_control.join()
            finally:
                os.unlink(bbs_parset)
                shutil.rmtree(vds_dir)

        self.outputs['data'] = self.inputs['args']
        return 0

    def _run_via_ssh(self, host, command, *arguments):
        engine_ppath = self.config.get('deploy', 'engine_ppath')
        engine_lpath = self.config.get('deploy', 'engine_lpath')
        ssh_cmd = [
            "ssh", "-x", host, "--",
            "PYTHONPATH=%s" % engine_ppath,
            "LD_LIBRARY_PATH=%s" % engine_lpath,
            command
        ]
        ssh_cmd.extend(arguments)
        self.logger.info("Running %s" % " ".join(ssh_cmd))
        subprocess.check_call(ssh_cmd)

    def _run_bbs_control(self, bbs_parset, run_flag):
        # Run BBS control & wait for it to finish
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
            self.logger.info("Waiting for GlobalControl")
            sout, serr = bbs_control_process.communicate()
        self.logger.info("Global Control stdout: %s" % (sout,))
        self.logger.info("Global Control stderr: %s" % (serr,))
        if bbs_control_process.returncode != 0:
            raise subprocess.CalledProcessError(
                bbs_control_process.returncode,
                self.inputs['control_exec']
            )


if __name__ == '__main__':
    sys.exit(bbs().main())
