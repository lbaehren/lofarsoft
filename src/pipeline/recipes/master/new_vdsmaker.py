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
from lofarpipe.support.lofarrecipe import LOFARrecipe
from lofarpipe.support.clusterlogger import clusterlogger
from lofarpipe.support.group_data import load_data_map
from lofarpipe.support.remotecommand import ProcessLimiter

class new_vdsmaker(LOFARrecipe):
    def __init__(self):
        super(new_vdsmaker, self).__init__()
        self.optionparser.add_option(
            '-g', '--gvds',
            dest="gvds",
            help="Output file name"
        )
        self.optionparser.add_option(
            '--directory',
            dest="directory",
            help="Directory for output files"
        )
        self.optionparser.add_option(
            '--makevds',
            dest="makevds",
            help="makevds executable",
            default="/opt/LofIm/daily/lofar/bin/makevds"
        )
        self.optionparser.add_option(
            '--combinevds',
            dest="combinevds",
            help="combinevds executable",
            default="/opt/LofIm/daily/lofar/bin/combinevds"
        )
        self.optionparser.add_option(
            '--unlink',
            help="Unlink VDS files after combining",
            default="True"
        )
        self.optionparser.add_option(
            '--nproc',
            help="Maximum number of simultaneous processes per compute node",
            default="8"
        )

    def go(self):
        super(new_vdsmaker, self).go()

        #                           Load file <-> compute node mapping from disk
        # ----------------------------------------------------------------------
        self.logger.debug("Loading map from %s" % self.inputs['args'])
        data = load_data_map(self.inputs['args'])

        #                               Limit number of process per compute node
        # ----------------------------------------------------------------------
        self.logger.debug("Limit to %s processes/node" % self.inputs['nproc'])
        compute_nodes_lock = ProcessLimiter(self.inputs['nproc'])

        if self.inputs['unlink'] == "False":
            self.inputs['unlink'] = False

        try:
            os.makedirs(self.inputs['directory'])
        except OSError, failure:
            if failure.errno != errno.EEXIST:
                raise

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
            self.logger.warn("Failed imager process detected")
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
            sour, serr = combineproc.communicate()
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
        else:
            return 0

if __name__ == '__main__':
    sys.exit(new_vdsmaker().main())
