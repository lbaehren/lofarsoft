#                                                       LOFAR PIPELINE FRAMEWORK
#
#                                                               Utility routines
#                                                         John Swinbank, 2009-10
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from __future__ import with_statement

from itertools import islice, repeat, chain, izip
from contextlib import closing, contextmanager
from tempfile import mkstemp
from string import Template

import os
import errno
import shutil
import subprocess
import time
import resource
import threading
import logging

from lofarpipe.cuisine.parset import Parset

#                                                                        Logging
# ------------------------------------------------------------------------------

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

@contextmanager
def catch_log4cplus(working_dir, logger_name, executable_name):
        """
        Set up a log4cplus log_prop file for executable_name in working dir.

        Set up a context in which we monitor the resulting log file in a
        separate thread, and send the reuslts to a logger called logger_name.

        When the context finishes, terminate the monitoring thread.
        """
        log_filename = os.path.join(
            working_dir, "pipeline_process.log"
        )
        log_prop_filename = os.path.join(
            working_dir, executable_name + ".log_prop"
        )
        with open(log_prop_filename, 'w') as log_prop_file:
            log_prop_file.write(log_prop.substitute(log_filename=log_filename))
        local_logger = logging.getLogger(logger_name)
        killswitch = threading.Event()
        logging_thread = threading.Thread(
            target=log_file,
            args=(log_filename, local_logger, killswitch)
        )
        logging_thread.start()
        yield
        killswitch.set()
        logging_thread.join()

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

#                                                            Parset Manipulation
# ------------------------------------------------------------------------------

def get_parset(parset_filename):
    """
    Read a parset from a parset_filename and return the result as an instance
    of lofarpipe.cuisine.Parset to the caller.
    """
    p = Parset()
    p.readFromFile(parset_filename)
    return p

def patch_parset(parset, data, output_dir=None):
    """
    Generate a parset file by adding the contents of the data dictionary to
    the specified parset object. Write it to file, and return the filename.
    """
    # We want to be able to update parsets on the fly, for instance to add the
    # names of data files which need processing into a standard configuration
    temp_parset_filename = mkstemp(dir=output_dir)[1]
    temp_parset = get_parset(parset)
    for k, v in data.items():
        temp_parset[k] = v
    temp_parset.writeToFile(temp_parset_filename)
    return temp_parset_filename

#                                                File and Directory Manipulation
# ------------------------------------------------------------------------------

def create_directory(dirname):
    """
    Recursively create a directory, without failing if it already exists.
    """
    try:
        os.makedirs(dirname)
    except OSError, failure:
        if failure.errno != errno.EEXIST:
            raise

def move_log(log_file, destination):
    """
    Move a log file into the directory destination, creating that directory
    if required.
    """
    create_directory(destination)
    shutil.move(log_file, destination)

@contextmanager
def locked_ms(ms_name):
    """
    Probably dangerous method for "locking" a MeasurementSet for the scope of
    the given context.

    Deprecated.
    """
    # If this fails, we'll raise an OSError 17 (EEXIST)
    lock_extension = "_lock"
    lock_name = os.path.join(
        ms_name,
        lock_extension
    )
    try:
        my_fd = os.open(lock_name, os.O_EXCL | os.O_CREAT)
        yield ms_name
        os.close(my_fd)
    finally:
        os.unlink(lock_name)

#                                                    IPython Dependency Checking
# ------------------------------------------------------------------------------

def build_available_list(listname):
    """
    This can be pushed to an IPython engine and run there to generate a list
    of data which is locally stored (and hence available for processing).
    """
    import os
    from IPython.kernel.engineservice import get_engine
    available = [
        filename for filename in filenames if os.access(filename, os.R_OK)
    ]
    properties = get_engine(id).properties
    properties[listname] = available

def clear_available_list(listname):
    """
    Delete lists of locally stored data to free up resources on the engine.
    """
    from IPython.kernel.engineservice import get_engine
    del(get_engine(id).properties[listname])

def check_for_path(properties, dependargs):
    """
    Run on IPython engine to check for the existence of a given path in the
    locally available data, as recorded by build_available_list().

    Used for dependency checking when scheduling jobs.
    """
    try:
        return dependargs[0] in properties[dependargs[1]]
    except NameError:
        return False

#                                                       Iterators and Generators
# ------------------------------------------------------------------------------

try:
    from itertools import izip_longest
except ImportError:
    def izip_longest(*args, **kwds):
        """
        This code is lifted from the Python 2.6 manual, since izip_longest
        isn't available in the 2.5 standard library.
        izip_longest('ABCD', 'xy', fillvalue='-') --> Ax By C- D-
        """
        fillvalue = None
        def sentinel(counter = ([fillvalue]*(len(args)-1)).pop):
            yield counter()         # yields the fillvalue, or raises IndexError
        fillers = repeat(fillvalue)
        iters = [chain(it, sentinel(), fillers) for it in args]
        try:
            for tup in izip(*iters):
                yield tup
        except IndexError:
            pass

def group_iterable(iterable, size):
    """
    Group the iterable into tuples of given size.  Returns a generator.

    Example:
    >>> for x in group([1,2,3,4,5], 3): print x
    (1, 2, 3)
    (4, 5)
    """
    return (filter(lambda x: x is not None, x) for x in izip_longest(*[islice(iterable, n, None, size) for n in xrange(size)]))

#                                                                  Miscellaneous
# ------------------------------------------------------------------------------

def read_initscript(filename, shell="/bin/sh"):
    """
    Return a dict of the environment after sourcing the given script in a shell.
    """
    p = subprocess.Popen(
        ['. %s ; env' % (filename)],
        shell=True,
        executable=shell,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        close_fds=True
    )
    so, se = p.communicate()
    return dict([x.split('=', 1) for x in so.strip().split('\n')])

def string_to_list(my_string):
    """
    Convert a list-like string (as in pipeline.cfg) to a list of values.
    """
    return [x.strip() for x in my_string.strip('[] ').split(',')]

