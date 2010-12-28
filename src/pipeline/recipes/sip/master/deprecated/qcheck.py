from __future__ import with_statement
import sys, os

from lofarpipe.support.ipython import LOFARTask
from lofarpipe.support.lofarrecipe import LOFARrecipe
import lofarpipe.support.utilities as utilities
from lofarpipe.support.clusterlogger import clusterlogger

def run_qcheck(infile, pluginlist, outputdir):
    from lofarrecipe.nodes.qcheck import qcheck_node
    return qcheck_node(loghost=loghost, logport=logport).run(
        infile,
        pluginlist,
        outputdir
    )

class qcheck(LOFARrecipe):
    def __init__(self):
        super(qcheck, self).__init__()
        self.optionparser.add_option(
            '-w', '--working-directory',
            dest="working_directory",
            help="Working directory used on compute nodes"
        )
        self.optionparser.add_option(
            '--plugins',
            dest="plugins",
            help="[Expert use] Quality check plugins"
        )

    def go(self):
        super(qcheck, self).go()
        self.logger.info("Quality check system starting")

        self.outputs['data'] = [
            os.path.join(
                self.inputs['working_directory'], self.inputs['job_name'], filename
            )
            for filename in self.inputs['args']
        ]
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
        mec.push(dict(filenames=self.outputs['data']))
        mec.execute(
            "build_available_list(\"%s\")" % (available_list,)
        )
        clusterdesc = self.config.get('cluster', 'clusterdesc')

        with clusterlogger(self.logger) as (loghost, logport):
            self.logger.debug("Logging to %s:%d" % (loghost, logport))
            tasks = []
            for image_name in self.outputs['data']:
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
