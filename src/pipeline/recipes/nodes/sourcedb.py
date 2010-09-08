from __future__ import with_statement
from subprocess import Popen, CalledProcessError, PIPE, STDOUT
import shutil
import os.path
import tempfile
import sys

from lofarpipe.support.lofarnode import LOFARnode
from lofarpipe.support.utilities import log_time
from lofarpipe.support.pipelinelogging import CatchLog4CPlus
from lofarpipe.support.utilities import catch_segfaults


class sourcedb(LOFARnode):
    def run(self, executable, infile, catalogue):
        with log_time(self.logger):
            if os.path.exists(infile):
                self.logger.info("Processing %s" % (infile))
            else:
                self.logger.error("%s does not exist" % (infile))
                return 1

            output = os.path.join(infile, "sky")

            # Remove any old sky database
            shutil.rmtree(output, ignore_errors=True)

            working_dir = tempfile.mkdtemp()
            try:
                cmd = [executable, "format=<", "in=%s" % (catalogue), "out=%s" % (output)]
                with CatchLog4CPlus(
                    working_dir,
                    self.logger.name + "." + os.path.basename(infile),
                    os.path.basename(executable)
                ) as logger:
                    catch_segfaults(cmd, working_dir, None, logger)
            except CalledProcessError, e:
                # For CalledProcessError isn't properly propagated by IPython
                # Temporary workaround...
                self.logger.error(str(e))
                return 1
            finally:
                shutil.rmtree(working_dir)

        return 0

if __name__ == "__main__":
    #   If invoked directly, parse command line arguments for logger information
    #                        and pass the rest to the run() method defined above
    # --------------------------------------------------------------------------
    loghost, logport = sys.argv[1:3]
    sys.exit(sourcedb(loghost, logport).run_with_logging(*sys.argv[3:]))
