#                                                         LOFAR IMAGING PIPELINE
#
#                                                  cimager (ASKAP imager) recipe
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from __future__ import with_statement

import os
import sys
import threading
import collections
import subprocess
import tempfile

from lofar.parameterset import parameterset

from lofarpipe.support.lofarrecipe import LOFARrecipe
from lofarpipe.support.pipelinelogging import log_time
from lofarpipe.support.clusterlogger import clusterlogger
from lofarpipe.support.utilities import patch_parset
from lofarpipe.support.remotecommand import run_remote_command
from lofarpipe.support.remotecommand import ProcessLimiter

class cimager(LOFARrecipe):
    """
    Provides a convenient, pipeline-based mechanism of running the cimager on
    a dataset.

    Ingests an MWimager-style parset, converting it to cimager format as
    required.
    """
    def __init__(self):
        super(cimager, self).__init__()
        self.optionparser.add_option(
            '--imager-exec',
            help="cimager executable"
        )
        self.optionparser.add_option(
            '--convert-exec',
            help="convertimagerparset executable"
        )
        self.optionparser.add_option(
            '--parset',
            help="Imager configuration parset (mwimager format)"
        )
        self.optionparser.add_option(
            '--nproc',
            help="Maximum number of simultaneous processes per compute node",
            default="8"
        )

    def go(self):
        self.logger.info("Starting cimager run")
        super(cimager, self).go()
        self.outputs['images' ] = []

        #              Build a GVDS file describing all the data to be processed
        # ----------------------------------------------------------------------
        self.logger.debug("Building VDS file describing all data for cimager")
        gvds_file = os.path.join(
            self.config.get("layout", "vds_directory"), "cimager.gvds"
        )
        self.run_task('vdsmaker', self.inputs['args'], gvds=gvds_file, unlink="False")
        self.logger.debug("cimager GVDS is %s" % (gvds_file,))

        #                            Read data for processing from the GVDS file
        # ----------------------------------------------------------------------
        parset = parameterset(gvds_file)

        data = []
        for part in range(parset.getInt('NParts')):
            host = parset.getString("Part%d.FileSys" % part).split(":")[0]
            vds  = parset.getString("Part%d.Name" % part)
            data.append((host, vds))

        #                               Limit number of process per compute node
        # ----------------------------------------------------------------------
        compute_nodes_lock = ProcessLimiter(self.inputs['nproc'])

        #       If an imager process fails, set the error Event & bail out later
        # ----------------------------------------------------------------------
        self.error = threading.Event()
        self.error.clear()

        #                          Run each cimager process in a separate thread
        # ----------------------------------------------------------------------
        command = "python %s" % (self.__file__.replace('master', 'nodes'))
        with clusterlogger(self.logger) as (loghost, logport):
            self.logger.debug("Logging to %s:%d" % (loghost, logport))
            with log_time(self.logger):
                imager_threads = [
                    threading.Thread(
                        target=self._run_cimager_node,
                        args=(host, compute_nodes_lock[host], command,
                            loghost, str(logport),
                            vds,
                            self.inputs['parset'],
                            self.inputs['convert_exec'],
                            self.inputs['imager_exec'],
                            self.config.get('layout', 'results_directory')
                        )
                    )
                    for host, vds in data
                ]
                [thread.start() for thread in imager_threads]
                self.logger.info("Waiting for imager threads")
                [thread.join() for thread in imager_threads]

        #                Check if we recorded a failing process before returning
        # ----------------------------------------------------------------------
        if self.error.isSet():
            self.logger.warn("Failed imager process detected")
            return 1
        else:
            return 0

    def _run_cimager_node(
        self, host, semaphore, command, loghost, logport, vds,
        template_parset, convert_exec, imager_exec, resultsdir
    ):
        """
        Run cimager on host for the data described in vds using the template
        parset provided.

        We use a semaphore to limit the number of simultaneous processes per
        node.
        """
        # The cimager parset is generated here, rather than on the node, so that
        #               we can scrape it to discover the image names produced(!)
        # ----------------------------------------------------------------------
        semaphore.acquire()
        try:
            #        Patch information required for imaging into template parset
            # ------------------------------------------------------------------
            self.logger.debug("Creating parset for %s" % vds)
            vds_data = parameterset(vds)
            frequency_range = [
                vds_data.getDoubleVector("StartFreqs")[0],
                vds_data.getDoubleVector("EndFreqs")[-1]
            ]
            cimager_parset = patch_parset(
                template_parset,
                {
                    'dataset': vds_data.getString("FileName"),
                    'Images.frequency': str(frequency_range),
                    'msDirType': vds_data.getString("Extra.FieldDirectionType"),
                    'msDirRa': vds_data.getStringVector(
                        "Extra.FieldDirectionRa"
                    )[0],
                    'msDirDec': vds_data.getStringVector(
                        "Extra.FieldDirectionDec"
                    )[0],
                    'restore': 'True' # cimager bug: non-restored image unusable
                }
            )
            self.logger.debug(cimager_parset)

            #                 Convert populated parset into ASKAP cimager format
            # ------------------------------------------------------------------
            try:
                self.logger.debug("Converting parset for %s" % vds)
                converted_parset = tempfile.mkstemp(
                    dir=self.config.get("layout", "parset_directory")
                )[1]
                convert_process = subprocess.Popen(
                    [convert_exec, cimager_parset, converted_parset],
                    stdout=subprocess.PIPE, stderr=subprocess.PIPE
                )
                sout, serr = convert_process.communicate()
                self.logger.debug("Parset conversion stdout: %s" % (sout,))
                self.logger.debug("Parset conversion stderr: %s" % (serr,))
                if convert_process.returncode != 0:
                    raise subprocess.CalledProcessError(
                        convert_process.returncode, convert_exec
                    )
                os.unlink(cimager_parset)
            except subprocess.CalledProcessError, e:
                self.logger.error(str(e))
                return 1

            #                                Run cimager process on compute node
            # ------------------------------------------------------------------
            engine_ppath = self.config.get('deploy', 'engine_ppath')
            engine_lpath = self.config.get('deploy', 'engine_lpath')
            cimager_process = run_remote_command(
                host,
                command,
                {
                    "PYTHONPATH": self.config.get('deploy', 'engine_ppath'),
                    "LD_LIBRARY_PATH": self.config.get('deploy', 'engine_lpath')
                },
                loghost,
                logport,
                imager_exec,
                vds,
                converted_parset,
                resultsdir
            )
            sout, serr = cimager_process.communicate()

            #                          Copy names of created images into outputs
            # ------------------------------------------------------------------
            parset = parameterset(converted_parset)
            image_names = parset.getStringVector("Cimager.Images.Names")
            self.outputs['images'].extend(image_names)
            os.unlink(converted_parset)

        finally:
            semaphore.release()
        if cimager_process.returncode != 0:
            self.error.set()
        return cimager_process.returncode

if __name__ == '__main__':
    sys.exit(cimager().main())
