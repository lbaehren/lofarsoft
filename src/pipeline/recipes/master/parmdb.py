from __future__ import with_statement
import os
import tempfile
import subprocess
import shutil
import threading

# Local helpers
from lofarpipe.support.lofarrecipe import LOFARrecipe
from lofarpipe.support.ipython import LOFARTask
from lofarpipe.support.clusterlogger import clusterlogger
from lofarpipe.support.remotecommand import run_remote_command
from lofarpipe.cuisine.parset import Parset
import lofarpipe.support.utilities as utilities

template = """
create tablename="%s"
adddef Gain:0:0:Ampl  values=1.0
adddef Gain:1:1:Ampl  values=1.0
adddef Gain:0:0:Real  values=1.0
adddef Gain:1:1:Real  values=1.0
adddef DirectionalGain:0:0:Ampl  values=1.0
adddef DirectionalGain:1:1:Ampl  values=1.0
adddef DirectionalGain:0:0:Real  values=1.0
adddef DirectionalGain:1:1:Real  values=1.0
adddef AntennaOrientation values=5.497787144
quit
"""

class parmdb(LOFARrecipe):
    def __init__(self):
        super(parmdb, self).__init__()
        self.optionparser.add_option(
            '--executable',
            help="Executable for parmdbm",
            default="/opt/LofIm/daily/lofar/bin/parmdbm"
        )

    def go(self):
        self.logger.info("Starting parmdb run")
        super(parmdb, self).go()

        self.logger.info("Generating template parmdb")
        pdbdir = tempfile.mkdtemp(
            dir=self.config.get("layout", "parset_directory")
        )
        pdbfile = os.path.join(pdbdir, 'instrument')

        parmdbm_process = subprocess.Popen(
            [self.inputs['executable']],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )
        sout, serr = parmdbm_process.communicate(template % pdbfile)
        self.logger.info("parmdb stdout: %s" % (sout,))
        self.logger.info("parmdb stderr: %s" % (serr,))

        #                           Load file <-> compute node mapping from disk
        # ----------------------------------------------------------------------
        self.logger.debug("Loading map from %s" % self.inputs['args'])
        datamap = Parset(self.inputs['args'])

        data = []
        for host in datamap.iterkeys():
            for filename in datamap.getStringVector(host):
                data.append((host, filename))

        #       If an imager process fails, set the error Event & bail out later
        # ----------------------------------------------------------------------
        self.error = threading.Event()
        self.error.clear()

        command = "python %s" % (self.__file__.replace('master', 'nodes'))
        with clusterlogger(self.logger) as (loghost, logport):
            self.logger.debug("Logging to %s:%d" % (loghost, logport))
            with utilities.log_time(self.logger):
                parmdb_threads = []
                for host, ms in data:
                    parmdb_threads.append(
                        threading.Thread(
                            target=self._run_parmdb_node,
                            args=(host, command, loghost, str(logport),
                                ms, pdbfile
                            )
                        )
                    )
                [thread.start() for thread in parmdb_threads]
                self.logger.info("Waiting for parmdb threads")
                [thread.join() for thread in parmdb_threads]

        self.logger.debug("Removing template parmdb")
        shutil.rmtree(pdbdir, ignore_errors=True)

        if self.error.isSet():
            return 1
        else:
            self.outputs['mapfile'] = self.inputs['args']
            return 0

    def _run_parmdb_node(self, host, command, loghost, logport, ms, pdbfile):
        parmdb_process = run_remote_command(
            host,
            command,
            {
                "PYTHONPATH": self.config.get('deploy', 'engine_ppath'),
                "LD_LIBRARY_PATH": self.config.get('deploy', 'engine_lpath')
            },
            loghost,
            logport,
            ms,
            pdbfile
        )
        sout, serr = parmdb_process.communicate()
        if parmdb_process.returncode != 0:
            self.error.set()
        return parmdb_process.returncode

if __name__ == '__main__':
    sys.exit(parmdb().main())
