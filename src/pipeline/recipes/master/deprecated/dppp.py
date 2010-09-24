from __future__ import with_statement
import sys, os

import lofarpipe.support.utilities as utilities
from lofarpipe.support.lofarrecipe import LOFARrecipe
from lofarpipe.support.ipython import LOFARTask
from lofarpipe.support.clusterlogger import clusterlogger
from lofarpipe.support.lofarnode import run_node

class dppp(LOFARrecipe):
    def __init__(self):
        super(dppp, self).__init__()
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
            '--nthreads',
            help="Number of threads per (N)DPPP process",
            default="2"
        )


    def go(self):
        self.logger.info("Starting DPPP run")
        super(dppp, self).go()

        job_directory = self.config.get("layout", "job_directory")
        ms_names = self.inputs['args']

        # Connect to the IPython cluster and initialise it with
        # the funtions we need.
        tc, mec = self._get_cluster()
        mec.push_function(
            dict(
                run_dppp=run_node,
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
        self.logger.debug("Data lists available. Starting processing loop.")

        # clusterlogger context manager accepts networked logging
        # from compute nodes.
        with clusterlogger(self.logger) as (loghost, logport):
            # Timer for total DPPP job execution
            with utilities.log_time(self.logger):
                self.logger.debug("Logging to %s:%d" % (loghost, logport))
                tasks = []
                outnames = []
                # Iterate over SB names, building and scheduling a DPPP job
                # for each one.
                for ms_name in ms_names:
                    outnames.append(
                        os.path.join(
                            self.inputs['working_directory'],
                            self.inputs['job_name'],
                            os.path.basename(ms_name) + self.inputs['suffix']
                        )
                    )
                    task = LOFARTask(
                        "result = run_dppp(ms_name, ms_outname, parset, executable, initscript, start_time, end_time, nthreads)",
                        push=dict(
                            recipename=self.name,
                            nodepath=os.path.dirname(self.__file__.replace('master', 'nodes')),
                            ms_name=ms_name,
                            ms_outname=outnames[-1],
                            parset=self.inputs['parset'],
                            executable=self.inputs['executable'],
                            initscript=self.inputs['initscript'],
                            start_time=self.inputs['data_start_time'],
                            end_time=self.inputs['data_end_time'],
                            end_time=self.inputs['nthreads'],
                            loghost=loghost,
                            logport=logport
                        ),
                        pull="result",
                        depend=utilities.check_for_path,
                        dependargs=(ms_name, available_list)
                    )
                    self.logger.info("Scheduling processing of %s" % (ms_name,))
                    if self.inputs['dry_run'] == "False":
                        self.inputs['dry_run'] = False
                    if not self.inputs['dry_run']:
                        tasks.append((tc.run(task), ms_name))
                    else:
                        self.logger.info("Dry run: scheduling skipped")

                # Wait for all jobs to finish
                self.logger.debug("Waiting for all DPPP tasks to complete")
                tc.barrier([task for task, subband in tasks])

        failure = False
        for task, subband in tasks:
            ##### Print failing tasks?
            ##### Abort if all tasks failed?
            res = tc.get_task_result(task)
            if res.failure:
                self.logger.warn("Task %s failed (processing %s)" % (task, subband))
                self.logger.warn(res)
                self.logger.warn(res.failure.getTraceback())
                failure = True
        if failure:
            return 1
        self.outputs['data'] = outnames
        return 0

if __name__ == '__main__':
    sys.exit(dppp().main())
