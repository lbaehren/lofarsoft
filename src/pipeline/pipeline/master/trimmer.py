import sys, os, tempfile

from pipeline.support.ipython import LOFARTask

# Local helpers
from pipeline.support.lofarrecipe import LOFARrecipe
import pipeline.support.utilities as utilities

def trim_ms(input, output, start_seconds, end_seconds):
    from pipeline.nodes.trimmer import trim_ms
    return trim_ms(input, output, start_seconds, end_seconds)

class trimmer(LOFARrecipe):
    def __init__(self):
        super(trimmer, self).__init__()
        self.optionparser.add_option(
            '--start-seconds',
            dest="start_seconds",
            type="float",
            help="Seconds to trim from start of data"
        )
        self.optionparser.add_option(
            '--end-seconds',
            dest="end_seconds",
            type="float",
            help="Seconds to trim from end of data"
        )
        self.optionparser.add_option(
            '--suffix',
            dest="suffix",
            help="Suffix to add to trimmed data (default: overwrite existing)"
        )

    def go(self):
        super(trimmer, self).go()

        ms_names = self.inputs['args']

        tc, mec = self._get_cluster()
        mec.push_function(
            dict(
                trim_ms=trim_ms,
                build_available_list=utilities.build_available_list,
                clear_available_list=utilities.clear_available_list
            )
        )
        self.logger.info("Pushed functions to cluster")

        # Build VDS files for each of the newly created MeasurementSets
        self.logger.info("Building list of data available on engines")
        available_list = "%s%s" % (self.inputs['job_name'], "trimmer")
        mec.push(dict(filenames=ms_names))
        mec.execute(
            "build_available_list(\"%s\")" % (available_list,)
        )
        clusterdesc = self.config.get('cluster', 'clusterdesc')
        tasks = []
        outnames = []
        for ms_name in ms_names:
            outnames.append(ms_name + self.inputs['suffix'])
            execute_string = "result = trim_ms(ms_name, \"%s\", %f, %f)" % (
                outnames[-1],
                self.inputs['start_seconds'],
                self.inputs['end_seconds']
            )
            print "Executing: ", execute_string
            task = LOFARTask(
                execute_string,
                push=dict(
                    ms_name=ms_name,
                ),
                pull="result",
                depend=utilities.check_for_path,
                dependargs=(ms_name, available_list)
            )
            self.logger.info("Scheduling processing of %s" % (ms_name,))
            tasks.append(tc.run(task))
        self.logger.info("Waiting for all trimmer tasks to complete")
        tc.barrier(tasks)
        for task in tasks:
            res = tc.get_task_result(task)
            if res.failure:
                print res.failure

        self.outputs['data'] = outnames

        mec.execute("clear_available_list(\"%s\")" % (available_list,))

if __name__ == '__main__':
    sys.exit(trimmer().main())
