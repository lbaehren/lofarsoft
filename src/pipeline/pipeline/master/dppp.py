import sys, os

from cuisine.WSRTrecipe import WSRTrecipe
from cuisine.ingredient import WSRTingredient

from utilities import ClusterError, get_parset, check_for_path

from IPython.kernel import client as IPclient

def run_dppp(ms_name, ms_outname, parset):
    from pipeline.nodes.dppp import run_dppp
    return run_dppp(ms_name, ms_outname, parset)

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

        try:
            parset = "%s/jobs/%s/dppp.parset" % (
                self.inputs['runtime_directory'], 
                self.inputs['job_name']
            )
            if not os.access(parset, os.R_OK):
                raise IOError
        except IOError:
            self.logger.error("DPPP parset not found")
            raise
            
        try:
            tc  = IPclient.TaskClient(self.inputs['runtime_directory'] + '/task.furl')
            mec = IPclient.MultiEngineClient(self.inputs['runtime_directory'] + '/multiengine.furl')
            mec.push_function(dict(run_dppp=run_dppp))
        except:
            self.logger.error("Unable to initialise cluster")
            raise ClusterError

        ms_names = [
            gvds["Part%d.FileName" % (part_no,)] 
            for part_no in xrange(int(gvds["NParts"]))
        ]

        parset = "%s/jobs/%s/dppp.parset" % (
            self.inputs['runtime_directory'], 
            self.inputs['job_name']
        )

        tasks = []
        for ms_name in ms_names:
            ms_outname = ms_name + ".dppp"
            task = IPclient.StringTask(
                "result = run_dppp(ms_name, ms_outname, parset)",
                push=dict(ms_name=ms_name, ms_outname=ms_outname, parset=parset),
                pull="result",
                depend=check_for_path,
                dependargs=ms_name
            )
            tasks.append(tc.run(task))
        tc.barrier(tasks)
        for task in tasks:
            print tc.get_task_result(task)

        print self.inputs

if __name__ == '__main__':
    sys.exit(TestPipeline().main())
