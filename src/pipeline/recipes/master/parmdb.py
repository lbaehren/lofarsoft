from __future__ import with_statement
import os, tempfile, subprocess, shutil

# Local helpers
from lofarpipe.support.lofarrecipe import LOFARrecipe
from lofarpipe.support.ipython import LOFARTask
from lofarpipe.support.clusterlogger import clusterlogger
from lofarpipe.support.lofarnode import run_node
import lofarpipe.support.utilities as utilities

template = """
create tablename="%s"
adddef Gain:0:0:Ampl  values=1.0
adddef Gain:1:1:Ampl  values=1.0
adddef Gain:0:0:Real  values=1.0
adddef Gain:1:1:Real  values=1.0
adddef DirectionalGain:0:0:Ampl  values=1.0
adddef DirectionalGain:1:1:Ampl  values=1.0
adddef DirectionalGain:0:0:Real  values=1.0
adddef DirectionalGain:1:1:Real  values=1.0
adddef AntennaOrientation values=5.497787144
quit
"""

class parmdb(LOFARrecipe):
    def __init__(self):
        super(parmdb, self).__init__()
        self.optionparser.add_option(
            '--executable',
            help="Executable for parmdbm"
        )

    def go(self):
        self.logger.info("Starting parmdb run")
        super(parmdb, self).go()

        self.logger.info("Generating template parmdb")
        pdbdir = tempfile.mkdtemp(
            dir=self.config.get("layout", "parset_directory")
        )
        pdbfile = os.path.join(pdbdir, 'instrument')

        parmdbm_process = subprocess.Popen(
            [self.inputs['executable']],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )
        sout, serr = parmdbm_process.communicate(template % pdbfile)
        self.logger.info("parmdb stdout: %s" % (sout,))
        self.logger.info("parmdb stderr: %s" % (serr,))

        ms_names = self.inputs['args']

        # Connect to the IPython cluster and initialise it with
        # the funtions we need.
        tc, mec = self._get_cluster()
        mec.push_function(
            dict(
                run_parmdb=run_node,
                build_available_list=utilities.build_available_list,
                clear_available_list=utilities.clear_available_list
            )
        )
        self.logger.debug("Pushed functions to cluster")

        # Use build_available_list() to determine which SBs are available
        # on each engine; we use this for dependency resolution later.
        self.logger.debug("Building list of data available on engines")
        available_list = "%s%s" % (
            self.inputs['job_name'], self.__class__.__name__
        )
        mec.push(dict(filenames=ms_names))
        mec.execute(
            "build_available_list(\"%s\")" % (available_list,)
        )

        with clusterlogger(self.logger) as (loghost, logport):
            with utilities.log_time(self.logger):
                self.logger.debug("Logging to %s:%d" % (loghost, logport))
                tasks = []
                for ms_name in ms_names:
                    task = LOFARTask(
                        "result = run_parmdb(ms_name, pdbfile)",
                        push=dict(
                            recipename=self.name,
                            nodepath=os.path.dirname(self.__file__.replace('master', 'nodes')),
                            ms_name=ms_name,
                            pdbfile=pdbfile,
                            loghost=loghost,
                            logport=logport
                        ),
                        pull="result",
                        depend=utilities.check_for_path,
                        dependargs=(ms_name, available_list)
                    )
                    self.logger.info("Scheduling processing of %s" % (ms_name,))
                    tasks.append((tc.run(task), ms_name))

                self.logger.debug("Waiting for all parmdb tasks to complete")
                tc.barrier([task for task, subband in tasks])

        failure = False
        for task, subband in tasks:
            res = tc.get_task_result(task)
            if res.failure:
                self.logger.warn("Task %s failed (processing %s)" % (task, subband))
                self.logger.warn(res)
                self.logger.warn(res.failure.getTraceback())
                failure = True

        self.logger.debug("Removing template parmdb")
        shutil.rmtree(pdbdir, ignore_errors=True)

        if failure:
            return 1
        else:
            self.outputs['data'] = self.inputs['args']
            return 0

if __name__ == '__main__':
    sys.exit(parmdb().main())
