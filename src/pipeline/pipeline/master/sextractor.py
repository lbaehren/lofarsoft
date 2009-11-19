import sys, os

# Local helpers
from pipeline.support.lofarrecipe import LOFARrecipe
from pipeline.support.lofaringredient import LOFARinput, LOFARoutput
from pipeline.support.ipython import LOFARTask
import pipeline.support.utilities as utilities

def sextract(image):
    # Run on engine to source extract
    from pipeline.nodes.sextractor import sextract
    return sextract(image)

class sextractor(LOFARrecipe):
    def __init__(self):
        super(sextractor, self).__init__()
        self.optionparser.add_option(
            '-w', '--working-directory',
            dest="working_directory",
            help="Working directory used on compute nodes"
        )

    def go(self):
        self.logger.info("Starting source extraction run")
        super(sextractor, self).go()

        tc, mec = self._get_cluster()
        mec.push_function(
            dict(
                sextract=sextract,
                build_available_list=utilities.build_available_list,
                clear_available_list=utilities.clear_available_list
            )
        )
        self.logger.info("Pushed functions to cluster")

        # We read the GVDS file to find the names of all the data files we're
        # going to process, then push this list out to the engines so they can
        # let us know which we have available
        image_names = [
            "%s/%s" % (self._input_or_default('working_directory'), image)
            for image in self.inputs['args']
        ]

        # Construct list of available files on engines
        self.logger.info("Building list of data available on engines")
        available_list = "%s%s" % (self.inputs['job_name'], "sextractor")
        mec.push(dict(filenames=image_names))
        mec.execute(
            "build_available_list(\"%s\")" % (available_list,)
        )

        tasks = []
        for image_name in image_names:
            task = LOFARTask(
                "result = sextract(image_name)",
                push=dict(
                    image_name=image_name,
                ),
                pull="result",
                depend=utilities.check_for_path,
                dependargs=(image_name, available_list)
            )
            self.logger.info("Scheduling processing of %s" % (image_name,))
            tasks.append(tc.run(task))
        self.logger.info("Waiting for all source extraction tasks to complete")
        tc.barrier(tasks)
        for task in tasks:
            ##### Print failing tasks?
            ##### Abort if all tasks failed?
            res = tc.get_task_result(task)
            if res.failure:
                print res.failure

        # Save space on engines by clearing out old file lists
        mec.execute("clear_available_list(\"%s\")" % (available_list,))

        self.logger.info("Source extraction done")

if __name__ == '__main__':
    sys.exit(sextractor().main())
