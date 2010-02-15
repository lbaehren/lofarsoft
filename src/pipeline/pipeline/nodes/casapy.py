# Python standard library
from __future__ import with_statement
from contextlib import closing
from subprocess import check_call, CalledProcessError
from dateutil.parser import parse as parse_date
import os.path, tempfile, shutil

from pipeline.support.lofarnode import LOFARnode
from pipeline.support.utilities import patch_parset, create_directory, log_time
from pipeline.support.lofarexceptions import ExecutableMissing
import pipeline.support.utilities as utilities

class casapy_node(LOFARnode):
    def run(self, infile, parset, start_time, end_time, increment)
        # Time execution of this job
        with log_time(self.logger):
            self.logger.info("Processing %s" % (infile,))

            start_time = parse_date(start_time)
            end_time   = parse_date(end_time)

            self.logger.debug("Start time: %s, end time: %s" % (str(start_time), str(end_time)))
            increment = timedelta(0, increment)



            # Patch the parset with the correct input/output MS names.
            temp_parset_filename = patch_parset(
                parset, {
                    'msin': infile,
                    'msout': outfile,
                }
            )

            try:
                if not os.access(executable, os.X_OK):
                    raise ExecutableMissing(executable)

                # DPPP looks for a log_prop file in its working directory
                # We create a temporary directory with the right log_prop,
                # run DPPP there, then delete it again when we're done.
                working_dir = tempfile.mkdtemp()
                log_prop_filename = os.path.join(
                    working_dir, os.path.basename(executable) + ".log_prop"
                )
                with open(log_prop_filename, 'w') as log_prop_file:
                    log_prop_file.write(log_prop % {'filename': os.path.join(
                            log_location,
                            os.path.basename(executable) + '.' + os.path.basename(infile) + '.log'
                        )}
                    )
                    log_prop_file.write(log_format)

                # We use subprocess.check_call() to spawn DPPP and check 
                # its exit status.
                # What is the '1' for? Required by DP3...
                cmd = [executable, temp_parset_filename, '1']
                self.logger.debug("Running: %s" % (' '.join(cmd),))
                result = check_call(
                    cmd,
                    cwd=working_dir,
                    env=env,
                    close_fds=True
                )
                self.logger.debug(
                    "%s returned exit status %d" % (executable, result)
                )
            except ExecutableMissing, e:
                self.logger.error("%s not found" % (e.args[0]))
                raise
            except CalledProcessError, e:
                # For CalledProcessError isn't properly propagated by IPython
                # Temporary workaround...
                self.logger.error(str(e))
                raise Exception
            finally:
                # Clean up tempoerary files.
                os.unlink(temp_parset_filename)
                shutil.rmtree(working_dir)

            return result
