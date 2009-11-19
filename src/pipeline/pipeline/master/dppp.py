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
            '-g', '--g(v)ds-file',
            dest="gvds",
            help="G(V)DS file describing data to be processed"
        )
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

        try:
            gvds = utilities.get_parset(self.inputs['gvds'])
        except:
            self.logger.error("Unable to read G(V)DS file")
            raise

        job_directory = self.config.get("layout", "job_directory")

        tc, mec = self._get_cluster()
        mec.push_function(
            dict(
                run_dppp=run_dppp,
#                build_available_list=utilities.build_available_list,
#                clear_available_list=utilities.clear_available_list
            )
        )
        self.logger.info("Pushed functions to cluster")

        # We read the GVDS file to find the names of all the data files we're
        # going to process, then push this list out to the engines so they can
        # let us know which we have available
        ms_names = [
            gvds["Part%d.FileName" % (part_no,)] 
            for part_no in xrange(int(gvds["NParts"]))
        ]

        # Construct list of available files on engines
#        self.logger.info("Building list of data available on engines")
#        available_list = "%s%s" % (self.inputs['job_name'], "dppp")
#        mec.push(dict(filenames=ms_names))
#        mec.execute(
#            "build_available_list(\"%s\")" % (available_list,)
#        )

        tasks = []
        outnames = []
        for ms_name in ms_names:
#            outnames.append(ms_name + ".dppp")
#            outnames.append("/data/swinbank/L2009_13244/" + os.path.basename(ms_name) + ".dppp")
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
                    parset=self.inputs['parset'],
                    log_location=log_location
                ),
                pull="result",
#                depend=utilities.check_for_path,
#                dependargs=(ms_name, available_list)
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
                failure = True
        if failure:
            return 1

        # Save space on engines by clearing out old file lists
#        mec.execute("clear_available_list(\"%s\")" % (available_list,))

        # Now set up a colmaker recipe to insert missing columns in the
        # processed data

        self.logger.info("Calling colmaker")
        inputs = LOFARinput(self.inputs)
        inputs['args'] = outnames
        outputs = LOFARoutput()
        if self.cook_recipe('colmaker', inputs, outputs):
            self.logger.warn("colmaker reports failure")
            return 1

        # Now set up a vdsmaker recipe to build a GDS file describing the
        # processed data
        self.logger.info("Calling vdsmaker")
        inputs = LOFARinput(self.inputs)
        inputs['directory'] = self.config.get('layout', 'vds_directory')
        inputs['gds'] = self.config.get('dppp', 'gds_output')
        inputs['args'] = outnames
        outputs = LOFARoutput()
        if self.cook_recipe('vdsmaker', inputs, outputs):
            self.logger.warn("vdsmaker reports failure")
            return 1
        else:
            self.outputs['gds'] = outputs['gds']
            return 0


if __name__ == '__main__':
    sys.exit(dppp().main())
