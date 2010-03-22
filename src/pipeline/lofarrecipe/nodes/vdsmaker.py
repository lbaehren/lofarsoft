# Python standard library
from __future__ import with_statement
import os
from subprocess import Popen, CalledProcessError, PIPE, STDOUT

from pipeline.support.lofarexceptions import ExecutableMissing
from pipeline.support.utilities import create_directory, log_time
from pipeline.support.lofarnode import LOFARnode

class makevds_node(LOFARnode):
    """
    Make a VDS file for the input MS in a specificed location.
    """
    def run(self, infile, clusterdesc, outfile, executable):
        with log_time(self.logger):
            self.logger.info("Processing: %s" % (infile))
            try:
                if not os.access(executable, os.X_OK):
                    raise ExecutableMissing(executable)
                cmd = [executable, clusterdesc, infile, outfile]
                self.logger.debug("Running: %s" % (' '.join(cmd,)))
                makevds_process = Popen(
                    cmd, stdout=PIPE, stderr=STDOUT, close_fds=True
                )
                result = makevds_process.wait()
                if result != 0:
                    raise CalledProcessError(result, cmd)
                return result
            except ExecutableMissing, e:
                self.logger.error("%s not found" % (e.args[0]))
                raise
            except CalledProcessError, e:
                # For CalledProcessError isn't properly propagated by IPython
                # Temporary workaround...
                self.logger.error(str(e))
                raise Exception
