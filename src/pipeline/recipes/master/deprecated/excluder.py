import sys
from lofarpipe.support.pyraprunner import pyraprunner
from lofarpipe.support.utilities import string_to_list

class excluder(pyraprunner):
    def __init__(self):
        super(excluder, self).__init__()
        self.optionparser.add_option(
            '--station',
            dest="station",
            help="Name of stations to exclude (e.g. DE001LBA)"
        )

    def _generate_arguments(self):
        return "\"%s\"" % ('\", \"'.join(string_to_list(self.inputs['station'])))

if __name__ == '__main__':
    sys.exit(excluder().main())
