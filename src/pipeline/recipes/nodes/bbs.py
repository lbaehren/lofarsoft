from __future__ import with_statement
from lofarpipe.support.lofarnode import LOFARnode
from lofarpipe.support.utilities import log_time, read_initscript, log_file
from lofarpipe.cuisine.parset import Parset

from tempfile import mkstemp, mkdtemp
import os
import sys
import shutil
import threading
import time
import logging

from subprocess import Popen, CalledProcessError, PIPE, STDOUT
from string import Template

log_prop = Template("""
log4cplus.rootLogger=DEBUG, FILE
log4cplus.logger.TRC=TRACE9

log4cplus.appender.FILE=log4cplus::RollingFileAppender
log4cplus.appender.FILE.File=$log_filename
log4cplus.appender.FILE.ImmediateFlush=true
log4cplus.appender.FILE.MaxFileSize=10MB
log4cplus.appender.FILE.MaxBackupIndex=1
log4cplus.appender.FILE.layout=log4cplus::PatternLayout
log4cplus.appender.FILE.layout.ConversionPattern=%l [%-3p] - %m%n
""")

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

            # Dummy log_prop file to disable stupid messages
            working_dir = mkdtemp()
            self.set_up_logging(
                working_dir,
                self.logger.name + "." + os.path.basename(infile),
                os.path.basename(executable)
            )

            env = read_initscript(initscript)
            try:
                cmd = [executable, parset_filename, "0"]
                self.logger.debug("Executing BBS kernel")
                bbs_kernel_process = Popen(cmd, stdout=PIPE, stderr=PIPE, cwd=working_dir)
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
                shutil.rmtree(working_dir)

            return 0

    def set_up_logging(self, working_dir, logger_name, executable_name):
            log_filename = os.path.join(
                working_dir, "pipeline_process.log"
            )
            log_prop_filename = os.path.join(
                working_dir, executable_name + ".log_prop"
            )
            with open(log_prop_filename, 'w') as log_prop_file:
                log_prop_file.write(log_prop.substitute(log_filename=log_filename))
            local_logger = logging.getLogger(logger_name)
            logging_thread = threading.Thread(
                target=log_file,
                args=(log_filename, local_logger)
            )
            logging_thread.setDaemon(True)
            logging_thread.start()

if __name__ == "__main__":
    loghost, logport = sys.argv[1:3]
    sys.exit(bbs(loghost, logport).run_with_logging(*sys.argv[3:]))
