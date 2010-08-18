#                                                         LOFAR IMAGING PIPELINE
#
#                                                         ASKAPsoft cimager node
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from __future__ import with_statement
from subprocess import Popen, CalledProcessError, PIPE, STDOUT
from tempfile import mkstemp, mkdtemp
import os
import sys
import shutil

from lofar.parameterset import parameterset

from lofarpipe.support.pipelinelogging import CatchLog4CXX
from lofarpipe.support.lofarnode import LOFARnode
from lofarpipe.support.utilities import log_time

class cimager(LOFARnode):
    #                 Handles running a single cimager process on a compute node
    # --------------------------------------------------------------------------
    def run(self, imager_exec, vds, parset, resultsdir):
        #       imager_exec:                          path to cimager executable
        #               vds:           VDS file describing the data to be imaged
        #            parset:                                imager configuration
        #        resultsdir:                         place resulting images here
        # ----------------------------------------------------------------------
        with log_time(self.logger):
            self.logger.info("Processing %s" % (vds,))
            #                                                        Run cimager
            # ------------------------------------------------------------------
            try:
                self.logger.debug("Running cimager")
                working_dir = mkdtemp()
                with CatchLog4CXX(
                    working_dir,
                    self.logger.name + "." + os.path.basename(vds)
                ):
                    cimager_process = Popen(
                        [imager_exec, "-inputs", parset],
                        stdout=PIPE, stderr=PIPE, cwd=working_dir
                    )
                    sout, serr = cimager_process.communicate()
                self.logger.debug("cimager stdout: %s" % (sout,))
                self.logger.debug("cimager stderr: %s" % (serr,))
                if cimager_process.returncode != 0:
                    raise CalledProcessError(
                        cimager_process.returncode, imager_exec
                    )

                #        Dump the resulting images in the pipeline results area.
                #    I'm not aware of a foolproof way to predict the image names
                #                that will be produced, so we read them from the
                #                      parset and add standard cimager prefixes.
                # --------------------------------------------------------------
                parset = parameterset(parset)
                image_names = parset.getStringVector("Cimager.Images.Names")
                prefixes = [
                    "image", "psf", "residual", "weights", "sensitivity"
                ]
                for image_name in image_names:
                    for prefix in prefixes:
                        filename = image_name.replace("image", prefix, 1)
                        shutil.move(
                            os.path.join(working_dir, filename),
                            os.path.join(resultsdir, filename)
                        )
            except CalledProcessError, e:
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
    sys.exit(cimager(loghost, logport).run_with_logging(*sys.argv[3:]))
