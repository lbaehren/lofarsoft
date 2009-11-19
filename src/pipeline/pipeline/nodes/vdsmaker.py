# Python standard library
from __future__ import with_statement
from contextlib import closing
from subprocess import check_call
from ConfigParser import SafeConfigParser as ConfigParser
import os.path

from pipeline.support.lofarexceptions import ExecutableMissing
from pipeline.support.utilities import create_directory

# Root directory for config file
from pipeline import __path__ as config_path

from pipeline.support.lofarnode import LOFARnode

class makevds_node(LOFARnode):
    def run(self, infile, clusterdesc, outfile, log_location):
        # Make VDS file for input MS in specifed location
        # The dev version of makevds appears to make more comprehensive VDS files
        # (including the FileName field) than the stable version.
        config = ConfigParser()
        config.read(os.path.join(config_path[0], 'pipeline.cfg'))
        executable = config.get('vds', 'makevds')

        create_directory(os.path.dirname(log_location))

        try:
            if not os.access(executable, os.X_OK):
                raise ExecutableMissing(executable)
            with closing(open(log_location, 'w')) as log:
                cmd = [executable, clusterdesc, infile, outfile]
                self.logger.debug("Running: %s" % (' '.join(cmd),))
                result = check_call(cmd, stdout=log)
            return result
        except ExecutableMissing, e:
            with closing(open(log_location, 'a')) as log:
                self.logger.error("%s not found" % (e.args[0]))
            raise
        except CalledProcessError, e:
            # For CalledProcessError isn't properly propagated by IPython
            # Temporary workaround...
            self.logger.error(str(e))
            raise Exception
