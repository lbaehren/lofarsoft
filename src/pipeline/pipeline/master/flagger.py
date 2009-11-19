import sys
from pyraprunner import pyraprunner

def remote_function(input, output):
    from pipeline.nodes.flagger import flag_data
    return flag_data(input, output)

class flagger(pyraprunner):
    @staticmethod
    def remote_function(input, output):
        from pipeline.nodes.flagger import flag_data
        return flag_data(input, output)

    def __init__(self):
        super(flagger, self).__init__()

if __name__ == '__main__':
    sys.exit(flagger().main())
