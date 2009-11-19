# Root directory for config file
from pipeline import __path__ as config_path

from twisted.python import log
import pyrap.tables

def make_columns(file):
    # Attempt to insert MODEL_DATA and CORRECTED_DATA columns into MS, in the
    # hope it'll persuade BBS to run.
    try:
        pyrap.tables.addImagingColumns(file)
    except ValueError:
        log.msg('Add imaging columns failed: they probably already exist.')

if __name__ == "__main__":
    from sys import argv
    try:
        make_columns(argv[1])
    except:
        print "Usage: colmaker [file]"


