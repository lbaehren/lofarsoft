# Python standard library
from __future__ import with_statement
from contextlib import closing
from subprocess import check_call, CalledProcessError
import os.path

from pipeline.support.lofarnode import LOFARnode
from pipeline.support.utilities import patch_parset, create_directory, log_time
from pipeline.support.lofarexceptions import ExecutableMissing
import pipeline.support.utilities as utilities

class dppp_node(LOFARnode):
    def run(self, infile, outfile, parset, log_location, executable, initscript):
        with log_time(self.logger):
            self.logger.info("Processing %s" % (infile,))

            # We need to patch the parset with the correct input/output MS names.
            temp_parset_filename = patch_parset(
                parset, {
                    'msin': infile,
                    'msout': outfile,
                }
            )

            create_directory(os.path.dirname(log_location))
            create_directory(os.path.dirname(outfile))

            env = utilities.read_initscript(initscript)

            try:
                if not os.access(executable, os.X_OK):
                    raise ExecutableMissing(executable)
                with closing(open(log_location, 'w')) as log:
                    raise
                    # What is the '1' for? Required by DP3...
                    cmd = [executable, temp_parset_filename, '1']
                    self.logger.debug("Running: %s" % (' '.join(cmd),))
                    result = check_call(
                        cmd,
                        stdout=log,
                        env=env
                    )
                    result = 0
                    self.logger.debug(
                        "%s returned exit status %d" % (executable, result)
                    )
                return result
            except ExecutableMissing, e:
                self.logger.error("%s not found" % (e.args[0]))
                raise
            except CalledProcessError, e:
                # For CalledProcessError isn't properly propagated by IPython
                # Temporary workaround...
                self.logger.error(str(e))
                raise Exception
            finally:
                os.unlink(temp_parset_filename)
