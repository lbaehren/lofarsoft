#                                                         LOFAR IMAGING PIPELINE
#
#                                     New vdsmaker recipe: fixed node allocation
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from __future__ import with_statement
import sys
import os
import tempfile
import errno
import subprocess
import threading

import lofarpipe.support.utilities as utilities
import lofarpipe.support.lofaringredient as ingredient

from lofarpipe.support.baserecipe import BaseRecipe
from lofarpipe.support.remotecommand import RemoteCommandRecipeMixIn
from lofarpipe.support.remotecommand import ProcessLimiter
from lofarpipe.support.clusterlogger import clusterlogger
from lofarpipe.support.group_data import load_data_map
from lofarpipe.support.pipelinelogging import log_process_output

class new_vdsmaker(BaseRecipe, RemoteCommandRecipeMixIn):
    inputs = {
        'gvds': ingredient.StringField(
            '-g', '--gvds',
            help="Output file name"
        ),
        'directory': ingredient.DirectoryField(
            '--directory',
            help="Output directory"
        ),
        'makevds': ingredient.ExecField(
            '--makevds',
            help="makevds executable"
        ),
        'combinevds': ingredient.ExecField(
            '--combinevds', help="combinevds executable"
        ),
        'unlink': ingredient.BoolField(
            '--unlink',
            help="Unlink VDS files after combining",
            default=True
        ),
        'nproc': ingredient.IntField(
            '--nproc',
            help="Maximum number of simultaneous processes per compute node",
            default=8
        )
    }

    outputs = {
        'gvds': ingredient.FileField()
    }

    def go(self):
        super(new_vdsmaker, self).go()

        #                           Load file <-> compute node mapping from disk
        # ----------------------------------------------------------------------
        self.logger.debug("Loading map from %s" % self.inputs['args'][0])
        data = load_data_map(self.inputs['args'][0])

        #                               Limit number of process per compute node
        # ----------------------------------------------------------------------
        self.logger.debug("Limit to %s processes/node" % self.inputs['nproc'])
        compute_nodes_lock = ProcessLimiter(self.inputs['nproc'])

        command = "python %s" % (
            self.__file__.replace('master', 'nodes').replace('new_vdsmaker', 'vdsmaker')
        )
        with clusterlogger(self.logger) as (loghost, logport):
            self.logger.debug("Logging to %s:%d" % (loghost, logport))
            with utilities.log_time(self.logger):
                vdsmaker_threads = []
                vdsnames = []
                for host, ms in data:
                    vdsnames.append(
                        "%s/%s.vds" % (self.inputs['directory'], os.path.basename(ms))
                    )
                    vdsmaker_threads.append(
                        threading.Thread(
                            target=self._dispatch_compute_job,
                            args=(host, command, compute_nodes_lock[host],
                                loghost, str(logport),
                                ms,
                                self.config.get('cluster', 'clusterdesc'),
                                vdsnames[-1],
                                self.inputs['makevds']
                            )
                        )
                    )
                [thread.start() for thread in vdsmaker_threads]
                self.logger.info("Waiting for vdsmaker threads")
                [thread.join() for thread in vdsmaker_threads]

        if self.error.isSet():
            self.logger.warn("Failed vdsmaker process detected")
            return 1

        # Combine VDS files to produce GDS
        failure = False
        self.logger.info("Combining VDS files")
        executable = self.inputs['combinevds']
        gvds_out = self.inputs['gvds']
        try:
            command = [executable, gvds_out] + vdsnames
            combineproc = subprocess.Popen(
                command,
                close_fds=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE
            )
            sout, serr = combineproc.communicate()
            log_process_output(executable, sout, serr, self.logger)
            if combineproc.returncode != 0:
                raise subprocess.CalledProcessError(combineproc.returncode, command)
            self.outputs['gvds'] = gvds_out
        except subprocess.CalledProcessError, cpe:
            self.logger.exception("combinevds failed with status %d: %s" % (cpe.returncode, serr))
            failure = True
        finally:
            if self.inputs["unlink"]:
                self.logger.debug("Unlinking temporary files")
                for file in vdsnames:
                    os.unlink(file)
            self.logger.info("vdsmaker done")
        if failure:
            self.logger.info("Failure was set")
            return 1
        elif not self.outputs.complete():
            self.logger.info("Outputs incomplete")
        else:
            return 0

if __name__ == '__main__':
    sys.exit(new_vdsmaker().main())
