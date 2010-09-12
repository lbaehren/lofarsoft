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

from pyrap.quanta import quantity

import lofarpipe.support.lofaringredient as ingredient
from lofarpipe.support.baserecipe import BaseRecipe
from lofarpipe.support.pipelinelogging import log_time, log_process_output
from lofarpipe.support.clusterlogger import clusterlogger
from lofarpipe.support.remotecommand import run_remote_command
from lofarpipe.support.pipelinelogging import log_process_output
from lofarpipe.support.remotecommand import ProcessLimiter
from lofarpipe.support.parset import Parset
from lofarpipe.support.parset import get_parset
from lofarpipe.support.parset import patched_parset
from lofarpipe.support.utilities import spawn_process

class cimager(BaseRecipe):
    """
    Provides a convenient, pipeline-based mechanism of running the cimager on
    a dataset.

    Ingests an MWimager-style parset, converting it to cimager format as
    required.
    """
    inputs = {
        'imager_exec': ingredient.ExecField(
            '--imager-exec',
            help="cimager executable"
        ),
        'convert_exec': ingredient.ExecField(
            '--convert-exec',
            help="convertimagerparset executable"
        ),
        'parset': ingredient.FileField(
            '--parset',
            help="Imager configuration parset (mwimager format)"
        ),
        'nproc': ingredient.IntField(
            '--nproc',
            help="Maximum number of simultaneous processes per compute node",
            default=8
        ),
        'timestep': ingredient.FloatField(
            '--timestep',
            help="If non-zero, multiple images will be made, each using timestep seconds of data",
            default=0.0
        ),
        'results_dir': ingredient.DirectoryField(
            '--results-dir',
            help="Directory in which resulting images will be placed",
        )
    }

    outputs = {
        'images': ingredient.ListField()
    }

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
        parset = Parset(gvds_file)

        data = []
        for part in range(parset.getInt('NParts')):
            host = parset.getString("Part%d.FileSys" % part).split(":")[0]
            vds  = parset.getString("Part%d.Name" % part)
            data.append((host, vds))

        #                               Limit number of process per compute node
        # ----------------------------------------------------------------------
        compute_nodes_lock = ProcessLimiter(self.inputs['nproc'])

        #                                 Divide data into timesteps for imaging
        #          timesteps is a list of (start, end, results directory) tuples
        # ----------------------------------------------------------------------
        timesteps = []
        results_dir = self.inputs['results_dir']
        if self.inputs['timestep'] == 0:
            self.logger.info("No timestep specified; imaging all data")
            timesteps = [("None", "None", results_dir)]
        else:
            self.logger.info("Using timestep of %s s" % self.inputs['timestep'])
            gvds = get_parset(gvds_file)
            start_time = quantity(gvds['StartTime']).get('s').get_value()
            end_time = quantity(gvds['EndTime']).get('s').get_value()
            step = float(self.inputs['timestep'])
            while start_time < end_time:
                timesteps.append(
                    (
                        str(start_time), str(start_time+step),
                        os.path.join(results_dir, str(start_time))
                    )
                )
                start_time += step

        #                          Run each cimager process in a separate thread
        # ----------------------------------------------------------------------
        command = "python %s" % (self.__file__.replace('master', 'nodes'))
        with clusterlogger(self.logger) as (loghost, logport):
            self.logger.debug("Logging to %s:%d" % (loghost, logport))
            with log_time(self.logger):
                for label, timestep in enumerate(timesteps):
                    self.logger.info("Processing timestep %d" % label)
                    start_time, end_time, resultsdir = timestep
                    imager_threads = [
                        threading.Thread(
                            target=self._dispatch_compute_job,
                            args=(host, command, compute_nodes_lock[host],
                                loghost, str(logport),
                                vds,
                                self.inputs['parset'],
                                self.inputs['convert_exec'],
                                self.inputs['imager_exec'],
                                resultsdir,
                                start_time,
                                end_time
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

    def _dispatch_compute_job(
        self, host, command, semaphore, loghost, logport, vds, template_parset,
        convert_exec, imager_exec, resultsdir, start_time, end_time
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
            vds_data = Parset(vds)
            frequency_range = [
                vds_data.getDoubleVector("StartFreqs")[0],
                vds_data.getDoubleVector("EndFreqs")[-1]
            ]
            with patched_parset(
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
            ) as cimager_parset:
                #             Convert populated parset into ASKAP cimager format
                # --------------------------------------------------------------
                try:
                    self.logger.debug("Converting parset for %s" % vds)
                    fd, converted_parset = tempfile.mkstemp(
                        dir=self.config.get("layout", "parset_directory")
                    )
                    convert_process = spawn_process(
                        [convert_exec, cimager_parset, converted_parset],
                        self.logger
                    )
                    os.close(fd)
                    sout, serr = convert_process.communicate()
                    log_process_output(convert_exec, sout, serr, self.logger)
                    if convert_process.returncode != 0:
                        raise subprocess.CalledProcessError(
                            convert_process.returncode, convert_exec
                        )
                except OSError, e:
                    self.logger.error("Failed to spawn convertimagerparset (%s)" % str(e))
                    self.error.set()
                    return 1
                except subprocess.CalledProcessError, e:
                    self.logger.error(str(e))
                    self.error.set()
                    return 1

            #                                Run cimager process on compute node
            # ------------------------------------------------------------------
            engine_ppath = self.config.get('deploy', 'engine_ppath')
            engine_lpath = self.config.get('deploy', 'engine_lpath')
            try:
                cimager_process = run_remote_command(
                    self.logger,
                    host,
                    command,
                    {
                        "PYTHONPATH": self.config.get('deploy', 'engine_ppath'),
                        "LD_LIBRARY_PATH": self.config.get('deploy', 'engine_lpath')
                    },
                    loghost,
                    str(logport),
                    imager_exec,
                    vds,
                    converted_parset,
                    resultsdir,
                    str(start_time),
                    str(end_time)
                )
                sout, serr = cimager_process.communicate()
                serr = serr.replace("Connection to %s closed.\r\n" % host, "")
                log_process_output("SSH session (cimager)", sout, serr, self.logger)
            except Exception, e:
                self.logger.error(str(e))
                self.error.set()
            log_process_output("Remote cimager", sout, serr, self.logger)

            #                          Copy names of created images into outputs
            # ------------------------------------------------------------------
            parset = Parset(converted_parset)
            image_names = parset.getStringVector("Cimager.Images.Names")
            self.outputs['images'].extend(image_names)
            os.unlink(converted_parset)

        finally:
            semaphore.release()
        if cimager_process.returncode != 0:
            self.logger.error(
                "cimager failed: returned %d" % cimager_process.returncode
            )
            self.error.set()
        return cimager_process.returncode

if __name__ == '__main__':
    sys.exit(cimager().main())
