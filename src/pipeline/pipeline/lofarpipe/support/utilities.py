#                                                       LOFAR PIPELINE FRAMEWORK
#
#                                                               Utility routines
#                                                         John Swinbank, 2009-10
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from __future__ import with_statement

from itertools import islice, repeat, chain, izip
from contextlib import closing, contextmanager

import os
import errno
import shutil
import subprocess

#                                                                  Compatibility
#                               The following used to be defined in this module;
#                        they are now included so as not to break existing code.
# ------------------------------------------------------------------------------
from lofarpipe.support.pipelinelogging import log_time
from lofarpipe.support.parset import get_parset, patch_parset

#                                                File and Directory Manipulation
# ------------------------------------------------------------------------------

def get_mountpoint(path):
    return path if os.path.ismount(path) else get_mountpoint(
        os.path.abspath(os.path.join(path, os.pardir))
    )

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

def is_iterable(obj):
    """
    Return True if the given object is iterable, else False.
    """
    try:
        iter(obj)
    except:
        return False
    else:
        return True

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
    return (
        filter(lambda x: x is not None, x)
        for x in izip_longest(
            *[islice(iterable, n, None, size) for n in xrange(size)]
        )
    )

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

