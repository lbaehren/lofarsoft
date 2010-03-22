from __future__ import with_statement
import sys, os

# Local helpers
from lofarpipe.support.lofarrecipe import LOFARrecipe
from lofarpipe.support.lofaringredient import LOFARoutput, LOFARinput
from lofarpipe.support.ipython import LOFARTask
from lofarpipe.support.group_data import group_files
import lofarpipe.support.utilities as utilities
from lofarpipe.support.clusterlogger import clusterlogger
from lofarpipe.support.clusterdesc import ClusterDesc

def run_casapy(infile, parset, start_time, end_time, increment):
    # Run on engine to process data with Casapy
    from lofarrecipe.nodes.casapy import casapy_node
    return casapy_node(loghost=loghost, logport=logport).run(
        infile,
        parset,
        start_time,
        end_time,
        increment
    )

class casapy(LOFARrecipe):
    def __init__(self):
        super(casapy, self).__init__()
        self.optionparser.add_option(
            '--executable',
            dest="executable",
            help="CASApy executable"
        )
        self.optionparser.add_option(
            '-p', '--parset',
            dest="parset",
            help="Parset containing configuration for CASAPY"
        )
        self.optionparser.add_option(
            '-w', '--working-directory',
            dest="working_directory",
            help="Working directory used on compute nodes"
        )
        self.optionparser.add_option(
            '-t', '--increment',
            dest="increment",
            help="Length of each image in seconds"
        )
        self.optionparser.add_option(
            '-g', '--g(v)ds-file',
            dest="gvds",
            help="G(V)DS file describing data to be processed"
        )
        self.optionparser.add_option(
            '--makevds-exec',
            dest="makevds_exec",
            help="makevds executable"
        )
        self.optionparser.add_option(
            '--combinevds-exec',
            dest="combinevds_exec",
            help="combinevds executable"
        )
        self.optionparser.add_option(
            '--max-bands-per-node',
            dest="max_bands_per_node",
            help="Maximum number of subbands to farm out to a given cluster node",
            default="8"
        )

    def go(self):
        self.logger.info("Starting CASApy run")
        super(casapy, self).go()

        job_directory = self.config.get("layout", "job_directory")

        # Connect to the IPython cluster and initialise it with
        # the funtions we need.
        tc, mec = self._get_cluster()
        mec.push_function(
            dict(
                run_casapy=run_casapy,
                build_available_list=utilities.build_available_list,
                clear_available_list=utilities.clear_available_list
            )
        )
        self.logger.info("Pushed functions to cluster")

        # Use build_available_list() to determine which SBs are available
        # on each engine; we use this for dependency resolution later.
        self.logger.info("Building list of data available on engines")
        available_list = "%s%s" % (
            self.inputs['job_name'], self.__class__.__name__
        )
        mec.push(dict(filenames=self.inputs['args']))
        mec.execute(
            "build_available_list(\"%s\")" % (available_list,)
        )

        clusterdesc = ClusterDesc(
            self.config.get('cluster', 'clusterdesc')
        )

        for data_group in group_files(
            self.logger,
            clusterdesc,
            os.path.join(self.inputs['working_directory'], self.inputs['job_name']),
            int(self.inputs['max_bands_per_node']),
            self.inputs['args']
        ):
            self.logger.debug("Processing: " + str(data_group))
            self.logger.info("Calling vdsmaker")
            inputs = LOFARinput(self.inputs)
            inputs['directory'] = self.config.get('layout', 'vds_directory')
            inputs['gvds'] = self.inputs['gvds']
            inputs['args'] = data_group
            inputs['makevds'] = self.inputs['makevds_exec']
            inputs['combinevds'] = self.inputs['combinevds_exec']
            outputs = LOFARoutput()
            if self.cook_recipe('vdsmaker', inputs, outputs):
                self.logger.warn("vdsmaker reports failure")
                return 1


            gvds = utilities.get_parset(
                os.path.join(
                    self.config.get('layout', 'vds_directory'), self.inputs['gvds']
                )
            )
            start_time = gvds['StartTime']
            end_time = gvds['EndTime']
            self.inputs['increment'] = int(self.inputs['increment'])

            # clusterlogger context manager accepts networked logging
            # from compute nodes.
            with clusterlogger(self.logger) as (loghost, logport):
                # Timer for total casapy job execution
                with utilities.log_time(self.logger):
                    self.logger.debug("Logging to %s:%d" % (loghost, logport))
                    tasks = []
                    # Iterate over SB names, building and scheduling a casapy job
                    # for each one.
                    for ms_name in data_group:
                        task = LOFARTask(
                            "result = run_casapy(infile, parset, start_time, end_time, increment)",
                            push=dict(
                                infile=ms_name,
                                parset=self.inputs['parset'],
                                start_time=start_time,
                                end_time=end_time,
                                increment=self.inputs['increment'],
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
                            tasks.append(tc.run(task))
                        else:
                            self.logger.info("Dry run: scheduling skipped")

                    # Wait for all jobs to finish
                    self.logger.info("Waiting for all CASApy tasks to complete")
                    tc.barrier(tasks)

            failure = False
            for task in tasks:
                ##### Print failing tasks?
                ##### Abort if all tasks failed?
                res = tc.get_task_result(task)
                if res.failure:
                    self.logger.warn("Task %s failed" % (task))
                    self.logger.warn(res)
                    self.logger.warn(res.failure.getTraceback())
                    failure = True

        if failure:
            return 1
#            self.outputs['data'] = outnames
        return 0

if __name__ == '__main__':
    sys.exit(casapy().main())
