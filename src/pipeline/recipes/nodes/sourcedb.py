from __future__ import with_statement
from lofarpipe.support.lofarnode import LOFARnode
from lofarpipe.support.utilities import log_time
from subprocess import Popen, CalledProcessError, PIPE, STDOUT
import shutil, os.path

class sourcedb(LOFARnode):
    def run(self, executable, infile, catalogue):
        with log_time(self.logger):
            self.logger.info("Processing %s" % (infile,))

            output = os.path.join(infile, "sky")

            # Remove any old sky database
            shutil.rmtree(output, ignore_errors=True)

            try:
                cmd = [executable, "format=<", "in=%s" % (catalogue), "out=%s" % (output)]
                makesourcedb_process = Popen(cmd, stdout=PIPE, stderr=PIPE)
                sout, serr = makesourcedb_process.communicate()
                self.logger.debug("makesourcedb stdout: %s" % (sout,))
                self.logger.debug("makesourcedb stderr: %s" % (serr,))
                if makesourcedb_process.returncode != 0:
                    raise CalledProcessError(makesourcedb_process.returncode, executable)
            except CalledProcessError, e:
                # For CalledProcessError isn't properly propagated by IPython
                # Temporary workaround...
                self.logger.error(str(e))
                raise Exception

        return 0

