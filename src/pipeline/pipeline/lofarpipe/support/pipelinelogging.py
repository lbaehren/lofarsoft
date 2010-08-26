#                                                       LOFAR PIPELINE FRAMEWORK
#
#                                                               Logging routines
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from __future__ import with_statement

from contextlib import contextmanager
from string import Template

import os
import time
import resource
import threading
import logging

def log_file(filename, logger, killswitch):
    """
    Do the equivalent of tail -f on filename -- ie, watch it for updates --
    and send any lines written to the file to the logger.

    killswitch is an instance of threading.Event: when set, we bail out of the
    loop.
    """
    if not os.path.exists(filename):
        open(filename, 'w').close()
    with open(filename, 'r') as f:
        while not killswitch.isSet():
            line = f.readline()
            if not line:
                f.seek(0, 2)
                time.sleep(1)
            else:
                logger.debug(line.strip())

class LogCatcher(object):
    """
    Sets up a context in which we can catch logs from individual pipeline
    process in a file, thensend then to the pipeline logger.

    This provides the basic mechanism, but requires subclassing to define
    self.log_prop and self.log_prop_filename (the name & contents of the log
    configuration file).
    """
    def __init__(self):
        raise NotImplementedError

    def __enter__(self):
        log_filename = os.path.join(
            self.working_dir, "pipeline_process.log"
        )
        with open(self.log_prop_filename, 'w') as log_prop_file:
            log_prop_file.write(
                self.log_prop.substitute(log_filename=log_filename)
            )
        local_logger = logging.getLogger(self.logger_name)
        self.killswitch = threading.Event()
        self.logging_thread = threading.Thread(
            target=log_file,
            args=(log_filename, local_logger, self.killswitch)
        )
        self.logging_thread.start()

    def __exit__(self, exc_type, exc_val, exc_tb):
        time.sleep(2)   # Wait in case any remaining data is to be flushed to log
        self.killswitch.set()
        self.logging_thread.join()

class CatchLog4CPlus(LogCatcher):
    """
    Implement a LogCatcher for log4cplus (as used by most LOFAR pipeline
    tools).
    """
    def __init__(self, working_dir, logger_name, executable_name):
        self.log_prop = Template("""
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
        self.log_prop_filename = os.path.join(
            working_dir, executable_name + ".log_prop"
        )
        self.working_dir = working_dir
        self.logger_name = logger_name

class CatchLog4CXX(LogCatcher):
    """
    Implement a LogCatcher for log4cplus (as used by ASKAP tools, eg cimager).
    """
    def __init__(self, working_dir, logger_name):
        self.log_prop = Template("""
log4j.rootLogger=DEBUG, FILE

log4j.appender.FILE=org.apache.log4j.RollingFileAppender
log4j.appender.FILE.File=$log_filename
log4j.appender.FILE.ImmediateFlush=true
log4j.appender.FILE.layout=org.apache.log4j.PatternLayout
log4j.appender.FILE.layout.ConversionPattern=%l [%-3p] - %m%n
        """)
        self.log_prop_filename = os.path.join(
            working_dir, "askap.log_cfg"
        )
        self.working_dir = working_dir
        self.logger_name = logger_name

@contextmanager
def log_time(logger):
    """
    Send information about the processing time used by code in this context to
    the specified logger.
    """
    def get_rusage():
        return [
            x + y for x, y in zip(
                resource.getrusage(resource.RUSAGE_CHILDREN),
                resource.getrusage(resource.RUSAGE_SELF)
            )
        ]

    start_time = time.time()
    start_rusage = get_rusage()
    try:
        yield
    finally:
        total_rusage = [x - y for x, y in zip(get_rusage(), start_rusage)]
        logger.info(
            "Total time %.4fs; user time: %.4fs; system time: %.4fs" % (
                time.time() - start_time, total_rusage[0], total_rusage[1]
            )
        )
        logger.debug(
            "Start time was %.4fs; end time was %.4fs" % (
                start_time, time.time()
            )
        )

def log_process_output(process_name, stdout, stderr, logger):
    """
    Log stdout/stderr from a process if they contain anything interesting.
    """
    if len(stdout.strip()) > 0:
        logger.debug("%s stdout: %s" % (process_name, stdout))
    stderr = stderr.replace("Debug: registered context Global=0\n", "")
    if len(stderr.strip()) > 0:
        logger.debug("%s stderr: %s" % (process_name, stdout))
