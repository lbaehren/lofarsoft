import sys
from pyraprunner import pyraprunner

def remote_function(input, output, start_seconds, end_seconds):
    from pipeline.nodes.trimmer import trim_ms
    return trim_ms(input, output, start_seconds, end_seconds)

class trimmer(pyraprunner):
    def __init__(self):
        super(trimmer, self).__init__()
        self.optionparser.add_option(
            '--start-seconds',
            dest="start_seconds",
            type="float",
            help="Seconds to trim from start of data"
        )
        self.optionparser.add_option(
            '--end-seconds',
            dest="end_seconds",
            type="float",
            help="Seconds to trim from end of data"
        )

    def _generate_arguments(self):
        return "%f, %f" %  (
            self.inputs['start_seconds'],
            self.inputs['end_seconds']
        )

if __name__ == '__main__':
    sys.exit(trimmer().main())
