from __future__ import with_statement
import sys, os

import lofarpipe.support.utilities as utilities
from lofarpipe.support.ipython import LOFARTask
from lofarpipe.support.lofarrecipe import LOFARrecipe
from lofarpipe.support.clusterlogger import clusterlogger
from lofarpipe.support.lofarnode import run_node

class pyraprunner(LOFARrecipe):
    """
    Provides all the basic infrastructure for applying a pyrap-based filter to
    code on the cluster, distributed using an IPython task client.
    """
    def __init__(self):
        super(pyraprunner, self).__init__()
        self.optionparser.add_option(
            '--suffix',
            dest="suffix",
            help="Suffix to add to trimmed data (default: overwrite existing)"
        )

    def _generate_arguments(self):
        return ''

    def go(self):
        super(pyraprunner, self).go()

        ms_names = self.inputs['args']

        tc, mec = self._get_cluster()
        function_name = self.__class__.__name__ + "_remote"
        mec.push_function(
            {
                function_name: run_node,
                "build_available_list": utilities.build_available_list,
                "clear_available_list": utilities.clear_available_list
            }
        )
        self.logger.info("Pushed functions to cluster")

        self.logger.info("Building list of data available on engines")
        available_list = "%s%s" % (
            self.inputs['job_name'], self.__class__.__name__
        )
        mec.push(dict(filenames=ms_names))
        mec.execute(
            "build_available_list(\"%s\")" % (available_list,)
        )

        with clusterlogger(self.logger) as (loghost, logport):
            self.logger.debug("Logging to %s:%d" % (loghost, logport))
            tasks = []
            outnames = []
            for ms_name in ms_names:
                outnames.append(ms_name + self.inputs['suffix'])
                execute_string = "result = %s(ms_name, \"%s\", %s)" % (
                    function_name, outnames[-1], self._generate_arguments()
                )
                task = LOFARTask(
                    execute_string,
                    push=dict(
                        recipename=self.name,
                        nodepath=os.path.dirname(self.__file__.replace('master', 'nodes')),
                        ms_name=ms_name,
                        loghost=loghost,
                        logport=logport
                    ),
                    pull="result",
                    depend=utilities.check_for_path,
                    dependargs=(ms_name, available_list)
                )
                self.logger.info("Scheduling processing of %s" % (ms_name,))
                tasks.append(tc.run(task))
            self.logger.info(
                "Waiting for all %s tasks to complete" %
                (self.__class__.__name__)
            )
            tc.barrier(tasks)


        failure = False
        for task in tasks:
            res = tc.get_task_result(task)
            if res.failure:
                self.logger.warn("Task %s failed" % (task))
                self.logger.warn(res)
                self.logger.warn(res.failure.getTraceback())
                failure = True
        if failure:
            return 1

        mec.execute("clear_available_list(\"%s\")" % (available_list,))
        self.outputs['data'] = outnames
        return 0

if __name__ == '__main__':
    sys.exit(pyraprunner().main())
