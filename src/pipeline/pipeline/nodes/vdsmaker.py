# Python standard library
from __future__ import with_statement
from contextlib import closing
from subprocess import check_call
from os.path import dirname
from ConfigParser import RawConfigParser
import os, errno

# Root directory for config file
from pipeline import __path__ as config_path

def make_vds(infile, clusterdesc, outfile, log_location):
    # Make VDS file for input MS in specifed location
    # The dev version of makevds appears to make more comprehensive VDS files
    # (including the FileName field) than the stable version.
    self.config = RawConfigParser()
    self.config.read("%s/pipeline.cfg" % (config_path[0],))
    executable = config.get('VDS', 'makevds')

    try:
        # Make the log directory
        os.makedirs(dirname(log_location))
    except OSError, failure:
        # It's ok if the directory already exists
        if failure.errno != errno.EEXIST:
            raise

    with closing(open(log_location, 'w')) as log:
        log.write("Executing: %s %s %s %s" %(executable, clusterdesc, infile, outfile))
        result = check_call(
            [executable, clusterdesc, infile, outfile],
            stdout=log
        )
    return result

if __name__ == "__main__":
    from sys import argv
    try:
        make_vds(argv[1], argv[2], argv[3], argv[4])
    except:
        print "Usage: vdsmaker [infile] [clusterdesc] [outfile] [logfile]"
