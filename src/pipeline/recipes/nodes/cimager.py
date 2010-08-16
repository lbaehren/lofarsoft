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
from lofarpipe.support.utilities import patch_parset

class cimager(LOFARnode):
    def run(self, imager_exec, convert_exec, vds, parset, resultsdir):
        #       imager_exec:                          path to cimager executable
        #      convert_exec:              path to convertimagerparset executable
        #               vds: single subband VDS file describing data for imaging
        #            parset:                       template imager configuration
        #        resultsdir:                         place resulting images here
        # ----------------------------------------------------------------------
        with log_time(self.logger):
            self.logger.info("Processing %s" % (vds,))

            #        Patch information required for imaging into template parset
            # ------------------------------------------------------------------
            self.logger.debug("Creating parset")
            vds_data = parameterset(vds)
            frequency_range = [
                vds_data.getFloatVector("StartFreqs")[0],
                vds_data.getFloatVector("EndFreqs")[-1]
            ]
            cimager_parset = patch_parset(
                parset,
                {
                    'dataset': vds_data.getString("FileName"),
                    'Images.frequency': str(frequency_range),
                    'msDirType': vds_data.getString("Extra.FieldDirectionType"),
                    'msDirRa': vds_data.getStringVector(
                        "Extra.FieldDirectionRa"
                    )[0],
                    'msDirDec': vds_data.getStringVector(
                        "Extra.FieldDirectionDec"
                    )[0]
                }
            )

            #                 Convert populated parset into ASKAP cimager format
            # ------------------------------------------------------------------
            try:
                self.logger.debug("Converting parset")
                converted_parset = mkstemp()[1]
                convert_process = Popen(
                    [convert_exec, cimager_parset, converted_parset],
                    stdout=PIPE, stderr=PIPE
                )
                sout, serr = convert_process.communicate()
                self.logger.debug("Parset conversion stdout: %s" % (sout,))
                self.logger.debug("Parset conversion stderr: %s" % (serr,))
                if convert_process.returncode != 0:
                    raise CalledProcessError(
                        convert_process.returncode, convert_exec
                    )
                os.unlink(cimager_parset)
            except CalledProcessError, e:
                self.logger.error(str(e))
                return 1

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
                        [imager_exec, "-inputs", converted_parset],
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
                #        that will be produced, so we read them from the parset.
                # --------------------------------------------------------------
                parset = parameterset(converted_parset)
                image_names = parset.getStringVector("Cimager.Images.Names")
                for image_name in image_names:
                    shutil.move(
                        os.path.join(working_dir, image_name),
                        os.path.join(resultsdir, image_name)
                    )
            except CalledProcessError, e:
                self.logger.error(str(e))
                return 1
            finally:
                os.unlink(converted_parset)
                shutil.rmtree(working_dir)
            return 0

if __name__ == "__main__":
    #   If invoked directly, parse command line arguments for logger information
    #                        and pass the rest to the run() method defined above
    # --------------------------------------------------------------------------
    loghost, logport = sys.argv[1:3]
    sys.exit(cimager(loghost, logport).run_with_logging(*sys.argv[3:]))
