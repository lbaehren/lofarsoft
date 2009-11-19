# Python standard library
from __future__ import with_statement
from contextlib import closing
from subprocess import Popen, CalledProcessError, PIPE, STDOUT
from ConfigParser import SafeConfigParser as ConfigParser
import os.path

from pipeline.support.lofarexceptions import ExecutableMissing
from pipeline.support.utilities import create_directory

# Root directory for config file
from pipeline.support.lofarnode import LOFARnode

class makevds_node(LOFARnode):
    def run(self, infile, clusterdesc, outfile, executable):
        # Make VDS file for input MS in specifed location
        # The dev version of makevds appears to make more comprehensive VDS files
        # (including the FileName field) than the stable version.
        self.logger.info("Processing: %s" % (infile))
        try:
            if not os.access(executable, os.X_OK):
                raise ExecutableMissing(executable)
            cmd = [executable, clusterdesc, infile, outfile]
            self.logger.debug("Running: %s" % (' '.join(cmd,)))
            makevds_process = Popen(cmd, stdout=PIPE, stderr=STDOUT)
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
