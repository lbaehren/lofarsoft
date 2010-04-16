from __future__ import with_statement
from contextlib import closing, contextmanager
from lofarpipe.cuisine.parset import Parset
from tempfile import mkstemp
import os, errno, shutil, subprocess, time, resource
from itertools import islice, repeat, chain, izip

def get_parset(parset):
    p = Parset()
    p.readFromFile(parset)
    return p

def build_available_list(listname):
    # Run on engine to construct list of locally-stored data
    import os
    from IPython.kernel.engineservice import get_engine
    available = [
        filename for filename in filenames if os.access(filename, os.R_OK)
    ]
    properties = get_engine(id).properties
    properties[listname] = available

def clear_available_list(listname):
    from IPython.kernel.engineservice import get_engine
    del(get_engine(id).properties[listname])

def check_for_path(properties, dependargs):
    try:
        return dependargs[0] in properties[dependargs[1]]
    except NameError:
        return False

def patch_parset(parset, data, output_dir=None):
    # We want to be able to update parsets on the fly, for instance to add the
    # names of data files which need processing into a standard configuration
    temp_parset_filename = mkstemp(dir=output_dir)[1]
    temp_parset = get_parset(parset)
    for k, v in data.items():
        temp_parset[k] = v
    temp_parset.writeToFile(temp_parset_filename)
    return temp_parset_filename

def create_directory(dirname):
    # Recursively create a directory, without failing if it already exists
    try:
        os.makedirs(dirname)
    except OSError, failure:
        if failure.errno != errno.EEXIST:
            raise

def move_log(log_file, destination):
    create_directory(destination)
    shutil.move(log_file, destination)

def read_initscript(filename, shell="/bin/sh"):
    # Return a dict of the environment after sourcing
    # the given script in a shell.
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

@contextmanager
def locked_ms(ms_name):
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

@contextmanager
def log_time(logger):
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

def string_to_list(my_string):
    """
    Convert a list-like string (as in pipeline.cfg) to a list of values.
    """
    return [x.strip() for x in my_string.strip('[] ').split(',')]

def izip_longest(*args, **kwds):
    # This code is lifted from the Python 2.6 manual, since izip_longest isn't
    # available in the 2.5 standard library.
    # izip_longest('ABCD', 'xy', fillvalue='-') --> Ax By C- D-
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
