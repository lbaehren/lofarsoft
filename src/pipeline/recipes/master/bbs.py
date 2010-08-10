from __future__ import with_statement
from contextlib import closing
import psycopg2
import subprocess
import sys
import os

# Local helpers
from lofarpipe.support.lofarrecipe import LOFARrecipe
from lofarpipe.support.lofarnode import run_node
from lofarpipe.support.ipython import LOFARTask
from lofarpipe.support.clusterlogger import clusterlogger
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

    def go(self):
        self.logger.info("Starting BBS run")
        super(bbs, self).go()

        ms_names = self.inputs['args']

        # First: clean the database
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

        # Second, build a VDS file describing the data to be processed
        self.logger.debug("Building VDS file for BBS")
        vds_file = os.path.join(
            self.config.get("layout", "vds_directory"), "bbs.gvds"
        )
        self.run_task('vdsmaker', ms_names, gvds=vds_file)
        self.logger.debug("BBS VDS is %s" % (vds_file,))

        # Third, construct a parset for BBS control
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

        # Fourth, start BBS kernels
        tc, mec = self._get_cluster()
        mec.push_function(
            dict(
                run_bbs=run_node,
                build_available_list=utilities.build_available_list,
                clear_available_list=utilities.clear_available_list
            )
        )
        self.logger.debug("Pushed functions to cluster")

        # Use build_available_list() to determine which SBs are available
        # on each engine; we use this for dependency resolution later.
        self.logger.debug("Building list of data available on engines")
        available_list = "%s%s" % (
            self.inputs['job_name'], self.__class__.__name__
        )
        mec.push(dict(filenames=ms_names))
        mec.execute(
            "build_available_list(\"%s\")" % (available_list,)
        )

        with clusterlogger(self.logger) as (loghost, logport):
            with utilities.log_time(self.logger):
                self.logger.debug("Logging to %s:%d" % (loghost, logport))
                tasks = []
                for ms_name in ms_names:
                    task = LOFARTask(
                        "result = run_bbs(executable, initscript, ms_name, key, db_name, db_user, db_host)",
                        push=dict(
                            recipename=self.name,
                            nodepath=os.path.dirname(self.__file__.replace('master', 'nodes')),
                            ms_name=ms_name,
                            executable=self.inputs['kernel_exec'],
                            initscript=self.inputs['initscript'],
                            key=self.inputs['key'],
                            db_name=self.inputs['db_name'],
                            db_user=self.inputs['db_user'],
                            db_host=self.inputs['db_host'],
                            loghost=loghost,
                            logport=logport
                        ),
                        pull="result",
                        depend=utilities.check_for_path,
                        dependargs=(ms_name, available_list)
                    )
                    self.logger.info("Scheduling processing of %s" % (ms_name,))
                    tasks.append((tc.run(task), ms_name))

                # Fifth, run BBS control & wait for it to finish
                env = utilities.read_initscript(self.inputs['initscript'])
                try:
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
                        self.logger.info("Waiting for GlobalControl")
                        sout, serr = bbs_control_process.communicate()
                    if bbs_control_process.returncode != 0:
                        raise subprocess.CalledProcessError(
                            bbs_control_process.returncode,
                            self.inputs['control_exec']
                        )
                finally:
                    os.unlink(bbs_parset)
                    os.unlink(vds_file)

                self.logger.debug("Waiting for all sourcedb tasks to complete")
                tc.barrier([task for task, subband in tasks])

        # Finally, wait for kernels to finish
        failure = False
        for task, subband in tasks:
            res = tc.get_task_result(task)
            if res.failure:
                self.logger.warn("Task %s failed (processing %s)" % (task, subband))
                self.logger.warn(res)
                self.logger.warn(res.failure.getTraceback())
                failure = True
        if failure:
            return 1
        self.outputs['data'] = self.inputs['args']
        return 0


if __name__ == '__main__':
    sys.exit(bbs().main())
