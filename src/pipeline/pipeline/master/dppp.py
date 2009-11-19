import sys

from cuisine.WSRTrecipe import WSRTrecipe
from cuisine.ingredient import WSRTingredient

from utilities import ClusterError, get_parset, check_for_path

from IPython.kernel import client as IPclient

class TestPipeline(WSRTrecipe):
    def __init__(self):
        super(TestPipeline, self).__init__()
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
            tc = IPclient.TaskClient(self.inputs['runtime_directory'] + '/task.furl')
        except:
            self.logger.error("Unable to connect to cluster")
            raise ClusterError

        ms_names = [
            gvds["Part%d.FileName" % (part_no,)] 
            for part_no in xrange(int(gvds["NParts"]))
        ]

        tasks = []
        for ms_name in ms_names:
            task = IPclient.StringTask(
                "result = ms_name",
                push=dict(ms_name=ms_name),
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
