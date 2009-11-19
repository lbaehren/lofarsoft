import sys, os, tempfile

from pipeline.support.ipython import LOFARTask

# Local helpers
from pipeline.support.lofarrecipe import LOFARrecipe
import pipeline.support.utilities as utilities

def exclude_station(input, output, station):
    from pipeline.nodes.excluder import exclude_stations
    return exclude_station(input, output, station)

class excluder(LOFARrecipe):
    def __init__(self):
        super(excluder, self).__init__()
        self.optionparser.add_option(
            '--station',
            dest="station",
            help="Name of station to exclude (e.g. DE001LBA)"
        )
        self.optionparser.add_option(
            '--suffix',
            dest="suffix",
            help="Suffix to add to output data (default: overwrite existing)"
        )

    def go(self):
        super(excluder, self).go()

        ms_names = self.inputs['args']

        tc, mec = self._get_cluster()
        mec.push_function(
            dict(
                exclude_station=exclude_station,
                build_available_list=utilities.build_available_list,
                clear_available_list=utilities.clear_available_list
            )
        )
        self.logger.info("Pushed functions to cluster")

        self.logger.info("Building list of data available on engines")
        available_list = "%s%s" % (self.inputs['job_name'], "excluder")
        mec.push(dict(filenames=ms_names))
        mec.execute(
            "build_available_list(\"%s\")" % (available_list,)
        )
        tasks = []
        outnames = []
        for ms_name in ms_names:
            outnames.append(ms_name + self.inputs['suffix'])
            execute_string = "result = exclude_station(ms_name, \"%s\", %f, %f)" % (
                outnames[-1],
                self.inputs['station'],
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
        self.logger.info("Waiting for all excluder tasks to complete")
        tc.barrier(tasks)
        for task in tasks:
            res = tc.get_task_result(task)
            if res.failure:
                print res.failure

        self.outputs['data'] = outnames

        mec.execute("clear_available_list(\"%s\")" % (available_list,))

if __name__ == '__main__':
    sys.exit(excluder().main())
