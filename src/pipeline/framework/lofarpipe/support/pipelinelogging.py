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
import re

class SearchPattern(object):
    """
    Match the contents of LogRecords against a regular expression, keeping
    track of matching records.
    """
    def __init__(self, pattern):
        self.pattern = re.compile(pattern)
        self.zero()

    def check(self, record):
        """
        If the message attached to LogRecords record matches our pattern,
        store the record.
        """
        if self.pattern.search(record.getMessage()):
            self.results.append(record)

    def zero(self):
        """
        Reset our list of stored messages.
        """
        self.results = []

class SearchPatterns(dict):
    """
    A dictionary of SearchPattern objects.

    When a new entry is appended, it's automatically compiled into a
    SearchPattern. Other access patterns are as for a dictionary.
    """
    def __init__(self):
        # We only support "bare" init, ie no arguments.
        super(SearchPatterns, self).__init__()

    def __setitem__(self, name, pattern):
        # Compile supplied string to a SearchPattern and add it to our
        # dictionary.
        super(SearchPatterns, self).__setitem__(name, SearchPattern(pattern))

    def check(self, record):
        """
        Check the supplied LogRecord against all
        registered SearchPatetrn objects.
        """
        for pattern in self.itervalues():
            pattern.check(record)

    def zero(self, name):
        """
        Zero the counter on a given SearchPattern.
        """
        self[name].zero()

    def zero_all(self, name):
        """
        Zero the counter on all SearchPatterns registered.
        """
        for name in self.iterkeys():
            self.zero(name)

class SearchingLogger(logging.Logger):
    """
    A SearchingLogger will act as a normal logger object, forwarding
    LogRecords to the appropriate handlers. In addition, it will check the
    LogRecord against a SearchPatterns object and save any useful results.
    """
    def __init__(self, *args, **kwargs):
        logging.Logger.__init__(self, *args, **kwargs)
        self.searchpatterns = SearchPatterns()

    def handle(self, record):
        logging.Logger.handle(self, record)
        self.searchpatterns.check(record)

def getSearchingLogger(name):
    """
    Return an instance of SearchingLogger with the given name.

    Equivalent to logging.getLogger, but returns a SearchingLogger.
    """
    old_class = logging.getLoggerClass()
    logging.setLoggerClass(SearchingLogger)
    try:
        return logging.getLogger(name)
    finally:
        logging.setLoggerClass(old_class)

def log_file(filename, logger, killswitch):
    """
    Do the equivalent of tail -f on filename -- ie, watch it for updates --
    and send any lines written to the file to the logger.

    killswitch is an instance of threading.Event: when set, we bail out of the
    loop.

    :param filename: Full path to file to watch
    :param logger: Logger to which to send updates
    :param killswitch: instance of :class:`threading.Event` -- stop watching file when set
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
    process in a file, then send then to the pipeline logger.

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
        return local_logger

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
#log4cplus.appender.FILE.MaxBackupIndex=1
#log4cplus.appender.FILE.layout=log4cplus::PatternLayout
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

    :param logger: logger to which timing information should be sent.
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

def log_process_output(process_name, sout, serr, logger):
    """
    Log stdout/stderr from a process if they contain anything interesting --
    some line-noise produced by many CEP executables is stripped.

    :param process_name: Name to be used for logging purposes
    :param sout: Standard out to log (string)
    :param serr: Standard error to log (string)
    :param logger: Logger to which messages should be sent

    The ``sout`` and ``serr`` params are intended to be used with the output
    of :meth:`subprocess.Popen.communicate`, but any string-a-like should
    work.
    """
    #     These lines are never logged, since they don't tell us anything useful
    # --------------------------------------------------------------------------
    excludepatterns = (
        "Debug: registered context Global=0\n",
        "tcgetattr: Invalid argument\n",
    )

    for pattern in excludepatterns:
        sout = sout.replace(pattern, "")
        serr = serr.replace(pattern, "")

    if len(sout.strip()) > 0:
        logger.debug("%s stdout: %s" % (process_name, sout))
    if len(serr.strip()) > 0:
        logger.debug("%s stderr: %s" % (process_name, serr))
