#                                                         LOFAR IMAGING PIPELINE
#
#                                                Example of a simple node script
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

import sys
from lofarpipe.support.lofarnode import LOFARnode

class example_parallel(LOFARnodeTCP):
    def run(self, *args):
        for arg in args:
            self.logger.info("Received %s as argument" % str(arg))
        return 0

if __name__ == "__main__":
    jobid, jobhost, jobport = sys.argv[1:4]
    sys.exit(example_parallel(jobid, jobhost, jobport).run_with_stored_arguments())

