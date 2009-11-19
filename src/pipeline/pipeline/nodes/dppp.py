import sys

from cuisine.WSRTrecipe import WSRTrecipe
from cuisine.ingredient import WSRTingredient

from IPython.kernel import client as IPclient

class TestPipeline(WSRTrecipe):
    def __init__(self):
        super(TestPipeline, self).__init__()
        self.tc = IPclient.TaskClient('/data/users/swinbank/pipeline_runtime/task.furl')

    def go(self):
        result = self.tc.map(lambda x: x**2, range(3200))
        print result

if __name__ == '__main__':
    sys.exit(TestPipeline().main())
