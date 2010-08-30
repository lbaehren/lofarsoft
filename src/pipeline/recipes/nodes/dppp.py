#                                                         LOFAR IMAGING PIPELINE
#
#                                        DPPP (Data Pre-Procesing Pipeline) node
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from __future__ import with_statement
from subprocess import Popen, CalledProcessError, PIPE, STDOUT
from logging import getLogger
import sys
import os.path
import tempfile
import shutil

from lofarpipe.support.pipelinelogging import CatchLog4CPlus
from lofarpipe.support.pipelinelogging import log_time
from lofarpipe.support.utilities import patch_parset
from lofarpipe.support.utilities import read_initscript
from lofarpipe.support.utilities import create_directory
from lofarpipe.support.lofarnode import LOFARnode
from lofarpipe.support.lofarexceptions import ExecutableMissing

class dppp(LOFARnode):
    def run(
        self, infile, outfile, parset, executable, initscript,
        start_time, end_time, nthreads
    ):
        # Time execution of this job
        with log_time(self.logger):
            self.logger.info("Processing %s" % (infile,))
            self.logger.debug("Time interval: %s %s" % (start_time, end_time))

            #                                             Initialise environment
            #                 Limit number of threads used, per request from GvD
            # ------------------------------------------------------------------
            env = read_initscript(initscript)
            if nthreads == "None": nthreads = 1
            self.logger.debug("Using %s threads for NDPPP" % nthreads)
            env['OMP_NUM_THREADS'] = nthreads

            #    If the input and output filenames are the same, DPPP should not
            #       write a new MS, but rather update the existing one in-place.
            #              This is achieved by setting msout to an empty string.
            # ------------------------------------------------------------------
            if outfile == infile:
                outfile = "\"\""
            else:
                create_directory(os.path.dirname(outfile))

            #       Patch the parset with the correct input/output MS names and,
            #                                   if available, start & end times.
            # ------------------------------------------------------------------
            patch_dictionary = {
                'msin': infile,
                'msout': outfile,
            }
            if start_time and start_time != "None":
                patch_dictionary['msin.starttime'] = start_time
            if end_time and end_time != "None":
                patch_dictionary['msin.endtime'] = end_time
            try:
                temp_parset_filename = patch_parset(parset, patch_dictionary)
            except Exception, e:
                self.logger.error(e)

            try:
                if not os.access(executable, os.X_OK):
                    raise ExecutableMissing(executable)

                working_dir = tempfile.mkdtemp()
                cmd = [executable, temp_parset_filename, '1']
                with CatchLog4CPlus(
                    working_dir,
                    self.logger.name + "." + os.path.basename(infile),
                    os.path.basename(executable),
                ):
                    #     Catch NDPPP segfaults (a regular occurance), and retry
                    # ----------------------------------------------------------
                    tries = 0
                    while tries < 2:
                        if tries > 0:
                            self.logger.debug("Retrying...")
                        self.logger.debug("Running: %s" % (' '.join(cmd),))
                        ndppp_process = Popen(
                            cmd,
                            cwd=working_dir,
                            env=env,
                            stdout=PIPE,
                            stderr=STDOUT
                        )
                        sout, serr = ndppp_process.communicate()
                        logger = getLogger(self.logger.name + "." + os.path.basename(infile))
                        for line in sout.split('\n'):
                            logger.debug(line.strip())
                        self.logger.debug("NDPPP stderr: %s" % (serr,))
                        if ndppp_process.returncode == 0:
                            break
                        elif ndppp_process.returncode == -11:
                            self.logger.warn("NDPPP process segfaulted!")
                            tries += 1
                            continue
                        else:
                            raise CalledProcessError(
                                ndppp_process.returncode, executable
                            )
            except ExecutableMissing, e:
                self.logger.error("%s not found" % (e.args[0]))
                raise
            except CalledProcessError, e:
                #        CalledProcessError isn't properly propagated by IPython
                # --------------------------------------------------------------
                self.logger.error(str(e))
                raise Exception
            finally:
                os.unlink(temp_parset_filename)
                shutil.rmtree(working_dir)

            return 0

if __name__ == "__main__":
    #   If invoked directly, parse command line arguments for logger information
    #                        and pass the rest to the run() method defined above
    # --------------------------------------------------------------------------
    loghost, logport = sys.argv[1:3]
    sys.exit(dppp(loghost, logport).run_with_logging(*sys.argv[3:]))
