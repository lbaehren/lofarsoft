#                                                         LOFAR IMAGING PIPELINE
#
#                                                                  vdsmaker node
#                                                         John Swinbank, 2009-10
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from __future__ import with_statement
from subprocess import Popen, CalledProcessError, PIPE, STDOUT
import os
import sys

from lofarpipe.support.lofarexceptions import ExecutableMissing
from lofarpipe.support.utilities import create_directory, log_time
from lofarpipe.support.pipelinelogging import log_process_output
from lofarpipe.support.lofarnode import LOFARnode

class vdsmaker(LOFARnode):
    """
    Make a VDS file for the input MS in a specificed location.
    """
    def run(self, infile, clusterdesc, outfile, executable):
        with log_time(self.logger):
            if os.path.exists(infile):
                self.logger.info("Processing %s" % (infile))
            else:
                self.logger.error("%s does not exist" % (infile))
                return 1

            try:
                if not os.access(executable, os.X_OK):
                    raise ExecutableMissing(executable)
                cmd = [executable, clusterdesc, infile, outfile]
                #   Catch makevds segfaults (a regular occurance), and retry
                # ----------------------------------------------------------
                tries = 0
                while tries < 2:
                    if tries > 0:
                        self.logger.debug("Retrying...")
                    self.logger.debug("Running: %s" % (' '.join(cmd),))
                    makevds_process = Popen(
                        cmd, stdout=PIPE, stderr=PIPE, close_fds=True
                    )
                    sout, serr = makevds_process.communicate()
                    result = makevds_process.returncode
                    log_process_output("makevds", sout, serr, self.logger)
                    if result == 0:
                        break
                    elif result == -11:
                        self.logger.warn("makevds process segfaulted!")
                        tries += 1
                        continue
                    else:
                        raise CalledProcessError(result, executable)
                return result
            except ExecutableMissing, e:
                self.logger.error("%s not found" % (e.args[0]))
                raise
            except CalledProcessError, e:
                # For CalledProcessError isn't properly propagated by IPython
                # Temporary workaround...
                self.logger.error(str(e))
                raise Exception

if __name__ == "__main__":
    #   If invoked directly, parse command line arguments for logger information
    #                        and pass the rest to the run() method defined above
    # --------------------------------------------------------------------------
    loghost, logport = sys.argv[1:3]
    sys.exit(vdsmaker(loghost, logport).run_with_logging(*sys.argv[3:]))
