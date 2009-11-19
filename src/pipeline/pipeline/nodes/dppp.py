# Python standard library
from __future__ import with_statement
from contextlib import closing
from subprocess import check_call
from tempfile import mkstemp
from os.path import dirname
import os, errno

# External
from cuisine.parset import Parset

def run_dppp(infile, outfile, parset, log_location):
    executable = "/app/lofar/stable/bin/CS1_IDPPP"

    # We need to patch the parset with the correct input/output MS names.
    temp_parset_filename = mkstemp()[1]
    temp_parset = Parset()
    temp_parset.readFromFile(parset)
    temp_parset['msin'] = infile
    temp_parset['msout'] = outfile
    temp_parset.writeToFile(temp_parset_filename)

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
    finally:
        os.unlink(temp_parset_filename)

if __name__ == "__main__":
    from sys import argv
    run_dppp(argv[1], argv[2], argv[3], argv[4])
