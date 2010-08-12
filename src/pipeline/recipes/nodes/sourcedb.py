from __future__ import with_statement
from lofarpipe.support.lofarnode import LOFARnode
from lofarpipe.support.utilities import log_time, log_prop
from subprocess import Popen, CalledProcessError, PIPE, STDOUT
import shutil, os.path, tempfile


class sourcedb(LOFARnode):
    def run(self, executable, infile, catalogue):
        with log_time(self.logger):
            self.logger.info("Processing %s" % (infile,))

            output = os.path.join(infile, "sky")

            # Remove any old sky database
            shutil.rmtree(output, ignore_errors=True)

#            # Dummy log_prop file to disable stupid messages
            working_dir = tempfile.mkdtemp()
#            log_prop_filename = os.path.join(
#                working_dir, os.path.basename(executable) + ".log_prop"
#            )
#            with open(log_prop_filename, 'w') as log_prop_file:
#                log_prop_file.write(log_prop)


            try:
                cmd = [executable, "format=<", "in=%s" % (catalogue), "out=%s" % (output)]
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

