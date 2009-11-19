# Python standard library
from __future__ import with_statement
from contextlib import closing
from subprocess import check_call, CalledProcessError
from tempfile import mkstemp
from os.path import dirname
from ConfigParser import SafeConfigParser as ConfigParser
import os, errno

# External
from cuisine.parset import Parset
from pipeline.support.utilities import patch_parset

# Root directory for config file
from pipeline import __path__ as config_path

def run_dppp(infile, outfile, parset, log_location):
    config = ConfigParser()
    config.read(os.path.join(config_path[0], 'pipeline.cfg'))
    executable = config.get('dppp', 'executable')

    # We need to patch the parset with the correct input/output MS names.
    temp_parset_filename = patch_parset(
        parset, {'msin': infile, 'msout': outfile}
    )

    try:
        # Make the log directory
        os.makedirs(dirname(log_location))
    except OSError, failure:
        # It's ok if the directory already exists
        if failure.errno != errno.EEXIST:
            raise

    try:
        # What is the '1' for? Required by DP3...
        with closing(open(log_location, 'w')) as log:
            result = check_call(
                [executable, temp_parset_filename, '1'],
                stdout=log
            )
        return result
    except CalledProcessError, e:
        raise "DPPP failed"

    finally:
        os.unlink(temp_parset_filename)

if __name__ == "__main__":
    from sys import argv
    try:
        run_dppp(argv[1], argv[2], argv[3], argv[4])
    except:
        print "Usage: dppp [infile] [outfile] [parset] [logfile]"
