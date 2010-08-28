#                                                         LOFAR IMAGING PIPELINE
#
#                                                  BBS (BlackBoard Selfcal) node
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from __future__ import with_statement
from subprocess import Popen, CalledProcessError, PIPE, STDOUT
from tempfile import mkstemp, mkdtemp
import os
import sys
import shutil

from lofarpipe.support.pipelinelogging import CatchLog4CPlus
from lofarpipe.support.lofarnode import LOFARnode
from lofarpipe.support.utilities import read_initscript
from lofarpipe.support.utilities import get_mountpoint
from lofarpipe.support.utilities import log_time
from lofarpipe.support.pipelinelogging import log_process_output
from lofarpipe.support.parset import Parset

class bbs(LOFARnode):
    #                      Handles running a single BBS kernel on a compute node
    # --------------------------------------------------------------------------
    def run(
        self, executable, initscript, infile, key, db_name, db_user, db_host
    ):
        #                           executable: path to KernelControl executable
        #                           initscript:             path to lofarinit.sh
        #                               infile:    MeasurementSet for processing
        #       key, db_name, db_user, db_host:   database connection parameters
        # ----------------------------------------------------------------------
        with log_time(self.logger):
            self.logger.info("Processing %s" % (infile,))

            #        Build a configuration parset specifying database parameters
            #                                                     for the kernel
            # ------------------------------------------------------------------
            self.logger.debug("Setting up kernel parset")
            filesystem = "%s:%s" % (os.uname()[1], get_mountpoint(infile))
            fd, parset_filename = mkstemp()
            kernel_parset = Parset()
            for key, value in {
                "ObservationPart.Filesystem": filesystem,
                "ObservationPart.Path": infile,
                "BBDB.Key": key,
                "BBDB.Name": db_name,
                "BBDB.User": db_user,
                "BBDB.Host": db_host,
                "ParmDB.Sky": os.path.join(infile, "sky"),
                "ParmDB.Instrument": os.path.join(infile, "instrument")
            }.iteritems():
                kernel_parset.add(key, value)
            kernel_parset.writeFile(parset_filename)
            os.close(fd)
            self.logger.debug("Parset written to %s" % (parset_filename,))


            #                                                     Run the kernel
            #               Catch & log output from the kernel logger and stdout
            # ------------------------------------------------------------------
            working_dir = mkdtemp()
            env = read_initscript(initscript)
            try:
                cmd = [executable, parset_filename, "0"]
                self.logger.debug("Executing BBS kernel")
                with CatchLog4CPlus(
                    working_dir,
                    self.logger.name + "." + os.path.basename(infile),
                    os.path.basename(executable),
                ):
                    bbs_kernel_process = Popen(
                        cmd, stdout=PIPE, stderr=PIPE, cwd=working_dir
                    )
                    sout, serr = bbs_kernel_process.communicate()
                log_process_output("BBS kernel", sout, serr, self.logger)
                if bbs_kernel_process.returncode != 0:
                    raise CalledProcessError(
                        bbs_kernel_process.returncode, executable
                    )
            except CalledProcessError, e:
                self.logger.error(str(e))
                return 1
            finally:
                os.unlink(parset_filename)
                shutil.rmtree(working_dir)
            return 0

if __name__ == "__main__":
    #   If invoked directly, parse command line arguments for logger information
    #                        and pass the rest to the run() method defined above
    # --------------------------------------------------------------------------
    loghost, logport = sys.argv[1:3]
    sys.exit(bbs(loghost, logport).run_with_logging(*sys.argv[3:]))
