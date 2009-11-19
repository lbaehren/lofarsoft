import sys, os

# Local helpers
from pipeline.support.lofarrecipe import LOFARrecipe
from pipeline.support.lofaringredient import LOFARinput, LOFARoutput
from pipeline.support.ipython import LOFARTask
import pipeline.support.utilities as utilities

def run_dppp(ms_name, ms_outname, parset, log_location):
    # Run on engine to process data with DPPP
    from pipeline.nodes.dppp import run_dppp
    return run_dppp(ms_name, ms_outname, parset, log_location)

class dppp(LOFARrecipe):
    def __init__(self):
        super(dppp, self).__init__()
        self.optionparser.add_option(
            '-p', '--parset',
            dest="parset",
            help="Parset containing configuration for DPPP"
        )
        self.optionparser.add_option(
            '-w', '--working-directory',
            dest="working_directory",
            help="Working directory used on compute nodes"
        )

    def go(self):
        self.logger.info("Starting DPPP run")
        super(dppp, self).go()

        job_directory = self.config.get("layout", "job_directory")
        ms_names = self.inputs['args']

        tc, mec = self._get_cluster()
        mec.push_function(
            dict(
                run_dppp=run_dppp,
                "build_available_list": utilities.build_available_list,
                "clear_available_list": utilities.clear_available_list
            )
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

        tasks = []
        outnames = []
        for ms_name in ms_names:
            outnames.append(
                os.path.join(
                    self._input_or_default('working_directory'),
                    self.inputs['job_name'],
                    os.path.basename(ms_name) + ".dppp"
                )
            )

            log_location = "%s/%s/%s" % (
                self.config.get('layout', 'log_directory'),
                os.path.basename(ms_name),
                self.config.get('dppp', 'log')
            )
            task = LOFARTask(
                "result = run_dppp(ms_name, ms_outname, parset, log_location)",
                push=dict(
                    ms_name=ms_name,
                    ms_outname=outnames[-1],
                    parset=self._input_or_default('parset'),
                    log_location=log_location
                ),
                pull="result"
                depend=utilities.check_for_path,
                dependargs=(ms_name, available_list)
            )
            self.logger.info("Scheduling processing of %s" % (ms_name,))
            if not self.inputs['dry_run']:
                tasks.append(tc.run(task))
        self.logger.info("Waiting for all DPPP tasks to complete")
        tc.barrier(tasks)
        failure = False
        for task in tasks:
            ##### Print failing tasks?
            ##### Abort if all tasks failed?
            res = tc.get_task_result(task)
            if res.failure:
                self.logger.warn("Task %s failed" % (task))
                self.logger.warn(res)
                failure = True
        if failure:
            return 1
        self.outputs['ms_names'] = outnames
        return 0

if __name__ == '__main__':
    sys.exit(dppp().main())
