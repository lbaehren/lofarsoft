from __future__ import with_statement
from lofarpipe.support.lofarnode import LOFARnode
from lofarpipe.support.utilities import log_time
from lofarpipe.support.pipelinelogging import CatchLog4CPlus
from subprocess import Popen, CalledProcessError, PIPE, STDOUT
import shutil, os.path, tempfile
import sys


class sourcedb(LOFARnode):
    def run(self, executable, infile, catalogue):
        with log_time(self.logger):
            self.logger.info("Processing %s" % (infile,))

            output = os.path.join(infile, "sky")

            # Remove any old sky database
            shutil.rmtree(output, ignore_errors=True)

            working_dir = tempfile.mkdtemp()
            try:
                cmd = [executable, "format=<", "in=%s" % (catalogue), "out=%s" % (output)]
                with CatchLog4CPlus(
                    working_dir,
                    self.logger.name + "." + os.path.basename(infile),
                    os.path.basename(executable)
                ):
                    makesourcedb_process = Popen(cmd, stdout=PIPE, stderr=PIPE, cwd=working_dir)
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
            finally:
                shutil.rmtree(working_dir)

        return 0

if __name__ == "__main__":
    #   If invoked directly, parse command line arguments for logger information
    #                        and pass the rest to the run() method defined above
    # --------------------------------------------------------------------------
    loghost, logport = sys.argv[1:3]
    sys.exit(sourcedb(loghost, logport).run_with_logging(*sys.argv[3:]))
