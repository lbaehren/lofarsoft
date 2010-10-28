#                                                         LOFAR IMAGING PIPELINE
#
#                                                Example of a simple node script
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

import sys
from lofarpipe.support.lofarnode import LOFARnode

class example_parallel(LOFARnode):
    def run(self, *args):
        for arg in args:
            self.logger.info("Received %s as argument" % arg)
        return 0

if __name__ == "__main__":
    sys.exit(example_parallel(sys.argv[1], sys.argv[2]).run_with_logging(*sys.argv[3:]))


