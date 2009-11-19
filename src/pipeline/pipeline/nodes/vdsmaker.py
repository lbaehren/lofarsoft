# Python standard library
from __future__ import with_statement
from contextlib import closing
from subprocess import check_call
from ConfigParser import SafeConfigParser as ConfigParser
import os.path

from pipeline.support.lofarexceptions import ExecutableMissing
from pipeline.support.utilities import create_directory

# Root directory for config file
from pipeline import __path__ as config_path

def make_vds(infile, clusterdesc, outfile, log_location):
    # Make VDS file for input MS in specifed location
    # The dev version of makevds appears to make more comprehensive VDS files
    # (including the FileName field) than the stable version.
    config = ConfigParser()
    config.read(os.path.join(config_path[0], 'pipeline.cfg'))
    executable = config.get('vds', 'makevds')

    create_directory(os.path.dirname(log_location))

    try:
        if not os.access(executable, os.X_OK):
            raise ExecutableMissing(executable)
        with closing(open(log_location, 'w')) as log:
            log.write("Executing: %s %s %s %s" %(executable, clusterdesc, infile, outfile))
            result = check_call(
                [executable, clusterdesc, infile, outfile],
                stdout=log
            )
        return result
    except ExecutableMissing, e:
        with closing(open(log_location, 'a')) as log:
            log.write("%s not found" % (e.args[0]))
        raise


if __name__ == "__main__":
    from sys import argv
    try:
        make_vds(argv[1], argv[2], argv[3], argv[4])
    except:
        print "Usage: vdsmaker [infile] [clusterdesc] [outfile] [logfile]"
