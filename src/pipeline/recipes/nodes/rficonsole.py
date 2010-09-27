#                                                         LOFAR IMAGING PIPELINE
#
#                                                    rficonsole (AOflagger) node
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from __future__ import with_statement
from subprocess import CalledProcessError
import sys
import os.path
import shutil
import tempfile

from lofarpipe.support.pipelinelogging import log_time
from lofarpipe.support.utilities import catch_segfaults
from lofarpipe.support.lofarnode import LOFARnode
from lofarpipe.support.lofarexceptions import ExecutableMissing
from lofarpipe.support.pipelinelogging import CatchLog4CPlus

class rficonsole(LOFARnode):
    def run(self, executable, nthreads, strategy, *infiles):
        with log_time(self.logger):
            self.logger.info("Processing %s" % " ".join(infiles))

            try:
                if not os.access(executable, os.X_OK):
                    raise ExecutableMissing(executable)

                working_dir = tempfile.mkdtemp()
                cmd = [executable, "-j", nthreads]
                if os.path.exists(strategy):
                    cmd.extend(["-strategy", strategy])
                cmd.extend(infiles)
                with CatchLog4CPlus(
                    working_dir,
                    self.logger.name,
                    os.path.basename(executable)
                ) as logger:
                    catch_segfaults(cmd, working_dir, None, logger)
            except ExecutableMissing, e:
                self.logger.error("%s not found" % (e.args[0]))
                return 1
            except CalledProcessError, e:
                self.logger.error(str(e))
                return 1
            except Exception, e:
                self.logger.exception(e)
                return 1
            finally:
                shutil.rmtree(working_dir)

            return 0

if __name__ == "__main__":
    #   If invoked directly, parse command line arguments for logger information
    #                        and pass the rest to the run() method defined above
    # --------------------------------------------------------------------------
    loghost, logport = sys.argv[1:3]
    sys.exit(rficonsole(loghost, logport).run_with_logging(*sys.argv[3:]))
