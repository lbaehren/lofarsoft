# Python standard library
from __future__ import with_statement
from contextlib import closing
from subprocess import Popen, CalledProcessError, PIPE, STDOUT
import os.path, tempfile, shutil

from lofarpipe.support.lofarnode import LOFARnode
from lofarpipe.support.utilities import patch_parset, create_directory, log_time, read_initscript
from lofarpipe.support.lofarexceptions import ExecutableMissing

log_prop = """
log4cplus.rootLogger=DEBUG, FILE
log4cplus.logger.TRC=DEBUG, FILE
log4cplus.additivity.TRC=FALSE

log4cplus.appender.FILE=log4cplus::FileAppender
log4cplus.appender.FILE.File=%(filename)s
log4cplus.appender.FILE.layout=log4cplus::PatternLayout
"""
log_format = "log4cplus.appender.FILE.layout.ConversionPattern=%x %D{%d-%m-%y %H:%M:%S} %-5p %c{3} - %m [%.25l]%n"

class dppp(LOFARnode):
    def run(self, infile, outfile, parset, log_location, executable, initscript, start_time, end_time):
        # Time execution of this job
        with log_time(self.logger):
            self.logger.info("Processing %s" % (infile,))
            self.logger.debug("Time interval: %s %s" % (start_time, end_time))

            # Create output directories and ensure the environment
            # is initialised for DPPP run
            create_directory(log_location)
            env = read_initscript(initscript)
            # Limit number of threads, per request from GvD
            env['OMP_NUM_THREADS'] = "1"

            # If the input and output filenames are the same, DPPP should not
            # write a new MS, but rather update the existing one in-place.
            # This is achieved by setting msout to an empty string.
            if outfile == infile:
                outfile = "\"\""
            else:
                create_directory(os.path.dirname(outfile))

            # Patch the parset with the correct input/output MS names and, if
            # available, start & end times.
            patch_dictionary = {
                'msin': infile,
                'msout': outfile,
            }
            if start_time:
                patch_dictionary['msin.starttime'] = start_time
            if end_time:
                patch_dictionary['msin.endtime'] = end_time
            temp_parset_filename = patch_parset(parset, patch_dictionary)

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
                tries = 0
                while tries < 2:
                    # If ndppp segfaults, we get a return value of -11.
                    # The segfaults seem to be transitory, so try re-running
                    # if we get one and see if it works on the second attempt
                    if tries > 0:
                        self.logger.debug("Retrying...")
                    self.logger.debug("Running: %s" % (' '.join(cmd),))
                    ndppp_process = Popen(cmd, cwd=working_dir, env=env, close_fds=True, stdout=PIPE, stderr=STDOUT)
                    sout, serr = ndppp_process.communicate()
                    self.logger.debug("NDPPP stdout: %s" % (sout,))
                    self.logger.debug("NDPPP stderr: %s" % (serr,))
                    if ndppp_process.returncode == 0:
                        break
                    elif ndppp_process.returncode == -11:
                        self.logger.warn("NDPPP process segfaulted!")
                        tries += 1
                        continue
                    else:
                        raise CalledProcessError(ndppp_process.returncode, executable)
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

            return 0
