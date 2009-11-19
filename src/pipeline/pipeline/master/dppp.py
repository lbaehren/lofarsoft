import sys, os

from cuisine.WSRTrecipe import WSRTrecipe
from cuisine.ingredient import WSRTingredient

from utilities import ClusterError, get_parset, check_for_path

from IPython.kernel import client as IPclient

def run_dppp(ms_name, ms_outname, parset, log_location):
    # Run on engine to process data with DPPP
    from pipeline.nodes.dppp import run_dppp
    return run_dppp(ms_name, ms_outname, parset, log_location)

def build_available_list():
    # Run on engine to construct list of locally-stored data
    from twisted.python import log
    log.msg('building available list')
    import os
    from IPython.kernel.engineservice import get_engine
    engineapi = get_engine(id)
    engineapi.properties['available'] = [ms_name for ms_name in ms_names if os.access(ms_name, os.R_OK)]

class TestPipeline(WSRTrecipe):
    def __init__(self):
        super(TestPipeline, self).__init__()
        self.optionparser.add_option(
            '-j', '--job-name', 
            dest="job_name",
            help="Job Name"
        )
        self.optionparser.add_option(
            '-r', '--runtime-directory', 
            dest="runtime_directory",
            help="Runtime Directory"
        )
        self.optionparser.add_option(
            '-g', '--g(v)ds-file',
            dest="gvds",
            help="G(V)DS file describing data to be processed"
        )

    def go(self):
        try:
            gvds = get_parset(self.inputs['gvds'])
        except:
            self.logger.error("Unable to read G(V)DS file")
            raise

        job_directory = "%s/jobs/%s" % (
            self.inputs['runtime_directory'],
            self.inputs['job_name']
        )
        try:
            parset = "%s/dppp.parset" % (job_directory,)
            if not os.access(parset, os.R_OK):
                raise IOError
        except IOError:
            self.logger.error("DPPP parset not found")
            raise
            
        try:
            tc  = IPclient.TaskClient(self.inputs['runtime_directory'] + '/task.furl')
            mec = IPclient.MultiEngineClient(self.inputs['runtime_directory'] + '/multiengine.furl')
            mec.push_function(dict(run_dppp=run_dppp, build_available_list=build_available_list))
        except:
            self.logger.error("Unable to initialise cluster")
            raise ClusterError

        # We read the GVDS file to find the names of all the data files we're
        # going to process, then push this list out to the engines so they can
        # let us know which we have available
        ms_names = [
            gvds["Part%d.FileName" % (part_no,)] 
            for part_no in xrange(int(gvds["NParts"]))
        ]

        mec.push(dict(ms_names=ms_names))
        mec.execute("build_available_list()")

        tasks = []
        for ms_name in ms_names:
            ms_outname = ms_name + ".dppp"
            log_location = "%s/logs/%s/dppp.log" % (
                job_directory,
                os.path.basename(ms_name)
            )
            task = IPclient.StringTask(
                "result = run_dppp(ms_name, ms_outname, parset, log_location)",
                push=dict(
                    ms_name=ms_name,
                    ms_outname=ms_outname,
                    parset=parset,
                    log_location=log_location
                ),
                pull="result",
                depend=check_for_path,
                dependargs=ms_name
            )
            tasks.append(tc.run(task))
        tc.barrier(tasks)
        for task in tasks:
            tc.get_task_result(task)

if __name__ == '__main__':
    sys.exit(TestPipeline().main())
