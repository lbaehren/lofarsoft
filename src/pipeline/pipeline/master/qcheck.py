from __future__ import with_statement
import sys, os

from pipeline.support.ipython import LOFARTask
from pipeline.support.lofarrecipe import LOFARrecipe
import pipeline.support.utilities as utilities
from pipeline.support.clusterlogger import clusterlogger

def run_qcheck(infile, pluginlist, outputdir):
    from pipeline.nodes.qcheck import qcheck_node
    return qcheck_node(loghost=loghost, logport=logport).run(
        infile,
        pluginlist,
        outputdir
    )

class qcheck(LOFARrecipe):
    def __init__(self):
        super(qcheck, self).__init__()
        self.optionparser.add_option(
            '--plugins',
            dest="plugins",
            help="[Expert use] Quality check plugins"
        )

    def go(self):
        super(qcheck, self).go()
        self.logger.info("Quality check system starting")

        image_names = self.inputs['args']
        plugins = utilities.string_to_list(self.inputs['plugins'])
        self.logger.info("Using plugins: %s" % (str(plugins)))

        tc, mec = self._get_cluster()
        mec.push_function(
            dict(
                run_qcheck=run_qcheck,
                build_available_list=utilities.build_available_list,
                clear_available_list=utilities.clear_available_list
            )
        )
        self.logger.info("Pushed functions to cluster")

        self.logger.info("Building list of data available on engines")
        available_list = "%s%s" % (self.inputs['job_name'], "qcheck")
        mec.push(dict(filenames=image_names))
        mec.execute(
            "build_available_list(\"%s\")" % (available_list,)
        )
        clusterdesc = self.config.get('cluster', 'clusterdesc')


        with clusterlogger(self.logger) as (loghost, logport):
            self.logger.debug("Logging to %s:%d" % (loghost, logport))
            tasks = []
            for image_name in image_names:
                task = LOFARTask(
                    "result = run_qcheck(infile, pluginlist, outputdir)",
                    push=dict(
                        infile=image_name,
                        pluginlist=plugins,
                        outputdir=self.config.get('layout', 'results_directory'),
                        loghost=loghost,
                        logport=logport
                    ),
                    pull="result",
                    depend=utilities.check_for_path,
                    dependargs=(image_name, available_list)
                )
                self.logger.info("Scheduling processing of %s" % (image_name,))
                tasks.append(tc.run(task))
            self.logger.info("Waiting for all qcheck tasks to complete")
            tc.barrier(tasks)

            for task in tasks:
                tc.get_task_result(task)

            mec.execute("clear_available_list(\"%s\")" % (available_list,))
            self.logger.info("qcheck done")

if __name__ == '__main__':
    sys.exit(eval(os.path.splitext(os.path.basename(sys.argv[0]))[0])().main())
