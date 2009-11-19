from cuisine.WSRTrecipe import WSRTrecipe
from cuisine.ingredient import WSRTingredient

from IPython.kernel import client as IPclient

class TestPipeline(WSRTrecipe):
    def __init__(self):
        super(TestPipeline, self).__init__()

    def go(self):
        print "hello"

if __name__ == '__main__':
    sys.exit(TestPipeline().main())
