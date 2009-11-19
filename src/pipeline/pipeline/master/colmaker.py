import sys, os, tempfile
from subprocess import check_call

from pipeline.support.ipython import LOFARTask
from pipeline.support.lofarrecipe import LOFARrecipe
import pipeline.support.utilities as utilities
from pipeline.support.clusterlogger import clusterlogger

def make_columns(file):
    from pipeline.nodes.colmaker import makecolumns_node
    return makecolumns_node(loghost=loghost, logport=logport).run(file)

class colmaker(LOFARrecipe):
    """
    Add imaging columns to inputs using pyrap.
    """
    def go(self):
        super(colmaker, self).go()

        ms_names = self.inputs['args']

        tc, mec = self._get_cluster()
        mec.push_function(
            dict(
                make_columns=make_columns,
                build_available_list=utilities.build_available_list,
                clear_available_list=utilities.clear_available_list
            )
        )
        self.logger.info("Pushed functions to cluster")

        # Build VDS files for each of the newly created MeasurementSets
        self.logger.info("Building list of data available on engines")
        available_list = "%s%s" % (self.inputs['job_name'], "colmaker")
        mec.push(dict(filenames=ms_names))
        mec.execute(
            "build_available_list(\"%s\")" % (available_list,)
        )
        clusterdesc = self.config.get('cluster', 'clusterdesc')
        tasks = []

        with clusterlogger(self.logger) as (loghost, logport):
            for ms_name in ms_names:
                task = LOFARTask(
                    "result = make_columns(ms_name)",
                    push=dict(
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
            self.logger.info("Waiting for all colmaker tasks to complete")
            tc.barrier(tasks)
        for task in tasks:
            res = tc.get_task_result(task)
            if res.failure:
                print res.failure

        mec.execute("clear_available_list(\"%s\")" % (available_list,))

if __name__ == '__main__':
    sys.exit(colmaker().main())
