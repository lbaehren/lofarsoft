from __future__ import with_statement
import subprocess, sys, time
from lofarpipe.support.lofarrecipe import LOFARrecipe
from lofarpipe.support.ipython import LOFARTask
from lofarpipe.support.clusterlogger import clusterlogger

def run_dummy_echo(filename, executable):
    import imp, os
    dummy_echo_node = imp.load_module('dummy_echo_parallel', *imp.find_module('dummy_echo_parallel', [os.path.dirname(recipe.replace('master', 'nodes'))])).dummy_echo_node
    return dummy_echo_node(loghost=loghost, logport=logport).run(filename, executable)

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
                run_dummy_echo=run_dummy_echo,
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
                        recipe=self.__file__,
                        filename=filename,
                        executable=self.inputs['executable'],
                        loghost=loghost,
                        logport=logport
                    ),
                    pull="result"
                )
                self.logger.info("Scheduling processing of %s" % (filename))
                tasks.append(tc.run(task))
                time.sleep(0.1) # Tiny delay to avoid everything starting logging at once.

        self.logger.info("Waiting for all dummy_echo tasks to complete")
        tc.barrier(tasks)

        for task in tasks:
            result = tc.get_task_result(task)
            print result
            if result.failure:
                self.logger.warn(result)
                self.logger.warn(result.failure.getTraceback())

        return 0

if __name__ == '__main__':
    sys.exit(dummy_echo_parallel().main())
