from __future__ import with_statement
import sys, os

from lofarpipe.support.lofarrecipe import LOFARrecipe
from lofarpipe.support.ipython import LOFARTask
from lofarpipe.support.clusterlogger import clusterlogger
from lofarpipe.support.lofarnode import run_node

class dummy_echo_parallel(LOFARrecipe):
    def __init__(self):
        super(dummy_echo_parallel, self).__init__()
        self.optionparser.add_option(
            '--executable',
            dest="executable",
            help="Executable to be run (ie, dummy_echo.sh)",
            default="/home/swinbank/sw/bin/dummy_echo.sh"
        )

    def go(self):
        self.logger.info("Starting dummy_echo run")
        super(dummy_echo_parallel, self).go()

        # Connect to the IPython cluster and initialise it with the functions
        # we need.
        tc, mec = self._get_cluster()
        mec.push_function(
            dict(
                run_dummy_echo=run_node,
            )
        )
        self.logger.info("Cluster initialised")

        with clusterlogger(self.logger) as (loghost, logport):
            self.logger.debug("Logging to %s:%d" % (loghost, logport))
            tasks = [] # this will be a list of scheduled jobs
            for filename in self.inputs['args']:
                task = LOFARTask(
                    "result = run_dummy_echo(filename, executable)",
                    push = dict(
                        recipename=self.name,
                        nodepath=os.path.dirname(self.__file__.replace('master', 'nodes')),
                        filename=filename,
                        executable=self.inputs['executable'],
                        loghost=loghost,
                        logport=logport
                    ),
                    pull="result"
                )
                self.logger.info("Scheduling processing of %s" % (filename))
                tasks.append(tc.run(task))
            self.logger.info("Waiting for all dummy_echo tasks to complete")
            tc.barrier(tasks)

        for task in tasks:
            result = tc.get_task_result(task)
            if result.failure:
                self.logger.warn(result)
                self.logger.warn(result.failure.getTraceback())

        return 0

if __name__ == '__main__':
    sys.exit(dummy_echo_parallel().main())
