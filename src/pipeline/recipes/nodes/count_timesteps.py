#                                                         LOFAR IMAGING PIPELINE
#
#                                                           count_timesteps node
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------
from __future__ import with_statement
import os.path
import sys

from pyrap.tables import taql

from lofarpipe.support.lofarnode import LOFARnodeTCP
from lofarpipe.support.utilities import log_time


class count_timesteps(LOFARnodeTCP):
    """
    Return the first and last values in the TIME column.
    """
    def run(self, infile):
        with log_time(self.logger):
            if os.path.exists(infile):
                self.logger.info("Processing %s" % (infile))
            else:
                self.logger.error("Dataset %s does not exist" % (infile))
                return 1

            try:
                self.outputs['start_time'] = taql(
                    "CALC MIN([SELECT TIME from %s])" % infile
                )[0]
                self.outputs['end_time'] = taql(
                    "CALC MAX([SELECT TIME from %s])" % infile
                )[0]
            except Exception, e:
                self.logger.error(str(e))
                return 1

        return 0

if __name__ == "__main__":
    #   If invoked directly, parse command line arguments for logger information
    #                        and pass the rest to the run() method defined above
    # --------------------------------------------------------------------------
    jobid, jobhost, jobport = sys.argv[1:4]
    sys.exit(count_timesteps(jobid, jobhost, jobport).run_with_stored_arguments())
