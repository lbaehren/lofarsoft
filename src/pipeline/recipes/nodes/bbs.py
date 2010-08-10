from __future__ import with_statement
from lofarpipe.support.lofarnode import LOFARnode
from lofarpipe.support.utilities import log_time, read_initscript
from lofarpipe.cuisine.parset import Parset

from tempfile import mkstemp
import os
import sys

from subprocess import Popen, CalledProcessError, PIPE, STDOUT

def get_mountpoint(path):
    return path if os.path.ismount(path) else get_mountpoint(os.path.abspath(os.path.join(path, os.pardir)))

class bbs(LOFARnode):
    def run(self, executable, initscript, infile, key, db_name, db_user, db_host):
        with log_time(self.logger):
            self.logger.info("Processing %s" % (infile,))

            self.logger.debug("Setting up kernel parset")
            filesystem = "%s:%s" % (os.uname()[1], get_mountpoint(infile))
            parset_filename = mkstemp()[1]
            kernel_parset = Parset()
            for key, value in {
                "ObservationPart.Filesystem": filesystem,
                "ObservationPart.Path": infile,
                "BBDB.Key": key,
                "BBDB.Name": db_name,
                "BBDB.User": db_user,
                "BBDB.Host": db_host,
                "ParmDB.Sky": os.path.join(infile, "sky"),
                "ParmDB.Instrument": os.path.join(infile, "instrument")
            }.iteritems():
                kernel_parset[key] = value
            kernel_parset.writeToFile(parset_filename)
            self.logger.debug("Parset written to %s" % (parset_filename,))

            env = read_initscript(initscript)
            try:
                cmd = [executable, parset_filename, "0"]
                self.logger.debug("Executing BBS kernel")
                bbs_kernel_process = Popen(cmd, stdout=PIPE, stderr=PIPE)
                sout, serr = bbs_kernel_process.communicate()
                self.logger.debug("BBS kernel stdout: %s" % (sout,))
                self.logger.debug("BBS kernel stderr: %s" % (serr,))
                if bbs_kernel_process.returncode != 0:
                    raise CalledProcessError(bbs_kernel_process.returncode, executable)
            except CalledProcessError, e:
                self.logger.error(str(e))
                return 1
            finally:
                os.unlink(parset_filename)

            return 0

if __name__ == "__main__":
    loghost, logport = sys.argv[1:3]
    sys.exit(bbs(loghost, logport).run_with_logging(*sys.argv[3:]))
