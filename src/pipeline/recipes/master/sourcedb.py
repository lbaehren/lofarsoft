from __future__ import with_statement
import os

# Local helpers
from lofarpipe.support.lofarrecipe import LOFARrecipe
from lofarpipe.support.ipython import LOFARTask
from lofarpipe.support.clusterlogger import clusterlogger
from lofarpipe.support.lofarnode import run_node
import lofarpipe.support.utilities as utilities

class sourcedb(LOFARrecipe):
    def __init__(self):
        super(sourcedb, self).__init__()
        self.optionparser.add_option(
            '--executable',
            help="Executable for makesourcedb"
        )
        self.optionparser.add_option(
            '-s', '--skymodel',
            dest="skymodel",
            help="Input sky catalogue"
        )

    def go(self):
        self.logger.info("Starting sourcedb run")
        super(sourcedb, self).go()

        ms_names = self.inputs['args']

        # Connect to the IPython cluster and initialise it with
        # the funtions we need.
        tc, mec = self._get_cluster()
        mec.push_function(
            dict(
                run_sourcedb=run_node,
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
                        "result = run_sourcedb(executable, ms_name, catalogue)",
                        push=dict(
                            recipename=self.name,
                            nodepath=os.path.dirname(self.__file__.replace('master', 'nodes')),
                            ms_name=ms_name,
                            executable=self.inputs['executable'],
                            catalogue=self.inputs['skymodel'],
                            loghost=loghost,
                            logport=logport
                        ),
                        pull="result",
                        depend=utilities.check_for_path,
                        dependargs=(ms_name, available_list)
                    )
                    self.logger.info("Scheduling processing of %s" % (ms_name,))
                    tasks.append((tc.run(task), ms_name))

                self.logger.debug("Waiting for all sourcedb tasks to complete")
                tc.barrier([task for task, subband in tasks])

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
    sys.exit(sourcedb().main())
