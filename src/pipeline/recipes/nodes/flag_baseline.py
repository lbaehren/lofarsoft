#                                                         LOFAR IMAGING PIPELINE
#
#                                                             flag_baseline node
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------
from __future__ import with_statement
import os.path
import sys

from pyrap.tables import taql

from lofarpipe.support.lofarnode import LOFARnode
from lofarpipe.support.utilities import log_time, string_to_list


class flag_baseline(LOFARnode):
    """
    Completely flag a series of baselines in a MeasurementSet.
    """
    def run(self, infile, *baselines):
        """
        baselines expects to receive a python array formatted as for the
        command line. So, for example:

        ("[1&1,", "1&2,", "1&3,"...)

        This is ugly, but makes it simple to call programmatically.
        """
        with log_time(self.logger):
            if os.path.exists(infile):
                self.logger.info("Processing %s" % (infile))
            else:
                self.logger.error("%s does not exist" % (infile))
                return 1

        antennae1, antennae2 = [], []
        baselines = string_to_list(" ".join(baselines))
        for baseline in baselines:
            ant1, ant2 = baseline.split("&")
            antennae1.append(int(ant1))
            antennae2.append(int(ant2))

        cmd = "UPDATE %s SET FLAG=True WHERE any(ANTENNA1=%s and ANTENNA2=%s)" % \
            (infile, str(antennae1), str(antennae2))
        self.logger.info("Running TaQL: " + cmd)

        try:
            taql(cmd)
        except Exception, e:
            self.logger.warn(str(e))
            return 1

        return 0

if __name__ == "__main__":
    #   If invoked directly, parse command line arguments for logger information
    #                        and pass the rest to the run() method defined above
    # --------------------------------------------------------------------------
    loghost, logport = sys.argv[1:3]
    sys.exit(flag_baseline(loghost, logport).run_with_logging(*sys.argv[3:]))
